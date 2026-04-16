//
// Created by kaede on 2026/4/12.
//

#include "DirectoryWatcher.hpp"

#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <array>
#include "../core/path.hpp"
#include "Log.hpp"

namespace core {
    namespace {
        constexpr DWORD kBufferSize = 16 * 1024;
        constexpr DWORD kNotifyFilter =
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE;
    }

    DirectoryWatcher::DirectoryWatcher(const std::filesystem::path& directory)
        : directory_(std::move(ProjectPaths::normalize(directory)))
    {
        std::error_code ec;
        if (!std::filesystem::exists(directory_, ec) || ec) {
            throw std::invalid_argument("Directory not exists");
        }
        if (!std::filesystem::is_directory(directory_, ec) || ec) {
            throw std::invalid_argument("Path must be a directory");
        }
    }

    DirectoryWatcher::~DirectoryWatcher() {
        stop();
    }

    bool DirectoryWatcher::start() {
        if (running_.load()) {
            return true;
        }

        HANDLE handle = CreateFileW(
            directory_.wstring().c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );
        if (handle == INVALID_HANDLE_VALUE) return false;

        directory_handle_ = handle;
        running_.store(true);
        try {
            watch_thread_ = std::thread(&DirectoryWatcher::watchLoop, this);
        } catch (...) {
            running_ = false;
            CloseHandle(directory_handle_);
            directory_handle_ = nullptr;
            return false;
        }

        Log::getInstance().write("Watcher","Start watching "+directory_.string());
        return true;
    }

    void DirectoryWatcher::watchLoop() {
        std::array<char, kBufferSize> buffer{};
        while (running_.load()) {
            DWORD bytes_returned;

            const BOOL success = ReadDirectoryChangesW(
                directory_handle_,
                buffer.data(),
                buffer.size(),
                FALSE,
                kNotifyFilter,
                &bytes_returned,
                nullptr,
                nullptr
            );
            if (!success) return;
            if (bytes_returned == 0) continue;

            const auto* notify = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(buffer.data());
            while (true) {
                const std::wstring changed_name(
                    notify->FileName,
                    notify->FileNameLength / sizeof(WCHAR)
                );

                const auto changed_path = ProjectPaths::normalize(directory_ / changed_name);

                switch (notify->Action) {
                    case FILE_ACTION_ADDED:
                    case FILE_ACTION_MODIFIED:
                    case FILE_ACTION_RENAMED_NEW_NAME:
                        enqueueDirtyFile(changed_path);
                        break;
                    default:
                        break;
                }

                if (notify->NextEntryOffset == 0) {
                    break;
                }

                notify = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>( reinterpret_cast<const std::byte*>(notify) + notify->NextEntryOffset);
            }
        }
    }

    void DirectoryWatcher::stop() noexcept {
        Log::getInstance().write("Watcher","Stopping watching");
        if (running_.load() == false) {
            if (watch_thread_.joinable()) {
                watch_thread_.join();
            }
            if (directory_handle_) {
                CloseHandle(directory_handle_);
                directory_handle_ = nullptr;
            }
            return;
        }

        running_.store(false);
        if (watch_thread_.joinable()) {
            CancelSynchronousIo(watch_thread_.native_handle());
            watch_thread_.join();
        }
        if (directory_handle_) {
            CloseHandle(directory_handle_);
            directory_handle_ = nullptr;
        }
    }

    std::vector<std::filesystem::path> DirectoryWatcher::consumeDirtyFiles() {
        std::lock_guard lock(dirty_files_mutex_);
        std::vector dirty(dirty_files_.begin(),dirty_files_.end());
        dirty_files_.clear();
        return dirty;
    }

    void DirectoryWatcher::enqueueDirtyFile(const std::filesystem::path& path) {
        std::lock_guard lock(dirty_files_mutex_);
        const auto normalized = ProjectPaths::normalize(path);
        dirty_files_.insert(normalized);
    }
} // namespace core

//
// Created by kaede on 2026/4/12.
//

#ifndef OPENGLRENDERBENCH_DIRECTORYWATCHER_HPP
#define OPENGLRENDERBENCH_DIRECTORYWATCHER_HPP

#include <atomic>
#include <filesystem>
#include <mutex>
#include <thread>
#include <unordered_set>
#include <vector>

#include "noncopyable.hpp"

namespace core {
    class DirectoryWatcher : public NonCopyable {
    public:
        explicit DirectoryWatcher(const std::filesystem::path& directory);
        ~DirectoryWatcher();

        bool start();
        void stop() noexcept;

        bool isRunning() const noexcept { return running_; }
        std::vector<std::filesystem::path> consumeDirtyFiles();

    private:
        void watchLoop();
        void enqueueDirtyFile(const std::filesystem::path& path);

    private:
        std::filesystem::path directory_;
        std::atomic<bool> running_{false};
        std::thread watch_thread_;
        void* directory_handle_ = nullptr;

        std::mutex dirty_files_mutex_;
        std::unordered_set<std::filesystem::path> dirty_files_;
    };
} // namespace core

#endif //OPENGLRENDERBENCH_DIRECTORYWATCHER_HPP

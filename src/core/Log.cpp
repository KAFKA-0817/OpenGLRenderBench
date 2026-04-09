//
// Created by kaede on 2026/4/9.
//

#include "Log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace core {
    Log::Log() {
        lines_.fill("");
    }

    std::string Log::makeTimestamp() {
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm local_time{};
        localtime_s(&local_time, &now_time);

        std::ostringstream stream;
        stream << std::put_time(&local_time, "%H:%M:%S");
        return stream.str();
    }

    std::string Log::formatLine(const std::string& source, const std::string& message) {
        std::ostringstream stream;
        stream << '[' << makeTimestamp() << ']';
        if (!source.empty()) {
            stream << '[' << source << "] ";
        } else {
            stream << ' ';
        }
        stream << message;
        return stream.str();
    }

    void Log::clear() {
        std::lock_guard lock(mutex_);
        lines_.fill("");
        firstLine_ = 0;
        lineCount_ = 0;
        nextLine_ = 0;
    }

    bool Log::empty() const {
        std::lock_guard lock(mutex_);
        return lineCount_ == 0;
    }

    std::vector<std::string> Log::snapshot() const {
        std::lock_guard lock(mutex_);
        std::vector<std::string> snapshot;
        snapshot.reserve(lineCount_);
        for (std::size_t i = 0; i < lineCount_; ++i) {
            const std::size_t index = (firstLine_ + i) % lines_.size();
            snapshot.push_back(lines_[index]);
        }
        return snapshot;
    }

    void Log::write(const std::string &source, const std::string &message) {
        std::lock_guard lock(mutex_);
        lines_[nextLine_] = formatLine(source, message);
        nextLine_ = (nextLine_ + 1) % lines_.size();

        if (lineCount_ < lines_.size()) {
            ++lineCount_;
            return;
        }

        firstLine_ = nextLine_;
    }
} // core

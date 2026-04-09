//
// Created by kaede on 2026/4/9.
//

#include "Log.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace core {

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
        buffer_.clear();
    }

    bool Log::empty() const {
        std::lock_guard lock(mutex_);
        return buffer_.empty();
    }

    std::vector<std::string> Log::snapshot() const {
        std::lock_guard lock(mutex_);
        return buffer_.buffer();
    }

    void Log::write(const std::string &source, const std::string &message) {
        std::string log = formatLine(source,message);
        std::lock_guard lock(mutex_);
        buffer_.write(std::move(log));
    }

    Log::LogBuffer::LogBuffer() {
        lines_.fill("");
    }

    void Log::LogBuffer::write(std::string message) {
        lines_[nextLine_] = std::move(message);
        nextLine_ = (nextLine_ + 1) % lines_.size();
        if (nextLine_ == firstLine_) {
            firstLine_ = (firstLine_ + 1) % lines_.size();
        }
    }

    std::vector<std::string> Log::LogBuffer::buffer() const {
        if (empty()) return {};

        std::vector<std::string> ret;
        ret.reserve(lines_.size() - 1);
        std::size_t p = firstLine_;
        while (p != nextLine_) {
            ret.push_back(lines_[p]);
            p = (p + 1) % lines_.size();
        }

        return ret;
    }
} // core

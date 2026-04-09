//
// Created by kaede on 2026/4/9.
//

#ifndef PBRRENDERER_LOG_HPP
#define PBRRENDERER_LOG_HPP
#include <mutex>
#include <string>
#include <array>
#include <vector>

#include "noncopyable.hpp"

namespace core {

class Log : public NonCopyable{
public:
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    static Log& getInstance(){ static Log instance; return instance; }
    void clear();
    bool empty() const;
    std::vector<std::string> snapshot() const;
    void write(const std::string &source, const std::string &message);

private:
    Log() = default;
    static std::string makeTimestamp();
    static std::string formatLine(const std::string& source, const std::string& message);

private:
    class LogBuffer : public NonCopyable {
    public:
        LogBuffer();
        ~LogBuffer() = default;
        LogBuffer(LogBuffer&&) = delete;
        LogBuffer& operator=(LogBuffer&&) = delete;

        void write(std::string message);
        std::vector<std::string> buffer() const;
        void clear() { firstLine_ = nextLine_; }
        bool empty() const { return firstLine_ == nextLine_; }

    private:
        std::array<std::string,2048> lines_;
        std::size_t firstLine_ = 0;
        std::size_t nextLine_ = 0;
    };

private:
    LogBuffer buffer_;
    mutable std::mutex mutex_;
};

} // core

#endif //PBRRENDERER_LOG_HPP

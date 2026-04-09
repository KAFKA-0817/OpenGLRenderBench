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
    Log();
    static std::string makeTimestamp();
    static std::string formatLine(const std::string& source, const std::string& message);
private:
    std::array<std::string,2048> lines_;
    std::size_t firstLine_ = 0;
    std::size_t lineCount_ = 0;
    std::size_t nextLine_ = 0;
    mutable std::mutex mutex_;
};

} // core

#endif //PBRRENDERER_LOG_HPP

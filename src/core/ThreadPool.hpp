//
// Created by kaede on 2026/4/2.
//

#ifndef PBRRENDERER_THREADPOOL_HPP
#define PBRRENDERER_THREADPOOL_HPP
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "noncopyable.hpp"

namespace core {
    class ThreadPool :public NonCopyable{
    public:
        explicit ThreadPool(std::size_t thread_count);
        ~ThreadPool();
        ThreadPool(ThreadPool&&) noexcept = delete;
        ThreadPool& operator=(ThreadPool&&) noexcept = delete;

        void enqueue(std::function<void()> task);
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;

        std::mutex mutex_;
        std::condition_variable condition_;
        bool running_ = true;
    };
} // core

#endif //PBRRENDERER_THREADPOOL_HPP
//
// Created by kaede on 2026/4/2.
//

#include "ThreadPool.hpp"

#include <iostream>
#include <ostream>

namespace core {
    ThreadPool::ThreadPool(std::size_t thread_count) {
        if (thread_count == 0) {
            throw std::invalid_argument("thread_count must be greater than 0");
        }

        workers_.reserve(thread_count);

        for (std::size_t i=0;i<thread_count;++i) {
            workers_.emplace_back([this] {
                while (true) {
                    std::unique_lock lock(this->mutex_);
                    this->condition_.wait(lock,[this]() {
                        return !this->tasks_.empty() || !this->running_;
                    });

                    if (!this->running_ && this->tasks_.empty()) {
                        return;
                    }

                    auto task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                    lock.unlock();
                    task();
                }
            });
        }

        std::cout << "thread pool created" << std::endl;
    }

    ThreadPool::~ThreadPool() {
        {
            std::lock_guard lock(mutex_);
            running_ = false;
        }
        condition_.notify_all();

        for (auto& worker: workers_) {
            if (worker.joinable())
                worker.join();
        }
        std::cout << "thread pool destroyed" << std::endl;
    }

    void ThreadPool::enqueue(std::function<void()> task) {
        {
            std::lock_guard lock(mutex_);
            if (!running_) {
                return;
            }
        }
        if (task) {
            {
                std::lock_guard lock(mutex_);
                tasks_.emplace(std::move(task));
            }
            condition_.notify_one();
        }
    }
} // core
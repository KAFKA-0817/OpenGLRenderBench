//
// Created by kaede on 2026/3/26.
//

#ifndef PBRRENDERER_NONCOPYABLE_HPP
#define PBRRENDERER_NONCOPYABLE_HPP

namespace core {
    class NonCopyable {
    public:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;

    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;
        NonCopyable(NonCopyable&&) = default;
        NonCopyable& operator=(NonCopyable&&) = default;
    };
}


#endif //PBRRENDERER_NONCOPYABLE_HPP
//
// Created by kaede on 2026/3/26.
//

#ifndef PBRRENDERER_WINDOW_HPP
#define PBRRENDERER_WINDOW_HPP
#include <string>
#include "../../core/opengl.hpp"
#include "../../core/noncopyable.hpp"

namespace renderer {
    class Window :public core::NonCopyable{
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(Window&& other) noexcept;
        Window& operator=(Window&& other) noexcept;

        bool should_close() const;
        void swap_buffers() const;
        void poll_events() const;
        void make_context_current() const;

        int width() const noexcept { return width_; }
        int height() const noexcept { return height_; }
        GLFWwindow* native_handle() const noexcept { return window_; }

    private:
        void destroy() noexcept;
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    private:
        GLFWwindow* window_ = nullptr;
        int width_ = 0;
        int height_ = 0;
    };
};




#endif //PBRRENDERER_WINDOW_HPP
//
// Created by kaede on 2026/4/8.
//

#ifndef PBRRENDERER_EDITORCOMMANDSFRAME_HPP
#define PBRRENDERER_EDITORCOMMANDSFRAME_HPP

#include "../../core/noncopyable.hpp"

namespace app {
    struct EditorCommands {
        bool reload_shaders = false;
    };

    class EditorCommandsFrame : public core::NonCopyable {
    public:
        EditorCommandsFrame() = default;
        ~EditorCommandsFrame() = default;
        EditorCommandsFrame(EditorCommandsFrame&&) noexcept = delete;
        EditorCommandsFrame& operator=(EditorCommandsFrame&&) noexcept = delete;

        void beginFrame() noexcept { commands_ = {}; }

        EditorCommands& writable() noexcept { return commands_; }
        const EditorCommands& commands() const noexcept { return commands_; }

    private:
        EditorCommands commands_{};
    };
} // app

#endif //PBRRENDERER_EDITORCOMMANDSFRAME_HPP

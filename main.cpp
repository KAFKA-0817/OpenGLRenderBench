#include <iostream>

#include "src/renderer/core/FrameBuffer.hpp"
#include "src/renderer/asset/Model.hpp"
#include "src/core/opengl.hpp"
#include "src/core/path.hpp"
#include "src/renderer/asset/PrimitiveFactory.hpp"
#include "src/renderer/pipeline/Renderer.hpp"

#include "src/renderer/core/Shader.hpp"
#include "src/renderer/core/Window.hpp"
#include "src/renderer/camera/Camera.hpp"
#include "src/renderer/camera/PerspectiveCamera.hpp"
#include "src/renderer/material/BlinnPhongMaterial.hpp"
#include "src/renderer/material/PbrMaterial.hpp"
#include "src/renderer/material/UnlitMaterial.hpp"

using namespace renderer;

int main()
{
    core::OpenGLContext context;
    constexpr int width = 1920;
    constexpr int height = 1080;
    Window window(width,height,"PBR Renderer");
    window.make_context_current();
    core::OpenGLContext::loadGlad();
    core::OpenGLContext::enableDebugOutput();
    glViewport(0,0,window.width(),window.height());

    Model cube = PrimitiveFactory::createCube();

    Shader unlit_shader(
            core::ProjectPaths::shader("unlit.vs"),
            core::ProjectPaths::shader("unlit.fs")
        );
    UnlitMaterial unlit_material(unlit_shader);
    Shader blinn_phong_shader(
        core::ProjectPaths::shader("blinn_phong.vs"),
        core::ProjectPaths::shader("blinn_phong.fs")
    );
    BlinnPhongMaterial blinn_phong_material(blinn_phong_shader);
    PBRMaterial pbr_material;

    Renderer renderer(width,height);
    PerspectiveCamera camera;
    camera.setPosition({0,0,5});

    bool reload_key_pressed_last_frame = false;
    bool key_1_last_frame = false;
    bool key_2_last_frame = false;
    bool key_3_last_frame = false;
    bool key_4_last_frame = false;
    bool key_5_last_frame = false;
    while (!window.should_close())
    {
        window.poll_events();
        const int current_width = window.width();
        const int current_height = window.height();

        if (current_width > 0 && current_height > 0 &&
            (current_width != renderer.width() || current_height != renderer.height()))
        {
            renderer.resize(current_width, current_height);
            camera.setAspect(static_cast<float>(current_width) / static_cast<float>(current_height));
            glViewport(0, 0, current_width, current_height);
        }

        bool reload_key_pressed = glfwGetKey(window.native_handle(), GLFW_KEY_R) == GLFW_PRESS;
        if (reload_key_pressed && !reload_key_pressed_last_frame) {
            renderer.reloadBuiltinShaders();

            unlit_shader.reload();
            blinn_phong_shader.reload();
        }
        reload_key_pressed_last_frame = reload_key_pressed;

        bool key_1 = glfwGetKey(window.native_handle(), GLFW_KEY_1) == GLFW_PRESS;
        if (key_1 && !key_1_last_frame) renderer.setPreviewMode(PreviewMode::FinalScene);
        key_1_last_frame = key_1;

        bool key_2 = glfwGetKey(window.native_handle(), GLFW_KEY_2) == GLFW_PRESS;
        if (key_2 && !key_2_last_frame) renderer.setPreviewMode(PreviewMode::GPosition);
        key_2_last_frame = key_2;

        bool key_3 = glfwGetKey(window.native_handle(), GLFW_KEY_3) == GLFW_PRESS;
        if (key_3 && !key_3_last_frame) renderer.setPreviewMode(PreviewMode::GNormal);
        key_3_last_frame = key_3;

        bool key_4 = glfwGetKey(window.native_handle(), GLFW_KEY_4) == GLFW_PRESS;
        if (key_4 && !key_4_last_frame) renderer.setPreviewMode(PreviewMode::GAlbedo);
        key_4_last_frame = key_4;

        bool key_5 = glfwGetKey(window.native_handle(), GLFW_KEY_5) == GLFW_PRESS;
        if (key_5 && !key_5_last_frame) renderer.setPreviewMode(PreviewMode::GMaterial);
        key_5_last_frame = key_5;


        renderer.clearSubmissions();

        // 左：Blinn-Phong
        glm::mat4 model_blinn = glm::translate(glm::mat4(1.0f), glm::vec3(-1.8f, -0.5f, 0.0f));
        model_blinn = glm::rotate(model_blinn, glm::radians(30.0f), glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
        renderer.submit(cube, blinn_phong_material, model_blinn);

        // 中：PBR
        glm::mat4 model_pbr = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));
        model_pbr = glm::rotate(model_pbr, glm::radians(45.0f), glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f)));
        renderer.submit(cube, pbr_material, model_pbr);

        // 右：Unlit
        glm::mat4 model_unlit = glm::translate(glm::mat4(1.0f), glm::vec3(1.8f, -0.5f, 0.0f));
        model_unlit = glm::rotate(model_unlit, glm::radians(60.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f)));
        renderer.submit(cube, unlit_material, model_unlit);

        renderer.renderFrame(camera);

        window.swap_buffers();
    }

    return 0;
}

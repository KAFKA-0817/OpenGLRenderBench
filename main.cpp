#include <stdexcept>

#include "src/app/Application.hpp"
#include "src/core/path.hpp"

int main(int argc, char** argv)
{
    if (argc <= 0 || argv == nullptr || argv[0] == nullptr) {
        throw std::runtime_error("Failed to determine executable path.");
    }

    core::ProjectPaths::initialize(argv[0]);

    app::Application app({
        3840,
        2160,
        "OpenGLRenderBench"
    });
    app.run();

    return 0;
}

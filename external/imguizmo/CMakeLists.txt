add_library(imguizmo
        ImGuizmo.cpp
)

target_include_directories(imguizmo PUBLIC
        .
)

target_link_libraries(imguizmo PUBLIC
        imgui
)

cmake_minimum_required(VERSION 3.5)

project(main
	VERSION 1.0
	DESCRIPTION "Reflection Law visualization"
	LANGUAGES CXX)

set(IMGUI_PATH "${CMAKE_CURRENT_LIST_DIR}/imgui")
file(GLOB IMGUI_GLOB
    ${IMGUI_PATH}/imgui.h
    ${IMGUI_PATH}/imgui.cpp
    ${IMGUI_PATH}/imconfig.h
    ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_internal.h
    ${IMGUI_PATH}/imstb_rectpack.h
    ${IMGUI_PATH}/imstb_textedit.h
    ${IMGUI_PATH}/imstb_truetype.h
    ${IMGUI_PATH}/imgui_tables.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdl2.h
    ${IMGUI_PATH}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer2.h
    ${IMGUI_PATH}/backends/imgui_impl_sdlrenderer2.cpp
)

add_library("imgui" STATIC ${IMGUI_GLOB})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH})

find_package(SDL2 REQUIRED)

include_directories(main ${SDL2_INCLUDE_DIRS})

add_executable(main main.cpp)

target_link_libraries(main PUBLIC ${SDL2_LIBRARIES} "imgui")
target_include_directories(main PUBLIC "imgui")

cmake_minimum_required(VERSION 3.10)
project(RandomGraphGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем исполняемый файл
add_executable(RandomGraphGenerator
        main.cpp
        Graph.cpp
        DirectedGraph.cpp
        UndirectedGraph.cpp
        GraphVisualizer.cpp
        resource.rc
)

# Необходимо для WinAPI
if (WIN32)
    target_link_libraries(RandomGraphGenerator gdi32)
endif()

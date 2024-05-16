
add_executable (async_server "client.h" "client.cpp" "server.h" "server.cpp" "main.cpp")

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

link_libraries(${Boost_LIBRARIES})

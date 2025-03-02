file(GLOB CLIENT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

file(GLOB PROTO_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../*.pb.cc)

add_executable(client ${CLIENT_SRCS} ${PROTO_SRCS})

target_link_libraries(client mrpc_core ${LIBS})

target_compile_options(client PRIVATE -std=c++11 -Wall)

# 设置可执行文件目录
set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
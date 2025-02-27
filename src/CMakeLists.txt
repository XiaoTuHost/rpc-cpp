file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

file(GLOB PROTO_SRCS ${CMAKE_CURRENT_BINARY_DIR}/*pb.cc)

# 制作成静态库
add_library(mrpc_core STATIC ${SRC_FILES} ${PROTO_SRCS})

target_link_libraries(mrpc_core PUBLIC protobuf)

target_include_directories(mrpc_core PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_compile_options(mrpc_core PRIVATE -std=c++11 -Wall)
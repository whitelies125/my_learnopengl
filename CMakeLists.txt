# 1,设置工程名称，叫 Fo
project( "learn_opengl" )

# 2,设置 CMake 最低版本号，我电脑装的是3.2
cmake_minimum_required( VERSION 3.2 )

# 3,设定编译参数
set(CMAKE_CXX_STANDARD    23)  # 指定 C++ 版本
set(CMAKE_BUILD_TYPE "Release")  # 调试使用Debug，可以查看中间变量；发布使用Release，运行速度快

# 输出 compile_command.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 4,设定源码列表,查找指定目录下的所有源文件,并将名称保存到 DIR_SRCS 变量中
aux_source_directory(./src/ DIR_SRC)

# 5,设定头文件路径
include_directories(./include/)
include_directories(../glfw/include/)

# 6，把源码编译成一个可执行文件，文件名为 fo，会保存在当前目录下
add_executable( learn_opengl ${DIR_SRC} )

# link_directories("../glfw/build/src/")
target_link_libraries(learn_opengl ${CMAKE_SOURCE_DIR}/../glfw/build/src/libglfw3.a)
target_link_libraries(learn_opengl opengl32.lib)

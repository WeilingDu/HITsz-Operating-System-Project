# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.0.0)
# 项目信息
project(net VERSION 0.1.0)

# 添加include目录存放.h文件
include_directories("include")
# 添加库
add_library(alllibs src/disk.c src/file.c src/dir.c src/inode.c src/shell.c src/superblock.c)
# 指定生成目标
add_executable(main src/main.c)
# 添加链接库
target_link_libraries(main alllibs)
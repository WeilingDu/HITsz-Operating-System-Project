#ifndef DIR_H
#define DIR_H

#include <stdint.h>
#include "file.h"

#define MAX_NAME_SIZE 121

typedef struct dir_item {       // 目录项一个更常见的叫法是 dirent(directory entry)
    uint32_t inode_id;          // 当前目录项表示的文件/目录的对应inode
    uint16_t valid;             // 当前目录项是否有效 
    uint8_t type;               // 当前目录项类型（文件/目录）
    char name[MAX_NAME_SIZE];             // 目录项表示的文件/目录的文件名/目录名
}dir_item;  // 128字节

dir_item block_buffer[8];  // 1024/128=8; 块缓存; 由于不需要对普通文件内容做操作，因此需要读块的情况只可能是读文件夹文件


/**
 * @brief 初始化根目录对应的block，即数据组的block 1
 * 
 * @return int 成功返回1，否则返回0
 */
int init_root_dir_item();

/**
 * @brief 将block_buffer写到磁盘中
 * 
 * @param index 文件系统的逻辑数据块下标（该函数需要将其转换成对应的磁盘块）
 * @return int 成功返回1，否则返回0
 */
int write_dir_item(int index);

/**
 * @brief 将block index读到block_buffer中
 * 
 * @param index 文件系统的逻辑数据块下标（该函数需要将其换成对应的磁盘块）
 * @return int 成功返回1，否则返回0
 */
int read_dir_item(int index);




#endif 
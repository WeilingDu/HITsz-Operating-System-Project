#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include <stdint.h>

#define MAGIC_NUM 0xdec0de
#define MAX_BLOCK_MAP 128
#define MAX_INODE_MAP 32

/**
 * @brief 超级块
 * 
 */
typedef struct super_block {
    int32_t magic_num;                  // 幻数
    int32_t free_block_count;           // 空闲数据块数
    int32_t free_inode_count;           // 空闲inode数
    int32_t dir_inode_count;            // 目录inode数
    uint32_t block_map[MAX_BLOCK_MAP];            // 数据块占用位图
    uint32_t inode_map[MAX_INODE_MAP];             // inode占用位图
}super_block;  // 656字节; 1block

struct super_block spb;

/**
 * @brief 初始化超级块（文件系统第一次打开时）
 * 
 */
void init_sp_block();

/**
 * @brief 将超级块写入磁盘（前两个磁盘块）
 * 
 * @return int 成功返回1，否则返回0
 */
int write_sp_block();


/**
 * @brief 从磁盘中读取超级块（前两个磁盘块）
 * 
 * @return int 成功返回1，否则返回0
 */
int read_sp_block();


/**
 * @brief 分配数据块
 * 
 * @return int 成功返回数据组中数据块下标，失败返回-1
 */
int alloc_block();


#endif 


#ifndef INODE_H
#define INODE_H

#include <stdint.h>


#define _FILE_ 0  // 文件类型为文件
#define _FOLDER_ 1  // 文件类型为文件夹

#define INODE_NUM 1024  // 索引节点32 * 32 = 1024
#define INODE_BLOCKS 32  // 索引节点表需要32个数据块
#define INODE_PER_DEVICE_BLOCK 16  // 每个磁盘块可放16个inode
#define INODE_START_DEVICE_BLOCK 2  // 开始存放inode的磁盘块


typedef struct inode {
    uint32_t size;              // 文件大小
    uint16_t file_type;         // 文件类型（文件/文件夹）
    uint16_t link;              // 连接数
    uint32_t block_point[6];    // 数据块指针
}inode; // 4+2+2+4*6=32字节

inode inode_table[INODE_NUM];  // 索引节点表缓存；32 blocks;


/**
 * @brief 将inode写入磁盘块中
 * 
 * @param node 指向inode的指针
 * @param index 该inode在inode_table中的下标
 * @return int 成功返回1，否则返回0
 */
int write_inode(struct inode* node, int index);

/**
 * @brief 读取磁盘块中的inode
 * 
 * @param node 指向读取得到的inode的指针
 * @param index 该inode在inode_table中的下标
 * @return int 成功返回1，否则返回0
 */

int read_inode(struct inode* node, int index);

/**
 * @brief 初始化第一个inode，对应根目录
 * 
 * @return int 成功返回1，否则返回0
 */
int init_root_inode();

/**
 * @brief 初始化文件的inode
 * 
 * @param node inode对应的指针，一般传入inode table[i]
 * @param size inode属性size
 * @param type inode属性type
 * @param link inode属性link
 * @return int 成功返回1，否则返回0
 */
int init_inode(struct inode* node, int size, int type, int link);

/**
 * @brief 分配一个空闲的inode
 * 
 * @return int 分配成功，则返回inode下标，失败则返回-1
 */
int alloc_inode();

#endif 
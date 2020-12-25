#include <string.h>
#include <stdio.h>
#include "inode.h"
#include "superblock.h"
#include "disk.h"

int init_inode(struct inode* node, int size, int type, int link)
{
    node->size = size;
    node->link = link;
    node->file_type = type;
    for (int i = 0; i < 6; i++) {
        node->block_point[i] = 0;
    }
    return 1;
};

int init_root_inode(){
    // 设置root的inode
    struct inode root_node;
    root_node.file_type = _FOLDER_;
    root_node.size = 0;  
    root_node.link = 0;
    root_node.block_point[0] = 33;
    for (int i = 1; i < 6; i++) {
        root_node.block_point[i] = 0;
    }
    inode_table[0] = root_node;
    return 1;
}


int write_inode(struct inode* node, int index){
    int device_block_num = INODE_START_DEVICE_BLOCK + index / INODE_PER_DEVICE_BLOCK;  // 该inode对应的磁盘块
    // printf("device_block_num: %d\n", device_block_num);
    int inode_start = sizeof(inode) * (index % INODE_PER_DEVICE_BLOCK);  // 对应磁盘块开始的索引
    // printf("inode_start: %d\n", inode_start);
    char buf[DEVICE_BLOCK_SIZE];
    // 将对应的磁盘块读到buf中
    if (disk_read_block(device_block_num, buf) < 0)
    {
        return 0;
    }
    // 修改buf中对应的inode
    memcpy(&buf[inode_start], node, sizeof(inode));
    // 将buf写回对应的磁盘块
    if (disk_write_block(device_block_num, buf) < 0)
    {
        return 0;
    }
    return 1;
}

int read_inode(struct inode* node, int index){
    int device_block_num = INODE_START_DEVICE_BLOCK + index / INODE_PER_DEVICE_BLOCK;  // 该inode对应的磁盘块
    int inode_start = sizeof(inode) * (index % INODE_PER_DEVICE_BLOCK);  // 对应磁盘块开始的索引
    char buf[DEVICE_BLOCK_SIZE];
    // 将对应的磁盘块读到buf中
    if (disk_read_block(device_block_num, buf) < 0)
    {
        return 0;
    }
    // 读取buf中对应的inode到node中
    memcpy(node, &buf[inode_start], sizeof(inode));
    return 1;
}


int alloc_inode() {
    if (!spb.free_inode_count)
    {// 没有空闲的inode
        return -1;
    }
    
    for (int i = 0; i < MAX_INODE_MAP; i++) {
        uint32_t inode = spb.inode_map[i];
        for (int j = 0; j < 32; j++) {
            if ((inode>>(31-j)) & 0x00000001) continue;  // 如果该inode已占用，则继续遍历
            else {
                spb.free_inode_count--;  // 空闲inode减1
                spb.inode_map[i] |= 1 << (31-j);  // 修改inode位图 
                write_sp_block();  // 将超级块写入磁盘
                return i*32 + j;  // 返回inode的下标
            }
        }
    }
    return -1;
}
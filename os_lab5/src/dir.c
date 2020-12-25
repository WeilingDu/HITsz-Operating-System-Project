#include <string.h>
#include <stdio.h>
#include "disk.h"
#include "dir.h"
#include "inode.h"
#include "file.h"


int init_root_dir_item(){
    block_buffer[0].inode_id = 0;
    for (int i = 0; i < 6; i++)
    {
        block_buffer[i].valid = 0; // 注意，要将根目录对应的数据块中的目录项全部初始化！
    }
    
    return 1;
}

int init_dir_item(){
    for (int i = 0; i < 8; i++)
    {
        block_buffer[i].valid = 0;
    }
    
}


int write_dir_item(int index){
    int block_num = index * 2;
    char buf[DEVICE_BLOCK_SIZE*2];
    // printf("11111111 block_buffer[1].valid: %d\n", block_buffer[1].valid);
    memcpy(buf, block_buffer, BLOCK_SIZE);
    if (disk_write_block(block_num, buf) < 0) {
        printf("Write dir item failed.\n");
        return 0;
    }
    if (disk_write_block(block_num+1, &buf[DEVICE_BLOCK_SIZE]) < 0) {
        printf("Write dir item failed.\n");
        return 0;
    }
    return 1;
}


int read_dir_item(int index){
    int block_num = index * 2;
    char buf[DEVICE_BLOCK_SIZE*2];
    if (disk_read_block(block_num, buf) < 0) {
        // printf("block_num: %d\n", block_num);
        // printf("here 1\n");
        // printf("Read dir item failed.\n");
        return 0;
    }
    if (disk_read_block(block_num+1, &buf[DEVICE_BLOCK_SIZE]) < 0) {
        // printf("here 2\n");
        // printf("Read dir item failed.\n");
        return 0;
    }
    memcpy(block_buffer, buf, BLOCK_SIZE);
}
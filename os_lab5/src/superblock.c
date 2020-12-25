#include <string.h>
#include <stdio.h>
#include "disk.h"
#include "superblock.h"


void init_sp_block(){
    spb.magic_num = MAGIC_NUM;
    spb.free_block_count = 4096 - 34;  // 1 for super block, 32 for inode table, 1 for folder "root".
    spb.free_inode_count = 1024 - 1;  // 1 for folder "root".
    spb.dir_inode_count = 1;  // folder "root".
    memset(spb.inode_map, 0, 128);  // 初始化inode_map
    memset(spb.block_map, 0, 512);  // 初始化block_map
    spb.inode_map[0] = (0x80000000);  // 将inode_map的第1位设置为1
    spb.block_map[0] = ~0;  // 将block_map的前32位设置为1
    spb.block_map[1] = (0xc0000000);  // 将block_map的第33位和第34位设置为1
}

int write_sp_block() {
    char buf[DEVICE_BLOCK_SIZE*2];
    memcpy(buf, &spb, sizeof(super_block));
    if (disk_write_block(0, buf) < 0) {
        printf("Write to disk failed.\n");
        return 0;
    }
    if (disk_write_block(1, &buf[DEVICE_BLOCK_SIZE]) < 0) {
        printf("Write to disk failed.\n");
        return 0;
    }
    return 1;
}


int read_sp_block(){
    char buf[DEVICE_BLOCK_SIZE*2];
    if (disk_read_block(0, buf) < 0) {
        printf("Read from disk failed.\n");
        return 0;
    }
    if (disk_read_block(1, &buf[DEVICE_BLOCK_SIZE]) < 0) {
        printf("Read from disk failed.\n");
        return 0;
    }
    memcpy(&spb, buf, sizeof(super_block));
    return 1;
}

int alloc_block() {
    if (!spb.free_block_count) return -1;
    for (int i = 0; i < MAX_BLOCK_MAP; i++) {
        uint32_t block = spb.block_map[i];
        for (int j = 0; j < 32; j++) {
            if ((block >> (31-j)) & 1) continue;
            else {
                spb.free_block_count--;
                spb.block_map[i] |= 1 << (31-j);
                write_sp_block();
                return i*32+j;
            }
        }
    }
    return -1;
}


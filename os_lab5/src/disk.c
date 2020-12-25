#include "disk.h"

#include <stdio.h>

inline int get_disk_size()
{
        return 4*1024*1024; 
}

static FILE* disk;

static int create_disk()
{
        FILE* tmp = fopen("disk","w");
        for(int i = 0; i < get_disk_size(); i++){
                fputc(0,tmp);
        }
        fclose(tmp);
}

int open_disk()
{
        if(disk != 0){
                return -1;
        }
        disk = fopen("disk","r+");
        if(disk == 0){
                create_disk();
                disk = fopen("disk","r+");
                if(disk == 0){
                        return -1;
                }
        }
        return 0;
}

int disk_read_block(unsigned int block_num, char* buf)
{
        if(disk == 0){
                return -1;
        }
        if(block_num * DEVICE_BLOCK_SIZE >= get_disk_size()){
                return -1;
        }
        if(fseek(disk, block_num * DEVICE_BLOCK_SIZE, SEEK_SET)){
                return -1;
        }
        if(fread(buf, DEVICE_BLOCK_SIZE,1,disk) != 1){
                return -1;
        }
        return 0;
}

int disk_write_block(unsigned int block_num, char* buf)
{
        if(disk == 0){
                return -1;
        }
        if(block_num * DEVICE_BLOCK_SIZE >= get_disk_size()){
                return -1;
        }
        if(fseek(disk, block_num * DEVICE_BLOCK_SIZE, SEEK_SET)){
                return -1;
        }
        if(fwrite(buf,DEVICE_BLOCK_SIZE,1,disk) != 1){
                return -1;
        }
        return 0;
}

int close_disk()
{
        if(disk == 0){
                return -1;
        }
        int r = fclose(disk);
        disk = 0;
        return r;
}
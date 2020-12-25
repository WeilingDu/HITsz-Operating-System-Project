#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "file.h"
#include "superblock.h"
#include "inode.h"
#include "dir.h"

void print_file_system_info(){
    printf("----------------------------------------------------------------------------------------\n");
    printf("In this Symplified File System :\n");
    printf("Supports: \\ls, \\mkdir, \\touch, \\cp, \\shutdown \n");
    printf("It has %d folders and %d files in this system now;\n", spb.dir_inode_count, 1024-(spb.dir_inode_count)-(spb.free_inode_count));
    printf("----------------------------------------------------------------------------------------\n");
}

int init_system(){
    // 初始化超级块并写入磁盘
    init_sp_block();
    if(!write_sp_block()){
        printf("Write super block failed when init file system.");
        return 0;
    };
    
    // 初始化第一个inode
    if(!init_root_inode()){
        printf("Init root inode failed.");
        return 0;
    };
    // 初始化其他inode
    for (int i = 1; i < INODE_NUM; i++) {
        if (!init_inode(&inode_table[i], 0, _FILE_, 0)) {
            printf("Init inode failed.\n");
            return 0;
        }
    }
    // 将初始化后的inode table写入磁盘
    for (int i = 0; i < INODE_NUM; i++)
    {
        if (!write_inode(&inode_table[i], i)) {
            printf("Write Inode failed.\n");
            return 0;
        }
    }
    // 初始化根目录
    init_root_dir_item();
    if(!write_dir_item(33)){
        printf("Init root dir item failed.\n");
        return 0;
    };
    printf("Init file system sueccessfully!\n");
    printf("--------------- Welcome to Simplified File System! Please input instructions.------------\n");
    print_file_system_info();
}

int open_system()
{
    // 每次打开文件系统时，要先打开磁盘
    if (open_disk()<0) {
        printf("Open disk failed.\n");
        return 0;
    }
    // 打开磁盘后
    if(read_sp_block() && spb.magic_num == MAGIC_NUM)
    {// 如果幻数正确，说明该磁盘已经建立过文件系统
        // 将磁盘中的inode表读到内存的inode_table中
        for (int i = 0; i < INODE_NUM; i++)
        {
            if (!read_inode(&inode_table[i], i))
            {
                printf("Read inode failed.\n");
                return 0;
            }
        }
        printf("-------------- Welcome to Simplified File System! Please input instructions.-----------\n");
        print_file_system_info();
    }else
    {// 如果幻数不正确，说明该磁盘还没有建立过文件系统或者是文件系统损坏
        printf("No correct file system. We are building a new file system...\n");
        init_system();
    }
    return 1;
}

int close_system() {
    if (!write_sp_block(&spb)) {
        printf("Save super bolck failed.\n");
        return 0;
    }
    printf("Save superblock successfully.\n");

    for (int i = 0; i < INODE_NUM; i++)
    {
        if (!write_inode(&inode_table[i], i)) {
            printf("Write inode failed.\n");
            return 0;
        }
    }
    printf("Write inode successfully.\n");

    if (close_disk() < 0) {
        printf("Close disk failed.\n");
        return 0;
    }
    printf("Close disk successfully.\n");
    return 1;
}

int check_name(char *text)
{
    int i = 0;
    char a = text[0];
    while(a!='\0')
    {
        if((a>='a'&&a<='z') || (a>='A'&&a<='Z') || (a>='0' && a<='9') || a=='_' || a=='.')
        {
            a = text[++i];
        }
        else 
        {
            printf("Illegal name!! (Only numbers ,letters ,_ and . can be accepted)\n");
            return 1;
        }
    }
    return 0;
}

int check_duplicate_name(char *tmp, int i_id, int type)
{
    int i, j;
    int flag = 0;
    for(i=0; i<6; i++)
    {
        if (!inode_table[i_id].block_point[i])
        {// 如果文件夹没有对应数据块
            return 1;
        } else
        {
            // 通过block_point找到其指向的块，然后从磁盘里把块读进block_buffer
            read_dir_item(inode_table[i_id].block_point[i]); 
            // 遍历该block_buffer中的dir_item
            for(j=0; j<8; j++)
            {
                if(block_buffer[j].type == type)
                {
                    if(!strcmp(block_buffer[j].name, tmp) && block_buffer[j].valid)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int insert_dir_item(int i_id, int new_inode, char *tmp, int type)
{
    for (int i=0; i<6; i++)
    {
        if (!inode_table[i_id].block_point[i])
        {// 如果文件夹没有对应数据块
            int block_num = alloc_block();  // 分配一个数据块
            // printf("111block_num: %d\n", block_num);
            if (block_num < 0)
            {
                printf("No empty block!\n");
                return 0;
            }
            inode_table[i_id].block_point[i] = block_num;  // 数据组的block index
            
            // 将block index读到block_buffer中
            read_dir_item(inode_table[i_id].block_point[i]);
            // 初始化dir item
            for (int j = 0; j < 8; j++)
            {
                block_buffer[j].valid = 0;
            }
            block_buffer[0].valid = 1;
            block_buffer[0].type = type;
            block_buffer[0].inode_id = new_inode;
            memcpy(block_buffer[0].name, tmp, MAX_NAME_SIZE);
            write_dir_item(inode_table[i_id].block_point[i]);

            inode_table[i_id].size++;
            if(!write_inode(&inode_table[i_id], i_id)){
                printf("write inode failed!\n");
            };
            return 1;
        }
        // 如果文件夹有对应数据块
        // 将block index读到block_buffer中
        read_dir_item(inode_table[i_id].block_point[i]);
        // printf("block_num: %d\n", inode_table[i_id].block_point[i]);
        // 遍历该block_buffer中的dir_item
        for(int j=0; j<8; j++)
        {
            if(!block_buffer[j].valid)
            {
                // 如果该文件夹下已经有6个文件夹
                // 检查发现前6个都满了
                // if (j == 6)
                // { 
                //     printf("There are 6 files in this path!\n");
                //     return 0;
                // }
                // printf("block_buffer j: %d\n", j);
                // printf("block_buffer[j].valid: %d\n", block_buffer[j].valid);
                // printf("block_buffer[j].name: %s\n", block_buffer[j].name);
                block_buffer[j].valid = 1;
                block_buffer[j].type = type;
                block_buffer[j].inode_id = new_inode;
                memcpy(block_buffer[j].name, tmp, MAX_NAME_SIZE);
                write_dir_item(inode_table[i_id].block_point[i]);
                
                inode_table[i_id].size++;
                // printf("inode_table[i_id].size: %d\n", inode_table[i_id].size);
                write_inode(&inode_table[i_id], i_id);
                return 1;
            }
            
            
        }
    }
    // 该文件夹下的文件已满
    printf("The number of files and dirs in this dir has met the maxmuim\n");
    return 0;
}

int get_the_last_but_one_touch(char *path, char *tmp)
{   
    int i_id, i, j = 0;
    if(path[0] == '/')
    {// 根目录
        i = 1;  // path的下标
        i_id = 0;  // 根目录对应第0个inode
    } else 
    {// 当前目录（即根目录）
        i = 1;
        i_id = 0;  // 根目录对应第0个inode
    }

    while(1)
    {
        if(path[i] != '/')
        {// tmp保存路径上的文件夹名字
            tmp[j++] = path[i];
        }
        if(path[i] == '\0')
        {// 到达路径名结尾
            break;
        }
        if(path[i] == '/')
        {
            tmp[j] = '\0';
            j = 0;  // tmp的下标
            int success = 0;
            for(int k=0; k<6; k++)
            {
                // 通过block_point找到其指向的块，然后从磁盘里把块读进block_buffer
                read_dir_item(inode_table[i_id].block_point[k]);

                // 遍历该block_buffer中的dir_item
                for(int l=0; l<8; l++)
                {
                    // printf("l: %d\n", l);
                    // printf("block_buffer[l].type: %d\n", block_buffer[l].type);
                    // printf("block_buffer[l].name: %s\n", block_buffer[l].name);
                    // printf("block_buffer[l].valid: %d\n", block_buffer[l].valid);
                    if(block_buffer[l].type && !strcmp(tmp, block_buffer[l].name) && block_buffer[l].valid)
                    {// 如果dir_item的类型是目录且名字符合，说明找到
                        i_id = block_buffer[l].inode_id;
                        success = 1;
                        break;  // 跳出block_buffer的遍历
                    }
                }
                if (success)
                {
                    break;  // 跳出block_buffer的遍历
                }
            }
            if(!success)
            {
                printf("There is no directory \"%s\"\n", tmp);
                return 0;
            }
        }
        i++;
    }
    return i_id;
}

int get_the_last_but_one_mkdir(char *path, char *tmp)
{
    int i_id, i, j = 0;
    if(path[0] == '/')
    {// 根目录
        i = 1;  // path的下标
        i_id = 0;  // 根目录对应第0个inode
    } else
    {// 当前目录（即根目录）
        i = 1;
        i_id = 0;  // 根目录对应第0个inode
    }

    while(1)
    {
        if(path[i] != '/')
        {// tmp保存路径上的文件夹名字
            tmp[j++] = path[i];
        }
        if(path[i] == '\0')
        {// 到达路径名结尾
            break;
        }
        if(path[i] == '/' && path[i+1] != '\0')
        {// tmp已得到文件夹名，且未到达结尾（即不是文件名）
            tmp[j] = '\0';
            j = 0;  // tmp的下标
            int success = 0;
            for(int k=0; k<6; k++)
            {
                // 通过block_point找到其指向的块，然后从磁盘里把块读进block_buffer
                read_dir_item(inode_table[i_id].block_point[k]);

                // 遍历该block_buffer中的dir_item
                for(int l=0; l<8; l++)
                {
                    if((block_buffer[l].type == _FOLDER_) && !strcmp(tmp, block_buffer[l].name) && block_buffer[l].valid)
                    {// 如果dir_item的类型是目录且名字符合，说明找到
                        i_id = block_buffer[l].inode_id;
                        success = 1;
                        break;  // 跳出block_buffer的遍历
                    }
                }
                if (success)
                {
                    break;  // 跳出block_buffer的遍历
                }
            }
            if(!success)
            {
                printf("Don't have directory \"%s\" !\n", tmp);
                return 0;
            }
        }
        if(path[i]=='/' && path[i+1]=='\0')
        {
            tmp[j] = '\0';
            break;
        }
        i++;
    }
    return i_id;
}

int get_the_last_dir(char *path, char *tmp)
{
    int i_id, i, j = 0;
    if(path[0] == '/')
    {// 根目录
        i = 1;  // path的下标
        i_id = 0;  // 根目录对应第0个inode
    } else
    {// 当前目录（即根目录）
        i = 1;
        i_id = 0;  // 根目录对应第0个inode
    }
    if (path[i] == '\0')
    {
        memcpy(tmp, path, 2);
    }
    
    while(path[i] != '\0')
    {
        if(path[i] != '/')
        {
            tmp[j++] = path[i];
        }
        if(path[i] == '/' || path[i+1] == '\0')
        {
            tmp[j] = '\0';
            // printf("tmp: %s\n", tmp);
            j = 0;  // tmp的下标
            int success = 0;
            for(int k = 0; k < 6; k++)
            {
                // 通过block_point找到其指向的块，然后从磁盘里把块读进block_buffer
                read_dir_item(inode_table[i_id].block_point[k]);

                // 遍历该block_buffer中的dir_item
                for(int l = 0; l < 8; l++)
                {
                    // printf("l: %d\n", l);
                    // printf("block_buffer[l].type: %d\n", block_buffer[l].type);
                    // printf("block_buffer[l].name: %s\n", block_buffer[l].name);
                    // printf("block_buffer[l].valid: %d\n", block_buffer[l].valid);
                    if(block_buffer[l].type && !strcmp(tmp, block_buffer[l].name) && block_buffer[l].valid)
                    {// 如果dir_item的类型是目录且名字符合，说明找到
                        i_id = block_buffer[l].inode_id;
                        success = 1;
                        break;  // 跳出block_buffer的遍历
                    }
                }
                if (success)
                {
                    break;  // 跳出block_buffer的遍历
                }
            }
            if(!success)
            {
                printf("There is no directory \"%s\" !\n", tmp);
                return -1;
            }
        }
        i++;
    }
    return i_id;
}

int get_the_last_file(char *path, char *tmp)
{
    int ori_dir_id = get_the_last_but_one_touch(path, tmp);
    // 路径存在各种错误
    if(ori_dir_id < 0)
    {
        return -1;
    }
    if(tmp[0] == '\0')
    {
        printf("Wrong path!\n");
        return -1;
    }
    // 遍历该文件夹的6个数据块
    for (int q = 0; q < 6; q++)
    {
        int block_num = inode_table[ori_dir_id].block_point[q];
        // printf("i_id: %d\n", ori_dir_id);
        // printf("block_num: %d\n", block_num);
        if (block_num)
        {// 如果该文件夹有被分配数据块
            read_dir_item(block_num);  // 读取该数据块到数据块缓存区block buffer中
            // 遍历数据块的8个目录项
            for (int p = 0; p < 8; p++)
            {
                // printf("block_buffer[p].type: %d\n", block_buffer[p].type);
                // printf("block_buffer[p].valid: %d\n", block_buffer[p].valid);
                if (block_buffer[p].type == _FILE_ && block_buffer[p].valid)
                {// 如果该目录项为文件且有效
                    return block_buffer[p].inode_id;
                }
            }
        }
    }
    return -1;
}

void touch(char *path)
{
    char tmp[MAX_NAME_SIZE];
    // id_id路径中最后一个文件夹的inode_id, tmp为要创建文件的名称
    int i_id = get_the_last_but_one_touch(path, tmp);  
    // printf("i_id: %d\n", i_id);
    // printf("tmp: %s\n", tmp);
    // 路径存在各种错误
    if(i_id < 0)
    {
        return;
    }
    int i, j;
    if(tmp[0] == '\0')
    {
        printf("Wrong path!\n");
        return;
    }
    if(check_name(tmp))
    {
        return;
    }
    // 路径没有错误  
    if (!check_duplicate_name(tmp, i_id, _FILE_))
    {// 如果有同名文件
        printf("There is already a file named \"%s\" in this path!\n", tmp);
        return;
    }
    int new_inode = alloc_inode();
    // printf("new_inode: %d\n", new_inode);
    if (new_inode < 0)
    {
        printf("No empty inode!\n");
        return;
    }
    if (!insert_dir_item(i_id, new_inode, tmp, _FILE_))
    {
        printf("Touch file failed. \n");
        return;
    }
    init_inode(&inode_table[new_inode], 0 , _FILE_, 1);
    write_inode(&inode_table[new_inode], new_inode);
    print_file_system_info();
    return;
}

void mkdir(char *path)
{
    char tmp[MAX_NAME_SIZE];
    int i, j;
    // printf("path: %s\n", path);
    // 获得了路径中倒数第二个文件夹的inode_id 
    int i_id = get_the_last_but_one_mkdir(path, tmp);  
    // printf("i_id: %d\n", i_id);
    // printf("tmp: %s\n", tmp);
    // 路径存在各种错误
    if(i_id < 0)
    {
        return;
    }
    if(tmp[0] == '\0')
    {
        printf("Wrong path!\n");
        return;
    }
    // 文件/文件夹命名错误
    if(check_name(tmp))
    {
        return;
    }
    // 检查是否有同名文件
    if (!check_duplicate_name(tmp, i_id, _FOLDER_))
    {// 如果有同名文件
        printf("There is already a folder named \"%s\" in this path !\n", tmp);
        return;
    }
    int new_inode = alloc_inode();
    // printf("new_inode: %d\n", new_inode);
    if (new_inode < 0)
    {
        printf("No empty inode!\n");
        return;
    }
    // 以下开始和touch有一些不一样
    if (!insert_dir_item(i_id, new_inode, tmp, _FOLDER_))
    {
        printf("Mkdir failed. \n");
        return;
    }
    init_inode(&inode_table[new_inode], 0 , _FOLDER_, 1);
    write_inode(&inode_table[new_inode], new_inode);
    // 注意：超级块的目录inode数量要更新
    spb.dir_inode_count++;
    write_sp_block();
    print_file_system_info();
}

void ls(char *path)
{
    // printf("path: %s\n", path);
    char tmp[MAX_NAME_SIZE];
    int i_id;
    // i_id为路径中最后一个文件夹的inode id，tmp为该文件夹的名称
    i_id = get_the_last_dir(path, tmp);
    // printf("i_id: %d\n", i_id);
    // printf("tmp: %s\n", tmp);
    // 遍历该文件夹的6个数据块
    for (int q = 0; q < 6; q++)
    {
        int block_num = inode_table[i_id].block_point[q];
        // printf("i_id: %d\n", i_id);
        // printf("block_num: %d\n", block_num);
        if (block_num)
        {// 如果该文件夹有被分配数据块
            read_dir_item(block_num);  // 读取该数据块到数据块缓存区block buffer中
            // 遍历数据块的8个目录项
            for (int p = 0; p < 8; p++)
            {
                // printf("block_buffer[p].type: %d\n", block_buffer[p].type);
                // printf("block_buffer[p].valid: %d\n", block_buffer[p].valid);
                if (block_buffer[p].type == _FOLDER_ && block_buffer[p].valid)
                {// 如果该目录项为文件夹且有效
                    printf("*");
                    printf("%s\n", block_buffer[p].name);
                }
                if (block_buffer[p].type == _FILE_ && block_buffer[p].valid)
                {// 如果该目录项为文件且有效
                    printf("%s\n", block_buffer[p].name);
                }
            }
        }
        
    }
    return;
}

void cp(char *ori, char *dest)
{
    // 对源文件路径ori的处理
    char ori_name[MAX_NAME_SIZE];
    // ori_id为源文件对应的inode，ori_name为源文件的名字
    int ori_id = get_the_last_file(ori, ori_name);
    // 路径存在各种错误
    if(ori_id < 0)
    {
        printf("Wrong ori file path!\n");
        return;
    }
    if(ori_name[0] == '\0')
    {
        printf("Wrong ori file path!\n");
        return;
    }
    int size = inode_table[ori_id].size;
    int link = inode_table[ori_id].link;

    // 对目标文件路径dest的处理
    char dest_name[MAX_NAME_SIZE];
    memcpy(dest_name, ori_name, MAX_NAME_SIZE);
    char tmp_folder[MAX_NAME_SIZE];
    // tmp_id为目标文件所在文件夹的id，tmp_folder为该文件夹的名字
    int tmp_id = get_the_last_dir(dest, tmp_folder);
    // printf("tmp_id: %d", tmp_id); 
    // printf("tmp_folder: %s", tmp_folder); 
    // 路径存在各种错误
    if(tmp_id < 0)
    {
        return;
    }
    if(tmp_folder[0] == '\0')
    {
        printf("Wrong dest path!\n");
        return;
    }
    // 路径没有错误，检查是否有同名文件
    if (!check_duplicate_name(dest_name, tmp_id, _FILE_))
    {// 如果有同名文件
        printf("There is already a file named \"%s\" in this path !\n", dest_name);
        return;
    }
    int new_inode = alloc_inode();
    // printf("new_inode: %d\n", new_inode);
    if (new_inode < 0)
    {
        printf("No empty inode!\n");
        return;
    }
    // printf("tmp_id: %d", tmp_id); 
    // printf("new_inode: %d", new_inode); 
    // printf("dest_name: %s", dest_name); 
    if (!insert_dir_item(tmp_id, new_inode, dest_name, _FILE_))
    {
        printf("Touch file failed. \n");
        return;
    }
    init_inode(&inode_table[new_inode], size , _FILE_, link);  // 注意这里的size和link与源文件相同
    write_inode(&inode_table[new_inode], new_inode);
    print_file_system_info();
    return;
}

void shutdown()
{
    if (!close_system())
    {
        printf("Shut down failed!\n");
        return;
    }
    printf("Shut down the Symplified File System sucessfully!\n");
    printf("----------------------------------- Thanks for your using :) ---------------------------\n");
    return;
}
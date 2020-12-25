#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "dir.h"

#define DEVICE_SIZE (1<<22)  // 磁盘大小为4MB
#define BLOCK_SIZE 1024  // 数据块大小1KB
#define DATA_START_DEVICE_BLOCK 66  // 开始存放数据的磁盘块

/**
 * @brief 初始化文件系统
 * 
 * @return int 
 */
int init_system();

/**
 * @brief 打开系统
 * 
 * @return int 
 */
int open_system();

/**
 * @brief 关闭该文件系统。要将超级块和inode表写入磁盘。
 * 
 * @return int 
 */
int close_system();

/**
 * @brief 检查文件/文件夹是否有非法字符（只允许数字、字母、下划线_和小数点.）
 * 
 * @param text 
 * @return int 有非法字符返回1，没有错返回0
 */
int check_name(char *text);

/**
 * @brief 检查i_id所对应的文件夹中是否存在同名文件
 * 
 * 
 * @param tmp 文件名/文件夹名
 * @param i_id 将要遍历的文件夹所对应的inode的下标
 * @param type 文件/文件夹
 * @return int 没有同名时返回1，有返回0
 */
int check_duplicate_name(char *tmp, int i_id, int type);

/**
 * @brief 在路径中的最后一个文件夹中创建一个dir item
 * 
 * @param i_id 路径中的最后一个文件夹的inode id
 * @param new_inode 新建文件/文件夹的inode id
 * @param tmp 文件/文件夹的名字
 * @param type 文件/文件夹
 * @return int 成功返回1，否则返回0
 */
int insert_dir_item(int i_id, int new_inode, char *tmp, int type);


/**
 * @brief 返回最后一个文件夹的inode的下标，保存要创建文件的名称在tmp中
 * 
 * @param path 路径
 * @param tmp 文件名
 * @return int 成功则返回最后一个文件夹的inode的下标
 */
int get_the_last_but_one_touch(char *path, char *tmp);

/**
 * @brief 返回倒数第二个文件夹的inode的下标，保存要创建的文件夹的名称在tmp中
 * 
 * @param path 路径
 * @param tmp 文件夹名
 * @return int 成功则返回倒数第二个文件夹的inode的下标
 */
int get_the_last_but_one_mkdir(char *path, char *tmp);

/**
 * @brief 返回最后一个文件夹的下标，其名称保存在tmp中
 * 
 * @param path 
 * @param tmp 
 * @return int 
 */
int get_the_last_dir(char *path, char *tmp);


/**
 * @brief 返回最后一个文件的下标，其名称保存在tmp中
 * 
 * @param path 
 * @param tmp 
 * @return int 
 */
int get_the_last_file(char *path, char *tmp);
/**
 * @brief 创建文件
 * 
 * @param path 路径
 */
void touch(char *path);

/**
 * @brief 创建文件夹
 * 
 * @param path 路径
 */
void mkdir(char *path);

/**
 * @brief 列举出该文件夹下的文件/文件夹
 * 
 * @param path 
 */
void ls(char *path);

/**
 * @brief 
 * 
 * @param ori 
 * @param dest 
 */
void cp(char *ori, char *dest);

/**
 * @brief 关闭该文件系统
 * 
 */
void shutdown();
#endif 
#ifndef SHELL_H
#define SHELL_H

#define MAX_ARG 128


char* argv[MAX_ARG];  // 参数
int argc;  // 参数数量
char buf[MAX_ARG];
int flag_close;

/**
 * @brief 通过空格分割命令行，得到指令关键字和参数
 * 
 */
void get_command();

/**
 * @brief 执行命令
 * 
 */
void run_command();


#endif
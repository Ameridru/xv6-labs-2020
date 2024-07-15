#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int i;
    int j = 0; // 用于记录lineSplit数组中已填充的字符串数量 
    int k;  // 用于存储read函数的返回值
    int l,m = 0; // l用于循环遍历读取的块，m用于记录buf中已填充的字符数量
    char block[32]; // 用于存储从stdin读取的块
    char buf[32]; // 临时缓冲区，用于存储当前正在处理的行或单词
    char *p = buf; 
    char *lineSplit[32]; // 字符串指针数组，用于存储分割后的单词或行  
    // 初始化lineSplit数组，将xargs本身的命令行参数（除了程序名外）放入数组 
    for(i = 1; i < argc; i++){
        lineSplit[j++] = argv[i]; // 将命令行参数的字符串指针加入到linesplit
    }
    // 从stdin标准输入中读取参数到block，直到EOF  
    while( (k = read(0, block, sizeof(block))) > 0){ 
        // 遍历读取的块 
        for(l = 0; l < k; l++){
            // 如果遇到换行符，说明该行的所有参数获取结束，需要调用一次argv[1]处理参数
            if(block[l] == '\n'){
                buf[m] = 0; // 在buf的当前位置放置字符串结束符
                m = 0; // 重置m，为处理下一行做准备
                lineSplit[j++] = p; // 将当前处理的字符串加入lineSplit数组
                p = buf;// 重置p，指向buf的开始，为处理下一行做准备
                lineSplit[j] = 0; // 标记lineSplit数组的末尾
                j = argc - 1; // 重置j，为下一次执行准备
                // 创建一个新进程来执行argv[1]指定的程序，将分割后的单词作为参数
                if(fork() == 0){
                    exec(argv[1], lineSplit); // 在子进程中执行argv[1]指向的程序  
                }                
                wait(0); // 等待子进程结束
            }
            // 如果遇到空格，说明获取了一个完整参数
            else if(block[l] == ' ') {  
                buf[m++] = 0; // 在buf的当前位置放置字符串结束符
                lineSplit[j++] = p; // 将当前处理的字符串指针加入lineSplit数组
                p = &buf[m]; // 更新p，指向buf中下一个字符的位置
            }
            else // 未获取到完整的参数 
            {
                buf[m++] = block[l]; // 将当前字符添加到buf中 
            }
        }
    }
    exit(0);
}
#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char* argv[])
{
    if(argc != 2) // 必须要有两个参数提供给命令行，第一个为程序名称（自动作为第一个参数），第二个为sleep_num
    {
        fprintf(2,"must 1 argument for %s \n", argv[0]);// 2表示标准错误
        exit(1);
    }
    printf("sleep for %s times", argv[1]);
    sleep(atoi(argv[1])); // 调用sleep
    exit(0); // 正常退出
}
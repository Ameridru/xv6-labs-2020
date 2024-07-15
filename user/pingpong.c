#include "kernel/types.h"
#include "user/user.h"
int main()
/*
    用两个管道分别进行父子进程的读写，对于不用的描述符，需要尽早关闭，防止因为管道中没有内容而阻塞
*/
{
    int p1[2]; // 父进程写，子进程读的管道
    if(pipe(p1) != 0) exit(1);
    int p2[2]; // 父进程读，子进程写的管道
    if(pipe(p2) != 0) exit(1);
    
    char buf = 'x'; // 待传输的字符

    if(fork()==0) // 子进程执行的内容
    {
        close(p1[1]); // 关闭p1的写端
        close(p2[0]); // 关闭p2的读端
        
        //子进程从pipe1的读端，读取字符数组
		if(read(p1[0], &buf, sizeof(buf)) != sizeof(buf)){
			printf("a--->b read error!");
			exit(1);
		}
        //打印读取到的字符数组
		printf("%d: received ping\n", getpid());
        //子进程向pipe2的写端，写入字符数组
		if(write(p2[1], &buf, sizeof(buf)) != sizeof(buf)){
			printf("a<---b write error!");
			exit(1);
		}
        exit(0);
    }  
    else // 父进程执行的内容
    {
        close(p1[0]); // 关闭p1的读端
        close(p2[1]); // 关闭p2的写端
        //父进程向pipe1的写端，写入字符数组
		if(write(p1[1], &buf, sizeof(buf)) != sizeof(buf)){
			printf("b--->a write error!");
			exit(1);
		}
        
        //父进程向pipe2的读端，读取字符数组
		if(read(p2[0], &buf, sizeof(buf)) != sizeof(buf)){
			printf("b<---a write error!");
			exit(1);
		}
        printf("%d: received pong\n", getpid());
        //等待进程子退出
        wait(0);
        exit(0);   
    }   
}

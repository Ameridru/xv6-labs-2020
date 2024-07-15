#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);

/**

 * @brief 读取左邻居的第一个数据
 * @param lpipe 左邻居的管道符
 * @param pfirst 用于存储第一个数据的地址
 * @return 如果没有数据返回-1,有数据返回0
   */
   int lpipe_first_data(int lpipe[2], int *dst)
   {
     if (read(lpipe[RD], dst, sizeof(int)) == sizeof(int)) { // 如果从左邻居成功读取到一个正数
       printf("prime %d\n", *dst); // 打印该整数
       return 0;
     }
     return -1;
   }

/**

 * @brief 读取左邻居的数据，将不能被first整除的数写入右邻居
 * @param lpipe 左邻居的管道符
 * @param rpipe 右邻居的管道符
 * @param first 左邻居的第一个数据
   */
   void transmit_data(int lpipe[2], int rpipe[2], int first)
   {
     int data;
     // 从左管道读取数据
     while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int)) 
     { // 将无法被第一个数整除的数据传递入右管道
       if (data % first)  write(rpipe[WR], &data, sizeof(int));
     }
         close(lpipe[RD]);
         close(rpipe[WR]);
   }

/**

 * @brief 寻找素数

 * @param lpipe 左邻居管道
   */
   void primes(int lpipe[2])
   {
     close(lpipe[WR]); // 关闭左管道的写端，只使用读端
     int first;
     if (lpipe_first_data(lpipe, &first) == 0) { // 读左管道的第一个数到first中
       int p[2];
       pipe(p); // 当前的管道
       transmit_data(lpipe, p, first); // 继续读左邻居数据，将不能被first整除的数传入右管道

       if (fork() == 0) { // 如果是子进程，就继续递归调用，并将当前进程的右管道作为子进程左管道
         primes(p);    	  // 递归的思想，但这将在一个新的进程中调用
       } else {
         close(p[RD]); // 父进程的任务完成，关闭左管道的读端
         wait(0); // 等待子进程退出
       }
     }
     exit(0);
   }

int main(int argc, char const *argv[])
{
  int p[2];
  pipe(p);

  for (int i = 2; i <= 35; ++i) //写入初始数据（2-35之间的数据）
    write(p[WR], &i, INT_LEN);

  if (fork() == 0) { // 创建一个子进程，在第一个子进程中，以p[2]作为左管道
    primes(p);
  } else {
    close(p[WR]);
    close(p[RD]);
    wait(0);
  }

  exit(0);
}
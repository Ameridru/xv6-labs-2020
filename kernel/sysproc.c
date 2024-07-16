#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// kernel/sysproc.c
uint64
sys_trace(void)
{
  // 获取系统调用的参数
  argint(0, &(myproc()->trace_mask)); // 获取参数列表中的第一个参数，解析为整数后，保存到trace_mask中
  return 0;
}

// kernel/sysproc.c
uint64
sys_sysinfo(void){
  struct sysinfo info;
  uint64 addr; // 保存地址
  struct proc* p = myproc();// 指向当前进程的PCB
  if(argaddr(0, &addr) < 0) return -1; // 从用户空间获取第一个系统调用参数,这里是用户空间虚拟地址
  // 获取空闲内存和正在使用的进程数
  info.freemem = freemem_size();
  info.nproc = proc_num();
  // 将info结构体的内容从内核空间拷贝到用户空间提供的地址addr处
  if(copyout(p->pagetable, addr ,(char *)&info, sizeof(info)) < 0)
    return -1;
  return 0;
}
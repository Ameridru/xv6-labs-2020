#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

/*
	format_name：将路径格式化为文件名
*/
char* fmt_name(char *path){
  static char buf[DIRSIZ+1]; // 静态变量buf，DIRSIZ是xv6系统中一个宏定义，表示指定目录项（directory entry）中文件名（name）字段的最大长度                                                   
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--); // 从后往前搜索，找到第一个斜杠'/'
  p++; // 斜杠后的第一个字符
  memmove(buf, p, strlen(p)+1); // 将p之后的内容复制到buf中
  return buf;
}
/*
	系统文件名与要查找的文件名，若一致，打印系统文件完整路径
*/
void eq_print(char *fileName, char *findName){
	if(strcmp(fmt_name(fileName), findName) == 0){ // 如果文件名和查找的文件名一致
		printf("%s\n", fileName); // 打印完整路径
	}
}
/*
	在某路径中查找某文件
*/
void find(char *path, char *findName){
	int fd;
	struct stat st;	
    // 尝试已只读方式打开path
	if((fd = open(path, O_RDONLY)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
    // 获取文件状态 
	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd); // 打开失败时关闭文件描述符
		return;
	}
	char buf[512], *p;	// 用于构建路径的缓冲区
	struct dirent de;  
    #define DIRSIZ 14  
	//struct dirent {  
    //    ushort inum;  // inode number  
    //    char name[DIRSIZ];  // name of file  
	//};
	switch(st.type){	
		case T_FILE: // 普通文件
			eq_print(path, findName); // 判断是不是目标文件			
			break;
		case T_DIR: // 目录
			if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
				printf("find: path too long\n");
				break;
			}
			strcpy(buf, path); // 将path加入buf
			p = buf+strlen(buf); 
			*p++ = '/'; // 在path后添加slash
            // 遍历目录项中的dirent结构体
			while(read(fd, &de, sizeof(de)) == sizeof(de)){
				//printf("de.name:%s, de.inum:%d\n", de.name, de.inum);
                // 忽略特殊项
				if(de.inum == 0 || de.inum == 1 || strcmp(de.name, ".")==0 || strcmp(de.name, "..")==0)
					continue;	
                // 构建完整路径并递归调用find
				memmove(p, de.name, strlen(de.name));
				p[strlen(de.name)] = 0;
				find(buf, findName); 
			}
			break;
	}
	close(fd);	
}

int main(int argc, char *argv[]){
	if(argc < 3){
		printf("find: find <path> <fileName>\n");
		exit(0);
	}
	find(argv[1], argv[2]); 
	exit(0);
}

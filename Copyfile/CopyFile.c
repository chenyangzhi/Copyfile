//mycp程序的通用函数
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//对系统函数进行包装，以便对可能的错误进行处理

void error_message(const char* error_message)  //错误消息输出函数
{
	perror(error_message);
	exit(EXIT_FAILURE);
}

int open_file(const char *pathname, int flags, mode_t mode)   //包装了open函数，创建失败能报错
{
	int filehand_dst;
	if((filehand_dst = open(pathname,flags, mode)) < 0)
	{
		error_message("open dest file error");
	}
	return filehand_dst;
}

int read_file(int filehand_src,char* buf,int buff_size)
{
	int n;
	if((n = read(filehand_src,buf, buff_size)) < 0)
		error_message("read erro");
	return n;
}
void write_file(int filehand_dst,char* buf,int n)
{
	if (write(filehand_dst,buf,n) != n)
		error_message("write erro");
}

void symbol_link(char* old_path, char* new_path)
{
	if(symlink(old_path,new_path) != 0)
		error_message("symlink erro");
}

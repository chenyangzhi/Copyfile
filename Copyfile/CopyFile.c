//mycp程序的通用函数
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include "CopyFile.h"
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
		error_message("symlink error");
}

bool is_parent_dir(char* input_path,char* output_path)
{
	if(strlen(input_path) > strlen(output_path))
			return false;	
	do{		
		if(*input_path == *output_path)
			input_path++,output_path++;
		else
			return false;
	}while(*input_path);
	if(*output_path == '/')
		return true;
	return false;
}
bool str_cmp(char* str1,char* str2,file flag)
{
	if(flag == ENUM_DIR)
	{
		while(str1 != NULL && str2 != NULL)
		{
			if(*str1 == *str2)
			{
				str1++;
				str2++;
			}else
				return false;
		}
		if(*str2 == '/' && (++str2) == NULL)
			return true;
		return false;
	}else{
		return  !strcmp(str1,str2);
	}
}
char* absolute_path(char* relapath,char* real_name)
{
	char* base_name,*dir_name;
	sprintf(real_name,"%s",relapath);
	base_name = basename(relapath);
	dir_name = dirname(real_name);
	if(realpath(dir_name,real_name) == NULL)
		printf("no this document:%s",dir_name);
	real_name = strcat(real_name,"/");
	return strcat(real_name,base_name);
	
}
void link_file(char* path1, char* path2)
{
	if(0 != link(path1,path2))
	{
		error_message("link error");
	}
	return;
}

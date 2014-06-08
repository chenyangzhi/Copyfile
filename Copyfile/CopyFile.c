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
		error_message(pathname);
	}
	return filehand_dst;
}

int read_file(int filehand_src,char* buf,int buff_size, const char* pathname)
{
	int n;
	if((n = read(filehand_src,buf, buff_size)) < 0)
	{
		fprintf(stderr,"%s ",pathname);
		error_message("read error");
	}
	return n;
}
void write_file(int filehand_dst,char* buf,int n, const char* pathname)
{
	if (write(filehand_dst,buf,n) != n)
		{
			fprintf(stderr,"%s ",pathname);
			error_message("write error");		
		}
}

void symbol_link(const char* old_path, const char* new_path)
{
	if(symlink(old_path,new_path) != 0)
	{
		fprintf(stderr,"%s ",old_path);
		error_message("symlink error");
	}
}

bool is_parent_dir(const char* input_path,const char* output_path)
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
bool str_cmp(const char* str1,const char* str2,const file flag)
{
	int l;
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
		if( strcmp(str1,str2) == 0 )
		{
			return  true;
		}
		return false;
	}
}
char* absolute_path(const char* relapath,char real_name[])
{
	char base_name[MAX_PATH_LENGTH],dir_name[MAX_PATH_LENGTH],
		*file_base_name,*parent_name;
	sprintf(real_name,"%s",relapath);
	sprintf(base_name,"%s",relapath);
	sprintf(dir_name,"%s",relapath);
	file_base_name = basename(base_name);
	
	parent_name = dirname(dir_name);
	if(realpath(parent_name,real_name) == NULL)
		printf("no this document:%s",dir_name);
	if(strcmp(real_name,"/") != 0)
		real_name = strcat(real_name,"/");
	return strcat(real_name,file_base_name);
	
}
void link_file(const char* path1,const char* path2)
{
	if(0 != link(path1,path2))
	{
		fprintf(stderr,"%s ",path1);
		error_message("link error");
	}
	return;
}
void file_status(const char* path,struct stat* st)
{
	if(0 != lstat(path,st))
	{
		fprintf(stderr,"path is %s ",path);
		error_message("stat error");
	}
}

bool access_file(const char* path, int mode)
{
	if(0 != access(path,mode))
	{
		fprintf(stderr,"path is %s ",path);
		error_message("access error");	
	}
	return true;
}

void rename_file(const char *oldpath, const char *newpath)
{
	if(0 != rename(oldpath,newpath))
	{
		fprintf(stderr,"path is %s ",oldpath);
		error_message("rename error");
	}
}


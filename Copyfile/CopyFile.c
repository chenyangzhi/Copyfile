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
bool str_cmp(const char* str1,const char* str2,const file_type flag)
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

bool strcmp_backup(const char* name1,char* name2,int *d)
{
	int i,k,j,l1,l2;
	char t,*tempname2,tempname1[MAX_PATH_LENGTH],numsuffix[MAX_PATH_LENGTH];
	
	k = 0;
	l1 =  strlen(name1);
	tempname2 = basename(name2);
	l2 = strlen(tempname2);
	if(l1 <= l2+3)
		return false;
	strcpy(tempname1,name1);
	if(name1[l1-1] != '~')
		return false;
	if(name1[l1-2] < '0' && name1[l1-2] > '9')
	{
		return false;
	}else{
		numsuffix[k] = name1[l1-2];
	}
	for(k = 1,i = l1-3; i >= 0; i--,k++)
	{
		if(name1[i] > '0' && name1[i] < '9')
		{
			numsuffix[k] = name1[i];
			continue;
		}else if(name1[i] == '~' && name1[i-1] == '.'){
			tempname1[i-1] = 0;
			if( strcmp(tempname1,tempname2) == 0)
			{
				numsuffix[k] = 0;
				for(i=0,j=k-1;i<j;i++,j--) { t=numsuffix[i]; numsuffix[i]=numsuffix[j]; numsuffix[j]=t; }
				*d = atoi(numsuffix);
				return true;
			}
		}			
	}
	return false;
}

void make_parent_dir(char* path1,char* path2,mode_t mode)
{
	int   i,len1,len2;
	char  seps[]   = "/";
	char  *token_cur,*token_next;

	if(path2 == NULL)
	{
		error_message("the parent path isn't exist");
	}
	token_cur = strtok(path2,seps);
	while( (token_next = strtok(NULL,seps)) != NULL )
	{
		strcat(path1,token_cur);
		strcat(path1,"/");
		if(access(path1,F_OK) != 0)
		{
			if(mkdir(path1,mode) == -1)
			{
				perror("mkdir error");
				exit(0);
			}
		}
		token_cur = token_next;
	}
	
}

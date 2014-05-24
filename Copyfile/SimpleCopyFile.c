//此文件执行相关的拷贝动作
#include "CopyFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utime.h>

#define BUFFSIZE  131072
extern globalArgs ga;
int simple_copyfile(char* input_file_path,char* output_file_path)   //执行相关拷贝动作
{
	int n;
	int filehand_src,filehand_dst;
	char buf[BUFFSIZE];
	struct stat info;
	stat(input_file_path, &info);  

	filehand_src = open_file(input_file_path,O_RDONLY);
	if(ga.need_preserve == true)
		filehand_dst = preserve_method(info,output_file_path,input_file_path);
	else
		filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775);
	
        while((n = read_file(filehand_src,buf,BUFFSIZE)) > 0)
		write_file(filehand_dst,buf,n);
	close(filehand_src);
	close(filehand_dst);		
        return true;
}


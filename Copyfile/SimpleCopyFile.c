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
extern file it;
int simple_copyfile(const char* input_file_path,const char* output_file_path)   //执行相关拷贝动作
{
	int n;
	int filehand_src,filehand_dst;
	char buf[BUFFSIZE];
	char lf[MAX_PATH_LENGTH];
	struct stat info;
	int x;
	x=1;
	file_status(input_file_path, &info);  
	if(ga.need_preserve == true)
	{
		if((filehand_dst = preserve_method(info,input_file_path,output_file_path)) == SUCCESS_LINK)
			return true;
	}else if(ga.need_no_deference == true){
		if(ENUM_SYMLINK == it)
		{
			input_file_path = realpath(input_file_path,lf);
			link_file(input_file_path, output_file_path);
			return true;
		}	
			
	}else if(ENUM_SYMLINK == it)
	{
		input_file_path = realpath(input_file_path,lf);
		filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775);
	}else{
		filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775);
	}
	filehand_src = open_file(input_file_path,O_RDONLY);
        while((n = read_file(filehand_src,buf,BUFFSIZE)) > 0)
		write_file(filehand_dst,buf,n);
	close(filehand_src);
	close(filehand_dst);		
        return true;
}


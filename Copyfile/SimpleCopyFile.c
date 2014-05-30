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
int overwrite = O_EXCL;

static void excute_copy(int src, int dst)
{
	int n;
	char buf[BUFFSIZE];
	while((n = read_file(src,buf,BUFFSIZE)) > 0)
		write_file(dst,buf,n);
	close(src);
	close(dst);
}

int argu_parse_copy(const char* input_file_path,const char* output_file_path)   //执行相关拷贝动作，
{
	
	int filehand_src,filehand_dst;
	char* input;	
	char lf[MAX_PATH_LENGTH];
	struct stat info;
	
	file_status(input_file_path, &info); 					//提取文件的状态信息
	if(ga.need_interactive)        
	{									//判断交互标志
		if(interactivity_method(output_file_path) == NOT_OVERWRITE)     //
		{
			return  true;
		}
	}
	if( true == ga.need_symbolic_link )
	{
		symbol_link(ga.input_file, output_file_path);
		return true;
	}
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
		input = realpath(input_file_path,lf);
		if(input != NULL)                                        //此软链接已经断掉了
		{		
			if(type_of_file(input) == ENUM_DIR)
			{
				symbol_link(input,output_file_path);
				return true;
			}
			filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT|overwrite, 0775);
		}else{
			readlink(input_file_path,lf,MAX_PATH_LENGTH);
			symbol_link(lf,output_file_path);
			return true;		
		}
		
	}else{
		filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT|overwrite, 0775);
	}
	filehand_src = open_file(input_file_path,O_RDONLY);
	
        excute_copy(filehand_src,filehand_dst);					//一切准备就绪，执行拷贝		
        return true;
}

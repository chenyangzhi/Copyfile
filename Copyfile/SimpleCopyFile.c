//此文件执行相关的拷贝动作
#include "CopyFile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utime.h>
#include <string.h>

#define BUFFSIZE  131072
extern globalArgs ga;
extern file it;
int overwrite = O_EXCL;

static void excute_copy(int src, int dst,struct stat const *src_info)
{
	int n_read;
	char buf[BUFFSIZE+1];
  	char *cp = 0;
  	int *ip ;
  	int n_read_total = 0;
  	int hole_last_write = 0;
	int buf_size = BUFFSIZE;
  	int make_holes = (ga.sparse_mode == SPARSE_ALWAYS);


  	if (ga.sparse_mode == SPARSE_AUTO)
  	{
      		if (S_ISREG (src_info->st_mode) && src_info->st_size / ST_NBLOCKSIZE >  src_info->st_blocks)
			make_holes = 1;
    	}
	while(1)
	{	
		n_read = read_file (src, buf, buf_size);	
      		if (n_read == 0)
			break;

      		n_read_total += n_read;

      		ip = 0;
      		if (make_holes)
		{
	 		buf[n_read] = 1;
	  		ip = (int *) buf;
	  		while (*ip++ == 0)
	    			;

	  		cp = (char *) (ip - 1);
	 		while (*cp++ == 0)
	   			 ;

	  		if (cp > buf + n_read)
	    		{
	      			if (lseek (dst, n_read, SEEK_CUR) < 0L)
				{
		 			error_message("lseek error");
				}
	      			hole_last_write = 1;
			}else
	    			ip = 0;
		}
      		if (ip == 0)
		{
	  		int n = n_read;
			write_file(dst, buf, n);
	  		hole_last_write = 0;
		}
    	}

  	if (hole_last_write)
    	{
      		if (write(dst, "", 1) != 1 || ftruncate (dst, n_read_total) < 0)
		{
			printf("last write erro or ftruncate error/n");
		}
    	}
	close(src);
	close(dst);
	return;
}

int argu_parse_copy(const char* input_file_path,const char* output_file_path)   //执行相关拷贝动作，
{
	
	int filehand_src,filehand_dst = -1;
	char* input;	
	char lf[MAX_PATH_LENGTH];
	struct stat info;
	
	file_status(input_file_path, &info); 
	if(ga.need_backup == true)
	{
		if(backup_method(output_file_path) == false)
			return true;	
	}else if(ga.need_suffix == true){
		if(0 == access(output_file_path,F_OK))
		{
			char temp[MAX_PATH_LENGTH];
			sprintf(temp,"%s",output_file_path);
			strcat(temp,ga.suffix);
			rename(output_file_path,temp);
		}
	}					//提取文件的状态信息
	if(ga.need_interactive)        
	{									//判断交互标志
		if(interactivity_method(output_file_path) == NOT_OVERWRITE)     //
		{
			return  true;
		}
	}else if(ga.need_force == true){
		if( filehand_dst = open(output_file_path,O_WRONLY|O_CREAT|O_EXCL) )
		{
			remove(output_file_path);
			
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
			symbol_link(input_file_path, output_file_path);
			return true;
		}	
			
	}else if(ENUM_SYMLINK == it)
	{
		input = realpath(input_file_path,lf);
		if(input != NULL && ga.need_no_deference == false)                                        //此软链接已经断掉了
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
		
	}else if(filehand_dst == -1){
		filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT|overwrite, 0775);
	}
	filehand_src = open_file(input_file_path,O_RDONLY);

	if( ga.need_attr_only == false)
	{
		excute_copy(filehand_src,filehand_dst,&info);					//一切准备就绪，执行拷贝		
	}
        
        return true;
}


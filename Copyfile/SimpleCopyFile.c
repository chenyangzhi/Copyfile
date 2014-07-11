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
#include <sys/statfs.h> 
#include "hash.h" 
   
#define BUFFSIZE  131072
extern globalArgs ga;
extern file_type it;
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

int prepare_copy(const char* src_path,const char* dst_path,struct stat* src_info,struct stat* dst_info,file_type it,file_type ot)   //prepare the copy with the set argument，
{
	bool backup_succeed;
	int filehand_src,filehand_dst = -1;
	char* src_point_file;	
	char lf[MAX_PATH_LENGTH],dst_backup[MAX_PATH_LENGTH];
	struct utimbuf preserve_timestamp;  
	
	preserve_timestamp.actime = src_info->st_atime;
	preserve_timestamp.modtime = src_info->st_mtime;
	
	filehand_src = open_file(dst_path,O_RDONLY);

	if(0 == access(dst_path,F_OK))
	{ 
		/*backup with special suffix by user set or common suffix ~*/
		if(ga.need_backup == true)
		{
			if(backup_succeed = backup_method(dst_path) == false)
				return true;	
		}
		/*mycp with argument interactive will interactive with the user*/					
		if(ga.need_interactive == true)        
		{									//判断交互标志
			if(interactivity_method(dst_path) == NOT_OVERWRITE)     //
			{
				return  true;
			}
		/*if argument no-clobber after the interactive argument, the no-clobber will overrite the interctive argument*/
		}else if(ga.need_no_clobber == true){
			return true;
		/*the interactive argument are higher priority than force argment*/
		}else if(ga.need_force == true){
			if( filehand_dst = open(dst_path,O_WRONLY|O_CREAT|O_EXCL) )
			{
				remove(dst_path);
			}		
		}else if(ga.need_remove_destination == true){
				remove(dst_path);
		}	
		/*then,to see if need to update*/
		else if(ga.need_update == true){
			
		}else{
			/*cp overwrite the exist file,but i don't think so*/
			printf("the file %s is exist,and omitting it\n",dst_path);
			return false;
		}
	}
	
	/*different copy method depend argument and file type*/
	if(ga.need_symbolic_link == true)
	{
		symbol_link(src_path, dst_path);
	}else if(ga.need_hard_link == true && it != ENUM_DIR){
		struct statfs buf1,buf2;
		if( src_info->st_dev != dst_info->st_dev)
		{
			printf("can't make hard link between with different file system\n");
		}else{
			link(src_path, dst_path);
		}
	}else{
		
	}	

	if(ga.preserve_mode)
	{
		chmod(dst_path, src_info->st_mode);
	}
	if(ga.preserve_timestamps)
	{
		utimes(dst_path,&preserve_timestamp);
	}
	if(ga.preserve_ownership)
	{
		chown(dst_path, src_info->st_uid, src_info->st_gid);
	}					
		
	if (ga.need_verbose && it == ENUM_DIR)
	{
	    	printf ("%s -> %s", src_path, dst_path);
	      	if (backup_succeed)
		printf(" (backup: %s)", dst_backup);
	      	putchar ('\n');
	}
	close(filehand_src);
	close(filehand_dst);
        return true;
}


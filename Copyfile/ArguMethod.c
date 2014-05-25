
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include "CopyFile.h"
#include "hash.h"                         //引入hash算法只是为了有--preserve=links 保持了 源内的链接性

extern globalArgs ga;
void argmatch_valid (char const *const *arglist) 
{	
	int i;
  	//当错误输入发生时，正确输入应该被告知 
 	fputs ("Valid arguments are:", stderr);
  	for (i = 0; arglist[i]; i++)
	    	fprintf(stderr,"%s",arglist[i]);
 	putc ('\n', stderr);
}
int argmatch (const char *arg, const char *const *arglist)//比较选项是否准确匹配
{
	int i;                                            // arglist 的下标 
        int arglen;                                       // arglist的长度
        int no_match_flag = -1;                          // 没有准确匹配的标志

        arglen = strlen (arg);

        //参数与所有的参数相匹配 
  	for (i = 0; arglist[i]; i++)
   	{
      		if (strncmp (arglist[i], arg, arglen) == 0)
        	{
            		no_match_flag = true;
			return i;	
        	}
    	}	
	return no_match_flag;
}
void argmatch_invalid (const char *context, const char *arg)
{
	fprintf(stderr,"for %s: can't resolve #%s#",context,arg);
	return;
}
int argument_match_report (const char *context, const char *arg, const char *const *arglist)
{
	int res = argmatch (arg, arglist);
  	if (res >= 0)
    	//成功匹配 
    		return res;

  	// 没有成功匹配，报告错误原因，及改正的方法 
  	argmatch_invalid (context, arg);
  	argmatch_valid (arglist);
 	return -1;
}

file type_of_file(char* input_file_path)
{
	struct stat info;
	if (stat(input_file_path, &info) == -1) {
             	perror("stat");
		fprintf(stderr,"stat file %s\n",input_file_path);
               	exit(EXIT_FAILURE);
        }
	switch (info.st_mode & S_IFMT) {
          	case S_IFBLK:  printf("block device\n");            return ENUM_BLOCKDEVICE;
           	case S_IFCHR:  printf("character device\n");        break;
          	case S_IFDIR:  printf("directory\n");               return ENUM_DIR;
           	case S_IFIFO:  printf("FIFO/pipe\n");               break;
           	case S_IFLNK:  printf("symlink\n");                 break;
           	case S_IFREG:  printf("regular file\n");            return ENUM_FILE;
           	case S_IFSOCK: printf("socket\n");                  break;
           	default:       printf("unknown?\n");                break;
        }
	return ENUM_FILE;
}

int preserve_method(struct stat info,char* output_file_path,char* input_file_path )
{
	int bufsiz = 512;
	char buf[512];
	int filehand_dst;
	char *ep;
	struct utimbuf preserve_timestamp;
	preserve_timestamp.actime = info.st_atime;
	preserve_timestamp.modtime = info.st_mtime;  
	if(ga.need_preserve == true)
	{
		if(ga.preserve_links)
		{
			static hashmap* hmap = NULL;
			ino_t ik;
			dev_t dk;
			if(hmap == NULL)                                       //首次创建hashtable
				hmap = mk_hmap(int_hash_fn, int_compare_fn);
			if(S_ISLNK(info.st_mode))
			{
				struct stat sb;
				int size = readlink(input_file_path, buf,bufsiz);
				buf[size] = 0;
				stat(buf,  &sb);
				ik = sb.st_ino;
				dk = sb.st_dev;
				input_file_path = realpath(buf,input_file_path);
			}else{
				ik = info.st_ino;
			        dk = info.st_dev;
				sprintf(buf,"%s",input_file_path);
			}	
			
			
			if( (ep = hmap_find(hmap, ik,dk)) != NULL)
			{
				
				link_file(ep, output_file_path);
				return SUCCESS_LINK;
			}else{
				hmap_add(hmap, ik, dk,output_file_path); 
				filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775);
			     }
		}else{	
			filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775);
		}
		if(ga.preserve_mode)
		{
			 chmod(output_file_path, info.st_mode);
		}
		if(ga.preserve_timestamps)
		{
			 utimes(output_file_path,&preserve_timestamp);
		}
		if(ga.preserve_ownership)
		{
			 chown(output_file_path, info.st_uid, info.st_gid);
		}			
	}
	return filehand_dst;
}

void recursive_method(char* input_directory,char* output_directory)
{
	struct dirent* dirent_next = NULL;
	DIR* to_readDir = NULL;
	char cur_inputfile_path[512];
	char cur_outputfile_path[512];       
	memset(cur_inputfile_path,0,sizeof(cur_inputfile_path));
	memset(cur_outputfile_path,0,sizeof(cur_outputfile_path));	
	to_readDir = opendir(input_directory);
	if (!to_readDir)
	{
   		fprintf(stderr,"open directory error\n");
   		exit(EXIT_FAILURE);
	}
	while(0 !=  (dirent_next = readdir(to_readDir)))
	{
		sprintf(cur_inputfile_path,"%s",input_directory);
		sprintf(cur_outputfile_path,"%s",output_directory);
		if(strcmp(".",dirent_next->d_name )== 0|| strcmp("..",dirent_next->d_name) == 0)
		{
			continue;
		}
		strcat(cur_inputfile_path,dirent_next->d_name);
		strcat(cur_outputfile_path,dirent_next->d_name);
		
       		if(type_of_file(cur_inputfile_path) == ENUM_DIR)
		{
			mkdir(cur_outputfile_path,0775);
			strcat(cur_inputfile_path,"/");
			strcat(cur_outputfile_path,"/");
			recursive_method(cur_inputfile_path,cur_outputfile_path);	
		}else{
			if(1 == ga.need_symbolic_link)
			{
				symbol_link(cur_inputfile_path,cur_outputfile_path);
			}
			simple_copyfile(cur_inputfile_path,cur_outputfile_path);		
		}
	}
        closedir(to_readDir);
	return;
}




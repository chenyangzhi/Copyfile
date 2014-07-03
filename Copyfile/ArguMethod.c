//此文件主要定义各个相关参数的方法
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
#include <libgen.h>
#include <sys/statfs.h> 
#include "CopyFile.h"
#include "hash.h"                         //引入hash算法只是为了有--preserve=links 保持了 源内的链接性

extern globalArgs ga;
extern int overwrite;
extern file_type it;
void argmatch_valid (char const *const *arglist)      //告知用户正确的参数输入
{	
	int i;
  	//当错误输入发生时，正确输入应该被告知 
 	fputs ("Valid arguments are:", stderr);
  	for (i = 0; arglist[i]; i++)
	    	fprintf(stderr,"%s  ",arglist[i]);
 	putc ('\n', stderr);
	exit(0);
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
void argmatch_invalid (const char *context, const char *arg)     //输出参数是非法的
{
	fprintf(stderr,"for %s: can't resolve #%s#/t",context,arg);
	printf("please input the right argument\n");
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

file_type type_of_file(const char* input_file_path,struct stat* info)             //判断文件的类型，是目录还是普通文件还是其他的什么
{
	file_status(input_file_path,info);
	switch (info->st_mode & S_IFMT) {
          	case S_IFBLK:             	return ENUM_BLOCKDEVICE;
           	case S_IFCHR:        		return ENUM_CHARDEVICE;
          	case S_IFDIR:              	return ENUM_DIR;
           	case S_IFIFO:           	return ENUM_FP;
           	case S_IFLNK:                	return ENUM_SYMLINK;
           	case S_IFREG:
			if(info->st_nlink == 1)            	
						return ENUM_FILE;
			else
						return ENUM_HARDLINK;	
           	case S_IFSOCK:                  return ENUM_SOCKET;
           	default:                      	return UNKNOWN;
        }
}

bool backup_method(const char* output_file_path)
{
	char new_name[MAX_PATH_LENGTH];
	char file_dir[MAX_PATH_LENGTH];
	char *file_name;
	int len = strlen(output_file_path);
	int i,d,flag;
	struct dirent* dirent_next = NULL;
	DIR* to_readDir = NULL;
	sprintf(file_dir,"%s",output_file_path);
	for(i = len; i >= 0; i--)
	{
		if(file_dir[i] == '/')
		{
			file_dir[i] = 0;
			break;
		}
	}
	strcpy(new_name,output_file_path);
	switch(ga.backup_type){
			case BACKUP_OFF:
				return false;
			case BACKUP_NIL:
			case BACKUP_T:
				i = 0,flag = 0;
				//char tmpname[MAX_PATH_LENGTH];
				file_name = basename(new_name);
				to_readDir = opendir(file_dir);		
				while(0 !=  (dirent_next = readdir(to_readDir)))
				{
					if(strcmp_backup(dirent_next->d_name,new_name,&d))
					{
						if(d > flag)
							flag = d;
					}
				}
				if(ga.backup_type == BACKUP_NIL && flag != 0)
				{
					sprintf(new_name,"%s.~%d~",output_file_path,flag+1);
					rename_file(output_file_path, new_name);
					return true;
				}else{
					sprintf(new_name,"%s.~%d~",output_file_path,flag+1);
					rename_file(output_file_path, new_name);
					return true;
				}
			case BACKUP_NEVER:
			case NO_ARG:
				if(ga.need_suffix == true)
				{
					sprintf(new_name,"%s~",output_file_path);
					if(0 != access(new_name,F_OK))
					{
						rename_file(output_file_path, new_name);
						return true;
					}
					sprintf(new_name,"%s~~",output_file_path);
					if(0 != access(new_name,F_OK))
					{				
						rename_file(output_file_path, new_name);
						return true;
					}
					return false;
				}else{
					
					char temp[MAX_PATH_LENGTH];
					sprintf(temp,"%s",new_name);
					strcat(temp,ga.suffix);
					if(0 != access(new_name,F_OK))
					{
						rename(output_file_path,temp);
						return true;
					}else{
						printf("warning:the file with suffix is exiting and will omit it\n");
						return false;
					}
				}
	}	
}

int interactivity_method(const char* output_file_path)    //交互方法
{
	char c;
	printf("mycp: overwrite '%s'? y/n",output_file_path);
	do {
    		c = getchar();
	   } while (isspace(c));
	if( c == 'y' )
	{	
		overwrite = OVERWRITE_DST ;
		return OVERWRITE;
	}else{
		overwrite = O_EXCL;
		return NOT_OVERWRITE;
	}
}

int preserve_links_method(const struct stat info,const char* input_file_path, const char* output_file_path )   //--preserve links
{
	char buf[MAX_PATH_LENGTH];
	int filehand_dst;
	char *ep;
	
	static hashmap* hmap = NULL;
	ino_t ik;
		dev_t dk;
		if(hmap == NULL)                                       //首次创建hashtable
			hmap = mk_hmap(int_hash_fn, int_compare_fn);
		if(S_ISLNK(info.st_mode))
		{
			struct stat sb;
			//int size = readlink(input_file_path, buf,bufsiz);
			//buf[size] = 0;
			input_file_path = realpath(input_file_path,buf);
			file_status(buf,  &sb);
			ik = sb.st_ino;
			dk = sb.st_dev;
				
		}else{
			ik = info.st_ino;
			dk = info.st_dev;
			//sprintf(buf,"%s",input_file_path);
		}	
		if( (ep = hmap_find(hmap, ik,dk)) != NULL)
		{
				
			link_file(ep, output_file_path);
			return SUCCESS_LINK;
		}else{
			hmap_add(hmap, ik, dk, output_file_path); 
			return NO_LINK;
		}
	return filehand_dst;
}

void recursive_method(const char* input_directory,const char* output_directory)                  //递归的拷贝文件
{
	struct dirent* dirent_next = NULL;
	struct stat child_dir;
	DIR* to_readDir = NULL;
	char cur_inputfile_path[MAX_PATH_LENGTH];
	char cur_outputfile_path[MAX_PATH_LENGTH];       
	memset(cur_inputfile_path,0,sizeof(cur_inputfile_path));
	memset(cur_outputfile_path,0,sizeof(cur_outputfile_path));
	strcat(cur_inputfile_path,"/");
	strcat(cur_outputfile_path,"/");	
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
		
       		if(type_of_file(cur_inputfile_path,&child_dir) == ENUM_DIR)
		{
			if(ga.need_one_filesystem == true)
			{
				struct statfs buf1,buf2;
				if( statfs(cur_inputfile_path, &buf1) != 0 && statfs(cur_inputfile_path, &buf2) )
				{
					error_message("statfs error");
				}
				if(buf1.f_type != buf2.f_type)
				{
					continue;
				}
			}
			mkdir(cur_outputfile_path,0775);	
		}
		prepare_copy(cur_inputfile_path,cur_outputfile_path);		
	}
        closedir(to_readDir);
	return;
}




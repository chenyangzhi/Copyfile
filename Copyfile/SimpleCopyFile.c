#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <utime.h>
#include "CopyFile.h"

#define BUFFSIZE  131072
extern globalArgs ga;
void error_message(const char* errormessage)
{
	perror(str);
	exit(EXIT_FAILURE);
}
int open_file(const char *pathname, int flags, mode_t mode)
{
	int filehand_dst;
	if((filehand_dst = open(output_file_path,flags, mode)) < 0)
	{
		error_message("open dest file error");
	}
	return filehand_dst;
}
int simple_copyfile(char* input_file_path,char* output_file_path)
{
	int n;
	int filehand_src,filehand_dst;
	char buf[BUFFSIZE];
	struct stat info;
	struct utimbuf preserve_timestamp;
	stat(input_file_path, &info);
	utimbuf.actime = info.st_atime;
	utimebuf.modtime = info.st_mtime;    

	filehand_src = open(input_file_path,O_RDONLY);
	filehand_dst = open_file(output_file_path,O_WRONLY|O_CREAT, 0775));
	if(ga.need_preserve == true)
	{
		if(ga.preserve_mode)
		{
			 chmod(output_file_path, info.st_mode);
		}
		if(ga.preserve_timestamp)
		{
			 utimes(output_file_path,&utimbuf);
		}
		if(ga.preserve_ownership)
		{
			  chown(output_file_path, info.st_uid, st_gid);
		}
		if(ga.preserve_links)
		{
			static int 
		}		
	}

        while((n = read(filehand_src,buf,BUFFSIZE)) > 0)
		if (write(filehand_dst,buf,n) != n)
			perror("write error");
  	if(n < 0)
	{
		perror("read error");
		exit(EXIT_FAILURE);
	}		
        return 1;
}


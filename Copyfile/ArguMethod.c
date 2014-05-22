#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "CopyFile.h"

file typeOfFile(char* inputFilePath)
{
	struct stat info;
	if (stat(inputFilePath, &info) == -1) {
             	perror("stat");
		fprintf(stderr,"stat file %s\n",inputFilePath);
               	exit(EXIT_FAILURE);
        }
	switch (info.st_mode & S_IFMT) {
          	case S_IFBLK:  printf("block device\n");            break;
           	case S_IFCHR:  printf("character device\n");        break;
          	case S_IFDIR:  printf("directory\n");               break;
           	case S_IFIFO:  printf("FIFO/pipe\n");               break;
           	case S_IFLNK:  printf("symlink\n");                 break;
           	case S_IFREG:  printf("regular file\n");            return ENUM_FILE;
           	case S_IFSOCK: printf("socket\n");                  break;
           	default:       printf("unknown?\n");                break;
        }
	if( S_ISDIR(info.st_mode) )
	{
		return ENUM_DIR;
	}
}
void recursiveMethod(char* inputDirectory,char* outputDirectory)
{
	struct dirent* direntNext = NULL;
	DIR* toReadDir = NULL;
	char curInputFilePath[512];
	char curOutputFilePath[512];       
	memset(curInputFilePath,0,sizeof(curInputFilePath));
	memset(curOutputFilePath,0,sizeof(curOutputFilePath));	
	toReadDir = opendir(inputDirectory);
	if (!toReadDir)
	{
   		fprintf(stderr,"open directory error\n");
   		exit(EXIT_FAILURE);
	}
	while(0 !=  (direntNext = readdir(toReadDir)))
	{
		sprintf(curInputFilePath,"%s",inputDirectory);
		sprintf(curOutputFilePath,"%s",outputDirectory);
		if(strcmp(".",direntNext->d_name )== 0|| strcmp("..",direntNext->d_name) == 0)
		{
			continue;
		}
		strcat(curInputFilePath,direntNext->d_name);
		strcat(curOutputFilePath,direntNext->d_name);
		
       		if(typeOfFile(curInputFilePath) == ENUM_DIR)
		{
			mkdir(curOutputFilePath,0775);
			strcat(curInputFilePath,"/");
			strcat(curOutputFilePath,"/");
			recursiveMethod(curInputFilePath,curOutputFilePath);	
		}else{
			simpleCopyFile(curInputFilePath,curOutputFilePath);		
		}
	}
        closedir(toReadDir);
	return;
}




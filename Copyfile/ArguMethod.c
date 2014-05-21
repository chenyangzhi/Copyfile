//#include"ArguMethod.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include<stdlib.h>
#include <string.h>

int simpleCopyFile(char* inputFilePath,char* outputFilePath);

void recursiveMethod(char* inputDirectory,char* outputDirectory)
{
	struct dirent* direntNext = NULL;
	DIR* toReadDir = NULL;
	char curInputFilePath[512];
	char curOutputFilePath[512];
        struct stat info;
	memset(curInputFilePath,0,sizeof(curInputFilePath));
	memset(curOutputFilePath,0,sizeof(curOutputFilePath));
	sprintf(curInputFilePath,"%s",inputDirectory);
	sprintf(curOutputFilePath,"%s",outputDirectory);
	toReadDir = opendir(curOutputFilePath);
	if (!toReadDir)
	{
   		fprintf(stderr,"open directory error\n");
   		exit(EXIT_FAILURE);
	}
	while(0 !=  (direntNext = readdir(toReadDir)))
	{
		if(strcmp(".",direntNext->d_name)|| strcmp("..",direntNext->d_name))
		{
			continue;
		}
       		if(S_ISDIR(info.st_mode))
		{
			strcat(curInputFilePath,direntNext->d_name);
			strcat(curOutputFilePath,direntNext->d_name);
			mkdir(curOutputFilePath,0775);
			recursiveMethod(curInputFilePath,curOutputFilePath);	
		}else{
			simpleCopyFile(curInputFilePath,curOutputFilePath);		
		}
	}
        closedir(toReadDir);
	return;
}



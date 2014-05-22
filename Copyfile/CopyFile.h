#ifndef COPYFILR_H
#define COPYFILE_H

#define true 1
#define false 0
typedef enum file{
  ENUM_DIR = 0, 
  ENUM_FILE,
  ENUM_BLOCKDEVICE
}file;

file typeOfFile(char* inputFilePath);
int simpleCopyFile(char* inputFilePath,char* outputFilePath);
void recursiveMethod(char* input,char* output);
#endif

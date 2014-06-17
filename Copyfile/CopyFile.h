#ifndef COPYFILR_H
#define COPYFILE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>

#define true 1
#define false 0
#define SUCCESS_LINK -2
#define MAX_PATH_LENGTH 512
#define DEBUG_INFO(s,d)  printf("Debug info:"),printf(s,d)
#define NOT_OVERWRITE 1
#define OVERWRITE 0
#define FILE_NOT_EXIST -1
#define OVERWRITE_DST 00000000
#define ST_NBLOCKSIZE 512

typedef enum file{
  UNKNOWN = -1,
  ENUM_DIR = 0, 
  ENUM_FILE,
  ENUM_SYMLINK,
  ENUM_BLOCKDEVICE,
  ENUM_CHARDEVICE,
  ENUM_FP,
  ENUM_SOCKET
}file;
typedef enum backup_arg {
	BACKUP_OFF = 0,
	BACKUP_T,
	BACKUP_NIL,
	BACKUP_NEVER,
	NO_ARG
}backup_arg;
typedef enum reflink_arg{
	ALWAYS,
	AUTO
}reflink_arg;
typedef enum sparse_arg {
	SPARSE_AUTO,
	SPARSE_ALWAYS,
	SPARSE_NEVER
}sparse_arg;
/* How to handle symbolic links.  */
typedef enum Dereference_symlink
{
  DEREF_UNDEFINED = 1,       // Copy the symbolic link itself.  -P 
  DEREF_NEVER,
  DEREF_COMMAND_LINE_ARGUMENTS,
  DEREF_ALWAYS
  /* If the symbolic is a command line argument, then copy
     its referent.  Otherwise, copy the symbolic link itself.  -H  */

  /* Copy the referent of the symbolic link.  -L  */

};

typedef struct globalArgs {
        int need_archive;                       /*-a option*/
	int need_attr_only;
	int need_backup;
	int need_copy_contents;
	int need_no_clobber;                    /*-n option*/
     	int need_force;                         /*-f option*/
  	int need_interactive;                   /*-i option*/
      	int need_link;                          /*-l option*/
 	int need_deference;                     /*-L option*/
	int need_no_deference;                  /*-P option*/
        int need_preserve;                      /*-p option*/
	int need_parents;
	int need_no_preserve;
	int need_suffix;
	int need_reflink;
	int preserve_ownership;
 	int preserve_mode;
  	int preserve_timestamps;
	int preserve_links;
        int need_recursive;                     
        int need_symbolic_link;
	int need_sparse;
	int need_one_filesystem;
	int need_help;
	int need_version;
	int need_verbose;
	sparse_arg sparse_mode;                 
	int num_src_files;
	char* suffix;
	char* parent_dir;
	backup_arg backup_type;                  /*-b option*/                
                
} globalArgs;

file type_of_file(const char* inputFilePath);
int argu_parse_copy(const char* inputFilePath,const char* outputFilePath);
void recursive_method(const char* input,const char* output);
int preserve_method(struct stat info, const char* input_file_path, const char* output_file_path);
void argmatch_valid (char const *const *arglist);
int argmatch (const char *arg, const char *const *arglist);
void argmatch_invalid (const char *context, const char *value);
int argument_match_report (const char *context, const char *arg, const char *const *arglist);
char* absolute_path(const char* relapath,char real_name[]);
#endif

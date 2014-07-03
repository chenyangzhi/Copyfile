#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#include "CopyFile.h"
# define no_argument     	0    
# define required_argument	1    
# define optional_argument	2   
# define ERROR_ARGUMENT        -1
# define ARG_MATCH(Context, Arg, Arglist, Vallist)              \
  ((Vallist) [argument_match_report (Context, Arg, Arglist)])	  
enum
{
  COPY_CONTENTS_OPTION = CHAR_MAX + 1,
  ATTRIBUTES_ONLY_OPTION,
  NO_PRESERVE_ATTRIBUTES_OPTION,
  PARENTS_OPTION,
  PRESERVE_ATTRIBUTES_OPTION,
  REPLY_OPTION,
  SPARSE_OPTION,
  TARGET_DIRECTORY_OPTION,
  UNLINK_DEST_BEFORE_OPENING,
  REFLINK_OPTION
};
enum File_attribute
{
   PRESERVE_MODE=0,
   PRESERVE_TIMESTAMPS,
   PRESERVE_OWNERSHIP,
   PRESERVE_LINK,
   PRESERVE_ALL 
};
globalArgs ga;
extern int overwrite;
static const char* optString = "abdfHilLprstuvxPRS:TV:";

static char const* const preserve_args[] =
{
   "mode", "timestamps", "ownership", "links", "all", 0
};
static char const* const reflink_args[] = 
{
   "none", "auto", 0
};
static char const* const backup_args[] =
{
   "off", "t", "nil","simple",0
};
static char const* const sparse_args[] =
{
   "auto", "always", "never",0
};
//
static sparse_arg decode_sparse_arg(char const* optarg)
{
	sparse_arg arg_val;
	char* optarg_writable = strdup(optarg);
	char *s = optarg_writable;
	sparse_arg const sparse_vals[] = 
	{
		SPARSE_AUTO,
		SPARSE_ALWAYS,
		SPARSE_NEVER
	};
	arg_val = ARG_MATCH("--reflink",s,sparse_args,sparse_vals);
}
static reflink_arg decode_relink_arg(char const* optarg)
{
	reflink_arg arg_val;
	char* optarg_writable = strdup(optarg);
	char *s = optarg_writable;
	reflink_arg const reflink_vals[] = 
	{
		ALWAYS,
		AUTO,
	};
	arg_val = ARG_MATCH("--reflink",s,reflink_args,reflink_vals);
}
static backup_arg decode_backup_arg(char const *optarg)
{
	backup_arg arg_val;
	char *optarg_writable = strdup (optarg);
	char *s = optarg_writable;
	backup_arg const backup_vals[] = 
	{
		BACKUP_OFF ,
		BACKUP_T,
		BACKUP_NIL,
		BACKUP_NEVER
	};
	arg_val = ARG_MATCH("--backup",s,backup_args,backup_vals);
	
	return arg_val;
}
static void decode_preserve_arg (char const *optarg)
{
  enum File_attribute const preserve_vals[] =
    {
      PRESERVE_MODE, PRESERVE_TIMESTAMPS,
      PRESERVE_OWNERSHIP, PRESERVE_LINK, PRESERVE_ALL
    };

  char *optarg_writable = strdup (optarg);
  char *s = optarg_writable;
  do{
      char *comma = strchr (s, ',');
      enum File_attribute attribute_val;
      if (comma)                          /*把逗号的换成0,这样就把参数提取出来，接下来，判断他们的类型和合法性*/
	*comma++ = 0;
     	attribute_val = ARG_MATCH("--preserve",s, preserve_args, preserve_vals);
      switch (attribute_val)
	{
	 case PRESERVE_MODE:
	 	ga.preserve_mode = true;
	  	break;

	 case PRESERVE_TIMESTAMPS:
	 	ga.preserve_timestamps = true;
	  	break;

	 case PRESERVE_OWNERSHIP:
	 	ga.preserve_ownership = true;
	  	break;

	 case PRESERVE_LINK:
	  	ga.preserve_links = true;		
	  	break;

	 case PRESERVE_ALL:
	  	ga.preserve_mode = true;
	  	ga.preserve_timestamps = true;
	  	ga.preserve_ownership = true;
	  	ga.preserve_links = true;
	  	break;

	 default:
	  	fprintf(stderr,"%s","Can't resolve the argument\n");
	  	exit(EXIT_FAILURE);
	}
      s = comma;
    }while (s);
  free (optarg_writable);
}
//输出正确的用法.

void display_usage( int exit_status )
{
	puts( "the useage is: cp [OPTION]... [-T] SOURCE DEST\
             cp [OPTION]... SOURCE... DIRECTORY\
       cp [OPTION]... -t DIRECTORY SOURCE..." );
	exit( exit_status );
}
bool is_directory(const char* pathname)
{
	return pathname[strlen(pathname) - 1] == '/'? 1:0;
}
char* target_file_parse(const char* pathname,char dst_name[],file_type* ot,struct stat* dst_info)
{
	if(0 == access(pathname,F_OK))
	{
		*ot = type_of_file(pathname,dst_info);
		if(ENUM_DIR != ot)                        //判断输出文件的类型
		{
			if(ga.num_src_files > 1)
			{
				display_usage(EXIT_FAILURE);
				printf("too many files for output file,the output must be a directory");
				return false;
			}
			return realpath(pathname,dst_name);
		}else{
	
			if((realpath(pathname,dst_name)) == NULL)
			{
				printf("realpath error\n");
				return false;
			}else{						
				strcat(dst_name,"/");             //如果输出的文件是目录，则在目录路径名后面加上“/”
				return dst_name;
			}
		}
	}else{	
		if( ga.num_src_files > 1)
		{
			printf("can't creat directory %s\n",pathname);
			return false;
		}else{				
			absolute_path(pathname,dst_name);
			if(ga.need_recursive)
			{
				mkdir(dst_name,0775);
				ot = ENUM_DIR;
				return strcat(dst_name,"/");
			}else{
				printf("the target %s is directory",dst_name);
				return false;
			}
		}
	}
}

int prepare_target_file(const char* src_file, const char* target_file,file_type ot)    //dst_path is the last pathname for copying  
{
	file_type it;
	struct stat src_info,dst_info;
	
	it = type_of_file(src_file,&src_info);
	stat(target_file,&dst_info);
	return prepare_copy(src_file, target_file,&src_info,&dst_info,it,ot);
}
bool argu_parse()
{
	int flag;
	if (ga.num_src_files <= 0)
    	{
     		fprintf(stderr,"%s","missing file argument");
      		return false;
    	}

	if (ga.need_hard_link && ga.need_symbolic_link)
	{
		fprintf(stderr,"%s","cannot make both hard and symbolic links");
		display_usage(EXIT_FAILURE);
		return false;
	}
	
	return true;	
}
int main( int argc, char *argv[] )
{
	bool exit_status;
	int i,ol,opt = 0;
	char* target_file,*src_file;
	char absolute_src_path[MAX_PATH_LENGTH];
	char absolute_target_path[MAX_PATH_LENGTH];
	struct stat dst_info;
	file_type ot = -1;
	file_type it = -1;
	struct option long_options[] = {{"archive", no_argument, NULL, 'a'},
					{"attributes-only", no_argument, NULL,ATTRIBUTES_ONLY_OPTION },
 					{"backup", optional_argument, NULL, 'b'},
  					{"copy-contents", no_argument, NULL, COPY_CONTENTS_OPTION},
  					{"deference", no_argument, NULL, 'L'},
  					{"force", no_argument, NULL, 'f'},
  					{"interactive", no_argument, NULL, 'i'},
 				        {"link", no_argument, NULL, 'l'},
   					{"no-deference", no_argument, NULL, 'P'},
					{"no-clobber",no_argument,NULL,'n'},
  					{"no-preserve", required_argument, NULL, NO_PRESERVE_ATTRIBUTES_OPTION},
  					{"one-file-system", no_argument, NULL, 'x'},
  					{"parents", no_argument, NULL, PARENTS_OPTION},
  					{"preserve", optional_argument, NULL, 'p'},
  					{"recursive", no_argument, NULL, 'R'},
  					{"remove-destination", no_argument, NULL, UNLINK_DEST_BEFORE_OPENING},
  					{"reply", required_argument, NULL, REPLY_OPTION},
  					{"sparse", required_argument, NULL, SPARSE_OPTION},
					{"reflink", optional_argument, NULL, REFLINK_OPTION},
  					{"suffix", required_argument, NULL, 'S'},
  					{"symbolic-link", no_argument, NULL, 's'},
 				 	{"update", no_argument, NULL, 'u'},
  					{"verbose", no_argument, NULL, 'v'},
					{"version",no_argument,NULL,'V'},
					{"help",no_argument,NULL,'h'},
  					{NULL, 0, NULL, 0}
				     };
	
	opt = getopt_long( argc, argv,optString,long_options,NULL );
	while( opt != -1 ) {
		switch( opt ) {
			case 'a':
				ga.need_no_deference = true;
				ga.preserve_mode = true;
				ga.preserve_ownership = true;
				ga.preserve_timestamps = true;
				ga.preserve_links = true;
				ga.need_preserve =true;
				ga.need_recursive = true;
				break;
			case ATTRIBUTES_ONLY_OPTION:
				ga.need_attr_only = true;
				ga.preserve_mode = true;
				ga.preserve_ownership = true;
				ga.preserve_timestamps = true;
				ga.preserve_links = true;
				ga.need_preserve =true;
				break;
			case 'b':
				ga.need_backup = true; 
				if(optarg != NULL)
					ga.backup_type = decode_backup_arg(optarg);
				else
					ga.backup_type = NO_ARG;
				break;
			case 'd':
				ga.need_no_deference = true;
				ga.need_preserve = true;
				ga.preserve_links = true;
				break;
			case COPY_CONTENTS_OPTION:
				ga.need_copy_contents = true;
				break;
			case 'f':
				ga.need_force = true;
				overwrite = 0;
				break;
			case 'l':
				ga.need_hard_link = 1;
	  			break;
			case 'L':
				ga.need_deference = true;
				break;
			case 'r':
			case 'R':
				ga.need_recursive = 1;    
				break;
			case 's':
				ga.need_symbolic_link = 1;
				break;
			case 'S':
				ga.need_suffix = true;
				ga.need_backup = true;
				ga.backup_type = NO_ARG; /*the backup suffix will be set with suffix argument*/
				if(optarg != NULL)
				{
					ga.suffix = strdup(optarg);
				}else{
					fprintf(stderr,"%s","the suffix argument must required argument");
				}
				break;
			case 'p':
          			if (optarg == NULL)
				{
				      // 与下面的p选项处理相同
				}else {
				        decode_preserve_arg (optarg);
				        ga.need_preserve = true;
				        break;
				}
		       	case 'P':
				ga.need_no_deference = true;                         //只对symbolic有用
				break;
			case PARENTS_OPTION:
				ga.need_parents = true;
				break;
			case NO_PRESERVE_ATTRIBUTES_OPTION:
				if(optarg == NULL)
				{
					printf("warning no attribute specified with no preserve option\n");
				}else{
					ga.need_no_preserve = true;
					decode_preserve_arg (optarg);
				}
				break;
			case SPARSE_OPTION:
				ga.need_sparse = true;
				ga.sparse_mode = decode_sparse_arg(optarg);
				break;
			case REFLINK_OPTION:
				ga.need_reflink = true;
				break;
			case 'i':
				ga.need_interactive = ASK_USER;
				break; 
			case 'n':
				ga.need_no_clobber = true; 
				ga.need_interactive = false;
				break;
			case 'u':
				ga.need_update = true;
				break;
			case 'x':
				ga.need_one_filesystem = true;
				break;
			case 'h':
				ga.need_help = true;
				break;
			case 'v':
				ga.need_verbose = true;
				break;
			case 'V':
				ga.need_version = true;
				break;
			case UNLINK_DEST_BEFORE_OPENING:
				ga.need_unlink = true;
				break;
			default:
				display_usage(EXIT_FAILURE);
				break;

		}
		
		opt = getopt_long(argc, argv,optString,long_options,NULL);
	}

	ga.num_src_files = argc - optind - 1;       //number of source files
	if(argu_parse() == true)
	{
		sprintf(absolute_target_path,"%s",argv[argc-1]);
		if(!(target_file = target_file_parse(argv[argc-1],absolute_target_path,&ot,&dst_info)))
		{
			exit(EXIT_FAILURE);
		}	
		ol = strlen(target_file);
		for(i = optind; i < argc-1 ; i++)
		{
			if(access_file(argv[i],F_OK) == false)                            //判断文件可不可达
			{
				continue;
			}
			if(ga.need_parents == true)
			{
				char p[MAX_PATH_LENGTH];
				sprintf(p,"%s",argv[i]);
				target_file[ol] = '\0';
				make_parent_dir(target_file,p,0775);
			}			
			/*the relative src path -> absolute src path,if it is a symbolic file ,don't dereference it*/
			src_file = absolute_path(argv[i],absolute_src_path);
			if(str_cmp(src_file,target_file,ot) == false)
			{
		  		exit_status = prepare_target_file(src_file,target_file,ot);
			}else{
				printf("it can't copy the same file twice in same directory\n");//
				exit_status = EXIT_FAILURE;
			}
		}
	}
        	
	exit(exit_status);
}

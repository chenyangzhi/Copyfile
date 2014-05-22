#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <libgen.h>
#include <string.h>
#include "CopyFile.h"

# define no_argument		0    
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
  STRIP_TRAILING_SLASHES_OPTION,
  TARGET_DIRECTORY_OPTION,
  UNLINK_DEST_BEFORE_OPENING
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

static char const* const preserve_args[] =
{
   "mode", "timestamps", "ownership", "links", "all", 0
};

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

static const char* optString = "abdfHilLprstuvxPRS:TV:";

/* xianshi yongfa bingqie tuichu.
 */
void display_usage( void )
{
	puts( "the useage is: cp [OPTION]... [-T] SOURCE DEST\
             cp [OPTION]... SOURCE... DIRECTORY\
       cp [OPTION]... -t DIRECTORY SOURCE..." );
	exit( EXIT_FAILURE );
}

void argu_action_excute()
{
	int i;
	
	if(1 == ga.need_recursive && (type_of_file(ga.input_file) == ENUM_DIR))
	{		
		strcat(ga.output_file,basename(ga.input_file));
		mkdir(ga.output_file,0775);
		strcat(ga.output_file,"/");
		strcat(ga.input_file,"/");
		recursive_method(ga.input_file,ga.output_file);
	}else if(1 == ga.need_symbolic_link){
		if(-1 == symlink(ga.input_file,ga.output_file))
		{
			perror("symlink error");
		}				
	}else{
		simple_copyfile(ga.input_file,ga.output_file);
	}
}

int main( int argc, char *argv[] )
{
	int i,opt = 0;
	ga.need_recursive = 0;		/* false */
	char real_inputfile_path[512];
	char real_outputfile_path[512];
	char outputfile_dir[512];
	char outputnewfile_name[512];
	char* temp_argv;	
	struct option long_options[] = { {"archive", no_argument, NULL, 'a'},
					{"attributes_only", no_argument, NULL,ATTRIBUTES_ONLY_OPTION },
 					{"backup", optional_argument, NULL, 'b'},
  					{"copy-contents", no_argument, NULL, COPY_CONTENTS_OPTION},
  					{"dereference", no_argument, NULL, 'L'},
  					{"force", no_argument, NULL, 'f'},
  					{"interactive", no_argument, NULL, 'i'},
 				        {"link", no_argument, NULL, 'l'},
   					{"no-dereference", no_argument, NULL, 'P'},
					{"no-clobber",no_argument,NULL,'n'},
  					{"no-preserve", required_argument, NULL, NO_PRESERVE_ATTRIBUTES_OPTION},
  					{"one-file-system", no_argument, NULL, 'x'},
  					{"parents", no_argument, NULL, PARENTS_OPTION},
  					{"preserve", optional_argument, NULL, 'p'},
  					{"recursive", no_argument, NULL, 'R'},
  					{"remove-destination", no_argument, NULL, UNLINK_DEST_BEFORE_OPENING},
  					{"reply", required_argument, NULL, REPLY_OPTION},
  					{"sparse", required_argument, NULL, SPARSE_OPTION},
  					{"strip-trailing-slashes", no_argument, NULL, STRIP_TRAILING_SLASHES_OPTION},
  					{"suffix", required_argument, NULL, 'S'},
  					{"symbolic-link", no_argument, NULL, 's'},
  					{"target-directory", required_argument, NULL, 't'},
 				 	{"update", no_argument, NULL, 'u'},
  					{"verbose", no_argument, NULL, 'v'},
					{"help",no_argument,NULL,'h'},
  					{NULL, 0, NULL, 0}
				     };

	opt = getopt_long( argc, argv,optString,long_options,NULL );
	while( opt != -1 ) {
		switch( opt ) {
			case 'r':
			case 'R':
				ga.need_recursive = 1;    
				break;
			case 's':
				ga.need_symbolic_link = 1;
			case PRESERVE_ATTRIBUTES_OPTION:
          			if (optarg == NULL)
				{
				      /* 与下面的p选项处理相同  */
				}else {
				        decode_preserve_arg (optarg);
				        ga.need_preserve = true;
				        break;
				}

				case 'p':
					ga.preserve_ownership = true;
				  	ga.preserve_mode = true;
				 	 ga.preserve_timestamps = true;
				  	ga.need_preserve = true;
				  	break;

		}
		
		opt = getopt_long(argc, argv,optString,long_options,NULL);
	}
	ga.num_of_files = argc - optind;
	sprintf(outputnewfile_name,"%s",basename(argv[argc-1]));
	sprintf(outputfile_dir,"%s",dirname(argv[argc-1]));
	
	if(realpath(outputfile_dir,real_outputfile_path) == 0)
	{
		
		perror("realpath error");
		exit(EXIT_FAILURE);
	}else{
		strcat(real_outputfile_path,"/");
		strcat(real_outputfile_path,outputnewfile_name);
		ga.output_file =  real_outputfile_path;
	}

        for(i = optind; i < argc-1 ; i++)
	{
		ga.input_file = realpath(argv[optind],real_inputfile_path);
  		argu_action_excute(ga.input_file);
	}
        
	
	exit(EXIT_SUCCESS);
}

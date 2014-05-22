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

#define argument_match(argu_type, s, args, option)         \
{ 	                                                   \
	char *saveptr1,*subtoken;    \
	enum File_attribute i;				   \
	if(argu_type == "--preserve")		           \
	{                                                  \
		str1 = s;				   \
        	subtoken = strtok_r(str1, ',', &saveptr1);        \
        	if (subtoken == NULL)                             \
                	ERROR_ARGUMENT;			          \
		for(i = PRESERVE_MODE; i < sizeof(args);i = i+1)  \
		{                                                 \
			if(subtoken == args[i])                   \
			{	                                  \
				printf(" --> %s\n", subtoken);    \
                   		break;                            \
			}					  \
		}						  \
	}                                                         \
	i;							  \
}
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
   ERROR_ARGUMENT,
   PRESERVE_MODE=0,
   PRESERVE_TIMESTAMPS,
   PRESERVE_OWNERSHIP,
   PRESERVE_LINK,
   PRESERVE_ALL
};

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
      if (comma)
	*comma++ = 0;
     	attribute_val = argument_match("--preserve",s, preserve_args, preserve_vals);
      switch (attribute_val)
	{
	 case PRESERVE_MODE:
	 	ga->preserve_mode = true;
	  	break;

	 case PRESERVE_TIMESTAMPS:
	 	ga->preserve_timestamps = true;
	  	break;

	 case PRESERVE_OWNERSHIP:
	 	ga->preserve_ownership = true;
	  	break;

	 case PRESERVE_LINK:
	  	ga->preserve_links = true;
	  	break;

	 case PRESERVE_ALL:
	  	ga->preserve_mode = true;
	  	ga->preserve_timestamps = true;
	  	ga->preserve_ownership = true;
	  	ga->preserve_links = true;
	  	break;

	 default:
	  	fprintf(stderr,"%s","Can't resolve the argument\n");
	  	exit(EXIT_FAILURE);
	}
      s = comma;
    }while (s);
  free (optarg_writable);
}

typedef struct globalArgs {
        int needArchive;                       /*-a option*/
        int needBackup;                        /*-b option*/
        int needNoDerence;                    /*-d option*/
     	int needForce;                         /*-f option*/
  	int needInteractive;                   /*-i option*/
      	int needLink;                          /*-l option*/
 	int needDereference;                   /*-L option*/
	int needNoclobber;                    /*-n option*/
	int needNodereference;                /*-P option*/
        int needPreserve;                      /*-p option*/
	int preserve_ownership;
 	int preserve_mode;
  	int preserve_timestamps;
	int preserve_links;
        int needRecursive;                     
        int needSymbolic_link;                 
	int numOfFiles;
	char* inputFile;
        char* outputFile;                
                
} globalArgs;
globalArgs ga;
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

void arguActionExcute()
{
	int i;
	
	if(1 == ga.recursive && (typeOfFile(ga.inputFile) == ENUM_DIR))
	{		
		strcat(ga.outputFile,basename(ga.inputFile));
		mkdir(ga.outputFile,0775);
		strcat(ga.outputFile,"/");
		strcat(ga.inputFile,"/");
		recursiveMethod(ga.inputFile,ga.outputFile);
	}else if(1 == ga.symbolic_link){
		if(-1 == symlink(ga.inputFile,ga.outputFile))
		{
			perror("symlink error");
		}				
	}else if(1 == ga.preserve)
	{
		preserveMethod();
	}
	else{
		simpleCopyFile(ga.inputFile,ga.outputFile);
	}
}

int main( int argc, char *argv[] )
{
	int i,opt = 0;
	ga.recursive = 0;		/* false */
	char realInputFilePath[512];
	char realOutputFilePath[512];
	char outputFileDir[512];
	char outputNewfileName[512];
	char* temp_argv;	
	struct option longOptions[] = { {"archive", no_argument, NULL, 'a'},
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

	opt = getopt_long( argc, argv,optString,longOptions,NULL );
	while( opt != -1 ) {
		switch( opt ) {
			case 'r':
			case 'R':
				ga.recursive = 1;    
				break;
			case 's':
				ga.symbolic_link = 1;
			case 'p':
				ga.preserve = 1; 


		}
		
		opt = getopt_long(argc, argv,optString,longOptions,NULL);
	}
	ga.numOfFiles = argc - optind;
	sprintf(outputNewfileName,"%s",basename(argv[argc-1]));
	sprintf(outputFileDir,"%s",dirname(argv[argc-1]));
	
	if(realpath(outputFileDir,realOutputFilePath) == 0)
	{
		
		perror("realpath error");
		exit(EXIT_FAILURE);
	}else{
		strcat(realOutputFilePath,"/");
		strcat(realOutputFilePath,outputNewfileName);
		ga.outputFile =  realOutputFilePath;
	}

        for(i = optind; i < argc-1 ; i++)
	{
		ga.inputFile = realpath(argv[optind],realInputFilePath);
  		arguActionExcute(ga.inputFile);
	}
        
	
	exit(EXIT_SUCCESS);
}

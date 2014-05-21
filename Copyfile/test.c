#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct globalArgs {
        int archive;                       /*-a option*/
        int backup;                        /*-b option*/
        int no-derence;                    /*-d option*/
     	int force;                         /*-f option*/
  	int interactive;                   /*-i option*/
      	int link;                          /*-l option*/
 	int dereference;                   /*-L option*/
	int no-clobber;                    /*-n option*/
	int no-dereference;                /*-P option*/
        int preserve;                      /*-p option*/
        int recursive;                     /*-r or R option*/
        int symbolic-link                  /*-s option*/
	char* inputFiles[];
        char* outputDirectory;                
        
        
} globalArgs;

static const char *optString = "rR";

/* xianshi yongfa bingqie tuichu.
 */
void display_usage( void )
{
	puts( "the useage is: cp [OPTION]... [-T] SOURCE DEST\
             cp [OPTION]... SOURCE... DIRECTORY\
       cp [OPTION]... -t DIRECTORY SOURCE..." );
	exit( EXIT_FAILURE );
}


int main( int argc, char *argv[] )
{
	int opt = 0;
	
	globalArgs.recursive = 0;		/* false */

	opt = getopt( argc, argv, optString );
	while( opt != -1 ) {
		switch( opt ) {
			case 'r':
				globalArgs.recursive = 1;    /* true */
				break;
			case 'R':
				globalArgs.recursive = 1;    /* true */
				break;


		}
		
		opt = getopt( argc, argv, optString );
	}
	
	globalArgs.inputFiles = argv + optind;
	globalArgs.outputDirectory = argc - optind;
        
	
	return EXIT_SUCCESS;
}

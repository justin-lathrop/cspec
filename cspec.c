#include <stdio.h>
#include <unistd.h>
#include <ftw.h>
#include <stdint.h>
#include <dlfcn.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

#include "cspec_tree.h"
#include "cspec_list.h"

/*
 * This executable is meant to be run as a unit 
 * test runner.  This means that it should be the 
 * thing that actually runs the tests and manages
 * the cspec_tree internally.  When invoked from 
 * the command line it will (per the arguments) 
 * loop through all .so files and run the tests 
 * within them.
 */

// Needs to be global because of nftw callback
char * prefix;
typedef void (* cspec_func)(cspec_tree_t *);

/*
 * Prints the usage of 'cspec'.
 */
void usage(char * location){
	printf("Usage: %s", location);
}

/*
 * Runs a simple regex check/test on any string given, 
 * along with the given pattern the compile.
 */
bool regex_test(char * regex_pattern, const char * compare){
	regex_t regex;
	regcomp(&regex, regex_pattern, 0);
	int regcheck = regexec(&regex, compare, 0, NULL, 0);
	regfree(&regex);

	return(regcheck == 0);
}

/*
 * This runs the system command 'nm' (linux command) to 
 * find all of the _CSPEC_handle_ symbols and returns a 
 * list of them.
 */
cspec_list_t * find_cspec_symbols(const char * path){
	FILE * fp;
	char output[1035];
	char command[1035];
	cspec_list_t * symbols = cspec_list_initialize();

	strcpy(command, "nm -j ");
	strcat(command, path);
	fp = popen(command, "r");

	while(fgets(output, sizeof(output)-1, fp) != NULL){
		// Remove newline
		output[strcspn(output, "\r\n")] = 0;

		if(strlen(output) > 14 && strncmp(output, "_CSPEC_handle_", 14) == 0){
			char * symbol = (char *)malloc(sizeof(char) * strlen(output));
			strcpy(symbol, output);

			cspec_list_add(symbols, (void *)symbol);
		}
	}

	pclose(fp);
	return(symbols);
}

/*
 * Given a verified cspec test, this will collect the 
 * corresponding symbols and run each of them to build 
 * the cspec tree.  Then run the tree.
 */
void process_cspec(const char * path){
	cspec_list_t * symbols = find_cspec_symbols(path);

	if(symbols != NULL && cspec_list_size(symbols) > 0){
		cspec_tree_t * cspec_tree = cspec_tree_initialize();
		int size = cspec_list_size(symbols);
		void * handle = dlopen(path, RTLD_NOW|RTLD_GLOBAL);

		if(!handle){
            printf("%s\n", dlerror());
            return;
        }

		for(int i = 0; i < size; i++){
			char * symbol = (char *)cspec_list_get(symbols, i);
			cspec_func func = (cspec_func)dlsym(handle, symbol);

			char * error;
	        if((error = dlerror()) != NULL){
	            printf("%s\n", error);
	            return;
	        }
			
			printf("BEFORE\n");
			func(cspec_tree);
			printf("AFTER\n");
		}

		// TODO: Now run through the tree...

		dlclose(handle);
	}
}

/*
 * This is the callback to the ftw (file tree walker) function 
 * call.  It checks that the file is the correct file type and 
 * is a .so file.  If then passes the path to the cspec test 
 * on to the process_cspec function to extract the CSPEC symbols 
 * and run the tree.
 */
int process_possible_cspec(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
	// Make sure file/directory isn't hidden
	if(regex_test("/\\..*", &fpath[strlen(prefix)])){ return(0); }

	// Make sure its a .so file
	if(!regex_test("\\.so$", fpath)){ return(0); }

	// Make sure its a file and not a directory
	if(tflag == FTW_D){ return(0); }

	// Finally try to process the cspec
	process_cspec(fpath);

	return(0);
}

/*
 * Collect the arguments and start walking the tree at 
 * the prefix given, running only valid cspec tests.
 */
int main(int argc, char ** argv){
	int c = -1;
	prefix = ".";

	while((c = getopt(argc, argv, "hp:")) != -1){
		switch(c){
		case 'h':
			usage(argv[0]);
			break;
		case 'p':
			prefix = optarg;
			break;
		}
	}

	if (nftw(prefix, process_possible_cspec, 20, 0) == -1) {
        perror("nftw");
    }

	return(0);
}

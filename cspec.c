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

// Needs to be global...
int num_failed_tests;
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

		// If '_' is first character, remove it
		if(output[0] == '_'){ memmove(output, output+1, strlen(output)); }

		if(regex_test("^CSPEC_handle_.*", output)){
			char * symbol = (char *)malloc(sizeof(char) * strlen(output)+1);
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
		void * handle = dlopen(path, RTLD_LAZY);

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
			
			func(cspec_tree);
		}

        num_failed_tests += cspec_tree_iterate(cspec_tree);

		dlclose(handle);
		cspec_tree_free(cspec_tree);
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
    num_failed_tests = 0;

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
    
    /*cspec_tree_t * tree = cspec_tree_initialize();
    cspec_tree_add_before(tree, "", "", NULL);
    cspec_tree_add_after(tree, "", "", NULL);
    
    cspec_tree_add_describe(tree, "TEST DESCRIBE");
    cspec_tree_add_before(tree, "TEST DESCRIBE", "", NULL);
    cspec_tree_add_after(tree, "TEST DESCRIBE", "", NULL);
    
    cspec_tree_add_it(tree, "TEST DESCRIBE", "TEST 1", NULL);
    cspec_tree_add_before(tree, "TEST DESCRIBE", "TEST 1", NULL);
    cspec_tree_add_after(tree, "TEST DESCRIBE", "TEST 1", NULL);
    
    cspec_tree_iterate(tree);*/
    
	return(num_failed_tests);
}

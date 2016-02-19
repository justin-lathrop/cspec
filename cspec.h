#ifndef __CSPEC__
#define __CSPEC__

#include "cspec_tree.h"

/*
 * This is the main entry point into CSpec, these defines 
 * are here to wrap the cspec_tree library for ease of 
 * use.  This is expected to be built into a shared library 
 * which then will be dynamically loaded by the cspec test 
 * runner executable which will then go through the tests.
 */

#define __append_helper(a, b) a##b
#define __append(a, b) __append_helper(a, b)
#define __stringize(a) #a

#define describe(name)\
void __append(CSPEC_handle_describe_, name)(cspec_tree_t * cspec_tree){\
	cspec_tree_add_describe(cspec_tree, __stringize(name));\
}

#define before(describe_key, it_key, block)\
void __append(CSPEC_before_, __LINE__)(){\
	block\
}\
void __append(CSPEC_handle_before_, __LINE__)(cspec_tree_t * cspec_tree){\
	cspec_tree_add_before(cspec_tree, __stringize(describe_key), __stringize(it_key), __append(CSPEC_before_, __LINE__));\
}

#define it(describe_key, name, block)\
void __append(CSPEC_it_, __LINE__)(){\
	block\
}\
void __append(CSPEC_handle_it_, __LINE__)(cspec_tree_t * cspec_tree){\
	cspec_tree_add_it(cspec_tree, __stringize(describe_key), __stringize(name), __append(CSPEC_it_, __LINE__));\
}

#define after(describe_key, it_key, block)\
void __append(CSPEC_after_, __LINE__()){\
	block\
}\
void __append(CSPEC_handle_after_, __LINE__)(cspec_tree_t * cspec_tree){\
	cspec_tree_add_after(cspec_tree, __stringize(describe_key), __stringize(it_key), __append(CSPEC_after_, __LINE__));\
}

#endif

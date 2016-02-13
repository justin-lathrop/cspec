#ifndef __CSPEC_TREE__
#define __CSPEC_TREE__

/*
 * CSPec_tree is the datastructure that contains 
 * every possible flow within the CSpec tests.  
 * Just like RSpec it has the following behavioral 
 * descriptors/tests:
 * 
 * Directive types:
 *     describe: Provides a 'test suite' or item that 
 *         is going to be behaviorly tested.  It contains 
 *         all the it's, before's and after's that exist 
 *         in the test suite, its the basis/starting 
 *         point of the CSpec tests.
 *     it: Is the actual test, the code given is just 
 *         run.  There is not a way to specify order if 
 *         multiple it's exist.  It must be attached to 
 *         a corresponding describe.
 *     before: Depending on the node this is attached 
 *         to it will run first, if other before's 
 *         exist then there is no gurantee of order.  
 *         This can be added anywhere in the tree.
 *     after: Depending on the node this is attached 
 *         to it will run last, if other after's 
 *         exist then there is no gurantee of order.  
 *         This can be added anywhere in the tree.
 * 
 * As an example of what the cspec_tree will look like 
 * in a typical case the example below follows.
 * 
 * The test:

describe("test 1")
before("test 1", { ... })
after("test 1", { ... })
it("test 1", "tests something", { ... })
it("test 1", "test another thing", { ... })

 * The evaluated cspec_tree:

                       * (describe("test 1"))
                    / | | \
          (before) *  * *  * (after)
(it("tests something")) (it("test another thing"))
 */

#include "cspec_list.h"

typedef struct cspec_tree_t{
	cspec_list_t * describes;
	cspec_list_t * befores;
	cspec_list_t * afters;
} cspec_tree_t;

typedef void (* cspec_block)();

typedef enum{
	DESCRIBE,
	IT,
	BEFORE,
	AFTER
} cspec_node_type;

typedef struct cspec_node_t{
	char * key;
	cspec_node_type type;
	cspec_list_t * befores;
	cspec_list_t * afters;
} cspec_node_t;

typedef struct cspec_node_describe_t{
	cspec_node_t * base;
	cspec_list_t * its;
} cspec_node_describe_t;

typedef struct cspec_node_code_block_t{
	cspec_node_t * base;
	cspec_block code_block;
} cspec_node_code_block_t;

cspec_tree_t * cspec_tree_initialize();
void cspec_tree_add_describe(cspec_tree_t *, char *);
void cspec_tree_add_before(cspec_tree_t *, char *, char *, cspec_block);
void cspec_tree_add_it(cspec_tree_t *, char *, char *, cspec_block);
void cspec_tree_add_after(cspec_tree_t *, char *, char *, cspec_block);
void cspec_tree_free(cspec_tree_t *);

#endif

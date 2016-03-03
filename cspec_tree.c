#include <stdlib.h>
#include <string.h>
#include "cspec_tree.h"

#include <stdio.h>

void free_base(cspec_node_t * base);

void free_befores(cspec_list_t * befores){
	int size = cspec_list_size(befores);
	
	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * before = (cspec_node_code_block_t *)cspec_list_get(befores, i);
		free_base(before->base);
		free(before->base);
		free(before);
	}
}

void free_afters(cspec_list_t * afters){
	int size = cspec_list_size(afters);
	
	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * after = (cspec_node_code_block_t *)cspec_list_get(afters, i);
		free_base(after->base);
		free(after->base);
		free(after);
	}
}

void free_its(cspec_list_t * its){
	int size = cspec_list_size(its);
	
	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * it = (cspec_node_code_block_t *)cspec_list_get(its, i);
		free_base(it->base);
		free(it->base);
		free(it);
	}
}

void free_describes(cspec_list_t * describes){
	int size = cspec_list_size(describes);
	
	for(int i = 0; i < size; i++){
		cspec_node_describe_t * describe = (cspec_node_describe_t *)cspec_list_get(describes, i);
		free_base(describe->base);
		free(describe->base);
		free_its(describe->its);
		free(describe->its);
		free(describe);
	}
}

void free_base(cspec_node_t * base){
    if(!base){ free(base->key); }

	free_befores(base->befores);
	free(base->befores);

	free_afters(base->afters);
	free(base->afters);
}

void * find_node(cspec_list_t * list, char * key){
	int size = cspec_list_size(list);
    cspec_node_describe_t * node;

	for(int i = 0; i < size; i++){
		node = (cspec_node_describe_t *)cspec_list_get(list, i);

		if(strcmp(node->base->key, key) == 0){
			return((void *)node);
		}
	}

	return(NULL);
}

cspec_node_t * initialize_node(cspec_node_type type, char * key){
	cspec_node_t * node = (cspec_node_t *)malloc(sizeof(cspec_node_t));
	node->type = type;
	node->key = key;
	node->befores = cspec_list_initialize();
	node->afters = cspec_list_initialize();

	return(node);
}

cspec_tree_t * cspec_tree_initialize(){
	cspec_tree_t * tree = (cspec_tree_t *)malloc(sizeof(cspec_tree_t));
	tree->describes = cspec_list_initialize();
	tree->befores = cspec_list_initialize();
	tree->afters = cspec_list_initialize();

	return(tree);
}

void cspec_tree_add_describe(cspec_tree_t * tree, char * key){
	if(find_node(tree->describes, key) != NULL){ return; }

	cspec_node_describe_t * describe = (cspec_node_describe_t *)malloc(sizeof(cspec_node_describe_t));
	describe->base = initialize_node(DESCRIBE, key);
	describe->its = cspec_list_initialize();

	cspec_list_add(tree->describes, (void *)describe);
}

void cspec_tree_add_it(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block_it code_block){
	cspec_node_describe_t * describe = find_node(tree->describes, describe_key);
    if(describe == NULL){ cspec_tree_add_describe(tree, describe_key); }

    cspec_node_code_block_t * it = find_node(describe->its, it_key);
    if(it != NULL){
        it->code_block_it = code_block;
    }else{
        cspec_node_code_block_t * it = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
        it->base = initialize_node(IT, it_key);
        it->code_block_it = code_block;
        
        cspec_list_add(describe->its, (void *)it);
    }
}

void cspec_tree_add_before(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block code_block){
	cspec_node_code_block_t * before = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
	before->base = initialize_node(BEFORE, NULL);
	before->code_block = code_block;

	if(strlen(describe_key) == 0){ // ROOT
		cspec_list_add(tree->befores, (void *)before);
	}else{
		cspec_tree_add_describe(tree, describe_key);
		cspec_node_describe_t * describe = find_node(tree->describes, describe_key);

		if(strlen(it_key) == 0){ // DESCRIBE
			cspec_list_add(describe->base->befores, (void *)before);
		}else{ // IT
            cspec_tree_add_it(tree, describe_key, it_key, NULL);
			cspec_node_code_block_t * it = find_node(describe->its, it_key);
            
            cspec_list_add(it->base->befores, (void *)before);
		}
	}
}

void cspec_tree_add_after(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block code_block){
	cspec_node_code_block_t * after = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
	after->base = initialize_node(AFTER, NULL);
	after->code_block = code_block;

	if(strlen(describe_key) == 0){ // ROOT
		cspec_list_add(tree->afters, (void *)after);
	}else{
		cspec_tree_add_describe(tree, describe_key);
		cspec_node_describe_t * describe = find_node(tree->describes, describe_key);

		if(strlen(it_key) == 0){ // DESCRIBE
			cspec_list_add(describe->base->afters, (void *)after);
		}else{ // IT
            cspec_tree_add_it(tree, describe_key, it_key, NULL);
			cspec_node_code_block_t * it = find_node(describe->its, it_key);

            cspec_list_add(it->base->afters, (void *)after);
		}
	}
}

void cspec_tree_free(cspec_tree_t * tree){
	free_befores(tree->befores);
	free(tree->befores);

	free_afters(tree->afters);
	free(tree->afters);

	free_describes(tree->describes);
	free(tree->describes);

	free(tree);
}

void cspec_tree_iterate(cspec_tree_t * tree){
    int describe_size,
        it_size,
        before_size,
        after_size,
        i, j, k = 0;
    
    // Befores
    before_size = cspec_list_size(tree->befores);
    for(i = 0; i < before_size; i++){
        cspec_node_code_block_t * before = (cspec_node_code_block_t *)cspec_list_get(tree->befores, i);
        before->code_block();
        
    }
    
    // Describes
    describe_size = cspec_list_size(tree->describes);
    for(i = 0; i < describe_size; i++){
        cspec_node_describe_t * describe = (cspec_node_describe_t *)cspec_list_get(tree->describes, i);
        
        // Befores
        before_size = cspec_list_size(describe->base->befores);
        for(j = 0; j < before_size; j++){
            cspec_node_code_block_t * before = (cspec_node_code_block_t *)cspec_list_get(describe->base->befores, j);
            before->code_block();
        }
        
        printf("DESCRIBE[ %s ]\n", describe->base->key);
        
        // Its
        it_size = cspec_list_size(describe->its);
        for(j = 0; j < it_size; j++){
            cspec_node_code_block_t * it = (cspec_node_code_block_t *)cspec_list_get(describe->its, j);
            
            // Befores
            before_size = cspec_list_size(it->base->befores);
            for(k = 0; k < before_size; k++){
                cspec_node_code_block_t * before = (cspec_node_code_block_t *)cspec_list_get(it->base->befores, k);
                before->code_block();
            }
            
            cspec_list_t * failures = cspec_list_initialize();
            it->code_block_it(failures);
            printf("\tIT[ %s ]\n", it->base->key);
            
            int failures_size = cspec_list_size(failures);
            if(failures_size > 0){
                for(int l = 0; l < failures_size; l++){
                    printf("\t\tFAIL[ %s ]\n", (char *)cspec_list_get(failures, l));
                }
                
                cspec_list_free(failures);
            }
                
            // Afters
            after_size = cspec_list_size(it->base->afters);
            for(k = 0; k < after_size; k++){
                cspec_node_code_block_t * after = (cspec_node_code_block_t *)cspec_list_get(it->base->afters, k);
                after->code_block();
            }
        }
        
        // Afters
        after_size = cspec_list_size(describe->base->afters);
        for(j = 0; j < after_size; j++){
            cspec_node_code_block_t * after = (cspec_node_code_block_t *)cspec_list_get(describe->base->afters, j);
            after->code_block();
        }
    }
    
    // Afters
    after_size = cspec_list_size(tree->afters);
    for(i = 0; i < after_size; i++){
        cspec_node_code_block_t * after = (cspec_node_code_block_t *)cspec_list_get(tree->afters, i);
        after->code_block();
    }
}

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
		free(before->code_block);
		free(before);
	}
}

void free_afters(cspec_list_t * afters){
	int size = cspec_list_size(afters);
	
	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * after = (cspec_node_code_block_t *)cspec_list_get(afters, i);
		free_base(after->base);
		free(after->base);
		free(after->code_block);
		free(after);
	}
}

void free_its(cspec_list_t * its){
	int size = cspec_list_size(its);
	
	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * it = (cspec_node_code_block_t *)cspec_list_get(its, i);
		free_base(it->base);
		free(it->base);
		free(it->code_block);
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
	free(base->key);

	free_befores(base->befores);
	free(base->befores);

	free_afters(base->afters);
	free(base->afters);
}

cspec_node_describe_t * find_describe(cspec_list_t * list, char * key){
	int size = cspec_list_size(list);

	for(int i = 0; i < size; i++){
		cspec_node_describe_t * describe = (cspec_node_describe_t *)cspec_list_get(list, i);

		if(strcmp(describe->base->key, key)){
			return(describe);
		}
	}

	return(NULL);
}

cspec_node_code_block_t * find_it(cspec_list_t * list, char * key){
	int size = cspec_list_size(list);

	for(int i = 0; i < size; i++){
		cspec_node_code_block_t * it = (cspec_node_code_block_t *)cspec_list_get(list, i);

		if(strcmp(it->base->key, key)){
			return(it);
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
	printf("ADD DESCRIBE\n");
	if(find_describe(tree->describes, key) != NULL){ return; }

	cspec_node_describe_t * describe = (cspec_node_describe_t *)malloc(sizeof(cspec_node_describe_t));
	describe->base = initialize_node(DESCRIBE, key);
	describe->its = cspec_list_initialize();

	cspec_list_add(tree->describes, (void *)describe);
}

void cspec_tree_add_it(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block code_block){
	printf("ADD IT\n");
	cspec_tree_add_describe(tree, describe_key);
	cspec_node_describe_t * describe = find_describe(tree->describes, describe_key);

	cspec_node_code_block_t * it = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
	it->base = initialize_node(IT, it_key);
	it->code_block = code_block;

	cspec_list_add(describe->its, (void *)it);
}

void cspec_tree_add_before(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block code_block){
	printf("ADD BEFORE\n");
	cspec_node_code_block_t * before = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
	before->base = initialize_node(BEFORE, NULL);
	before->code_block = code_block;

	if(describe_key == NULL){ // ROOT
		cspec_list_add(tree->befores, (void *)before);
	}else{
		cspec_tree_add_describe(tree, describe_key);
		cspec_node_describe_t * describe = find_describe(tree->describes, describe_key);

		if(it_key == NULL){ // DESCRIBE
			cspec_list_add(describe->base->befores, (void *)before);
		}else{ // IT
			cspec_node_code_block_t * it = find_it(describe->its, it_key);

			if(it != NULL){
				cspec_list_add(it->base->befores, (void *)before);
			}else{
				// Ignore the before and don't add it...
			}
		}
	}
}

void cspec_tree_add_after(cspec_tree_t * tree, char * describe_key, char * it_key, cspec_block code_block){
	printf("CSPEC_AFTER\n");
	cspec_node_code_block_t * after = (cspec_node_code_block_t *)malloc(sizeof(cspec_node_code_block_t));
	after->base = initialize_node(AFTER, NULL);
	after->code_block = code_block;

	if(describe_key == NULL){ // ROOT
		printf("0\n");
		cspec_list_add(tree->afters, (void *)after);
		printf("1\n");
	}else{
		printf("2\n");
		cspec_tree_add_describe(tree, describe_key);
		printf("3\n");
		cspec_node_describe_t * describe = find_describe(tree->describes, describe_key);
		printf("4\n");

		if(it_key == NULL){ // DESCRIBE
			cspec_list_add(describe->base->afters, (void *)after);
		}else{ // IT
			cspec_node_code_block_t * it = find_it(describe->its, it_key);

			if(it != NULL){
				cspec_list_add(it->base->afters, (void *)after);
			}else{
				// Ignore the after and don't add it...
			}
		}
	}
}

void cspec_tree_free(cspec_tree_t * tree){
	free_befores(tree->befores);
	cspec_list_free(tree->befores);
	free(tree->befores);

	free_afters(tree->afters);
	cspec_list_free(tree->afters);
	free(tree->afters);

	free_describes(tree->describes);
	cspec_list_free(tree->describes);
	free(tree->describes);

	free(tree);
}

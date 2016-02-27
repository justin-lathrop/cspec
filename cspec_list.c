#include <stdlib.h>
#include "cspec_list.h"

cspec_list_item_t * get_bottom(cspec_list_item_t * start){
	cspec_list_item_t * next = start;
	while(next->next){ next = next->next; }
	return(next);
}

cspec_list_t * cspec_list_initialize(){
	cspec_list_t * list = (cspec_list_t *)malloc(sizeof(cspec_list_t));
	list->head = NULL;

	return(list);
}

void cspec_list_add(cspec_list_t * list, void * data){
	cspec_list_item_t * item = cspec_list_item_initialize();
	item->data = data;

	if(list->head == NULL){
		list->head = item;
	}else{
		cspec_list_item_t * bottom = get_bottom(list->head);
		bottom->next = item;
	}
}

void cspec_list_free(cspec_list_t * list){
	cspec_list_item_t * next = list->head;

	while(next->next){
		if(!next->next->next){
			cspec_list_item_free(next->next);
			next = list->head;
		}else{
			next = next->next;
		}
	}

	cspec_list_item_free(list->head);
	free(list);
}

int cspec_list_size(cspec_list_t * list){
	cspec_list_item_t * next;
	int size = 0;
	
	next = list->head;
	while(next){
		size++;
		next = next->next;
	}

	return(size);
}

void * cspec_list_get(cspec_list_t * list, int index){
	cspec_list_item_t * next;
	int current_index = 0;

	next = list->head;
	while(next){
		if(current_index == index){
			return(next->data);
		}else{
			current_index++;
			next = next->next;
		}
	}

	return(NULL);
}

cspec_list_item_t * cspec_list_item_initialize(){
	cspec_list_item_t * item = (cspec_list_item_t *)malloc(sizeof(cspec_list_item_t));
	item->next = NULL;
	item->data = NULL;

	return(item);
}

void cspec_list_item_free(cspec_list_item_t * item){
	free(item->next);
}

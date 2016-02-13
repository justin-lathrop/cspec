#ifndef __CSPEC_LIST__
#define __CSPEC_LIST__

typedef struct cspec_list_item_t{
	struct cspec_list_item_t * next;
	void * data;
} cspec_list_item_t;

typedef struct cspec_list_t{
	cspec_list_item_t * head;
} cspec_list_t;

cspec_list_t * cspec_list_initialize();
void cspec_list_add(cspec_list_t *, cspec_list_item_t *);
void cspec_list_free(cspec_list_t *);

int cspec_list_size(cspec_list_t *);
cspec_list_item_t * cspec_list_get(cspec_list_t *, int);

cspec_list_item_t * cspec_list_item_initialize();
void cspec_list_item_free(cspec_list_item_t *);

#endif

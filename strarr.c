#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "common.h"
#include "strarr.h"

struct _StrArr {
	unsigned int length, itemsize;
	unsigned int count;
	char **items;
};


StrArr *strarr_init(StrArr *arr, char **items, unsigned int length, unsigned int itemsize){
	assert(arr != NULL && items != NULL);
	arr->items = items;
	arr->count = 0;
	arr->length = length;
	arr->itemsize = itemsize;
	return arr;
}
StrArr *strarr_new(unsigned int length, unsigned int itemsize){
	StrArr *arr = malloc(sizeof(StrArr));
	char **items = malloc(sizeof(char *) * length);
	for(unsigned int i = 0; i < length; i++)
		items[i] = malloc(itemsize);

	strarr_init(arr, items, length, itemsize);
	return arr;
}

void strarr_free(StrArr *arr){
	for(unsigned int i = 0; i < arr->length; i++)
		free(arr->items[i]);
	free(arr->items);
	free(arr);
}

int strarr_item_count(StrArr *arr){
	return arr->count;
}
int strarr_is_full(StrArr *arr){
	return arr->count == arr->length;
}

int strarr_is_empty(StrArr *arr){
	return arr->count == 0;
}

int strarr_has_index(StrArr *arr, unsigned int index){
	return index < arr->count;
}

void strarr_clear(StrArr *arr){
	arr->count = 0;
}

int strarr_append(StrArr *arr, char *str){
	if(strempty(str))
		return FAIL;
	if(strlen(str) + 1 > arr->itemsize)
		return FAIL;
	
	if(strarr_is_full(arr))
		return FAIL;
	if(strarr_fetch(arr, str) != FAIL)
		return FAIL;


	strcpy(arr->items[arr->count], str);
	arr->count++;

	return SUCCESS;
}

int strarr_remove(StrArr *arr, unsigned int index){

	if(strarr_has_index(arr, index)){
		unsigned int last_index = arr->count - 1;
		if(index != last_index)
			strcpy(arr->items[index], arr->items[last_index]);
		arr->count--;
	}
	return FAIL;
}

int strarr_fetch(StrArr *arr, char *str){
	unsigned int count = arr->count;

	for(unsigned int i = 0; i < count; i++){
		if(strcmp(arr->items[i], str) == 0)
			return i;
	}
	return FAIL;
}

char *strarr_get(StrArr *arr, unsigned int index){
	if(strarr_has_index(arr, index))
		return arr->items[index];
	return NULL;
}

void strarr_print(StrArr *arr){
	assert(arr != NULL);

	printf("{");
	for(unsigned int i = 0; i < (arr->count - 1); i++){
		printf("´%s´, ", arr->items[i]);
	}
	printf("´%s´}\n", arr->items[arr->count - 1]);
}

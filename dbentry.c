// TODO: suporte para UTF-8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dbentry.h"
#include "common.h"

int db_entry_tag_count(DbEntry *entry){
	return entry->_private.tag_count;
}

int db_entry_tag_list_is_full(DbEntry *entry){
	return entry->_private.tag_count == TAG_LIST_LENGTH;
}

int db_entry_tag_list_is_empty(DbEntry *entry){
	return entry->_private.tag_count == 0;
}

int db_entry_tag_list_has_index(DbEntry *entry, unsigned int index){
	return entry->_private.tag_count > index;
}

void db_entry_clear_tags(DbEntry *entry){
	entry->_private.tag_count = 0;
}

int db_entry_append_tag(DbEntry *entry, const char *tag){
	if(strempty(tag))
		return TAG_LIST_INVALID_TAG;
	if(strlen(tag) + 1 > TAG_NAME_SIZE)
		return TAG_LIST_INVALID_TAG;

	int tag_count = entry->_private.tag_count;

	if(db_entry_tag_list_is_full(entry))
		return TAG_LIST_IS_FULL;
	if(db_entry_fetch_tag(entry, tag) != FAIL)
		return TAG_ALREADY_EXISTS;

	strcpy(entry->_private.tags[tag_count], tag);
	entry->_private.tag_count++;

	return SUCCESS;
}

int db_entry_remove_tag(DbEntry *entry, unsigned int index){
	if(db_entry_tag_list_has_index(entry, index)){
		unsigned int last_index = entry->_private.tag_count - 1;

		if(index != last_index)
			strcpy(entry->_private.tags[index], entry->_private.tags[last_index]);

		entry->_private.tag_count--;
		return SUCCESS;
	}

	return FAIL;
}


char *db_entry_get_tag(DbEntry *entry, unsigned int index){
	if(db_entry_tag_list_has_index(entry, index))
		return entry->_private.tags[index];

	return NULL;
}

int db_entry_fetch_tag(DbEntry *entry, const char *tag){
	int tag_count = entry->_private.tag_count;
	
	if(tag_count == 0)
		return FAIL;

	for(int i = 0; i < tag_count; i++){
		if(strcmp(entry->_private.tags[i], tag) == 0)
			return i;
	}
	
	// not found
	return FAIL;
}


DbEntry *db_entry_new(void){

	// a lista de tags fica vazia
	DbEntry *new = calloc(1, sizeof(DbEntry));

	return new;

}

void debug_db_entry_print_tags(DbEntry *entry){
	int tag_count = db_entry_tag_count(entry);

	puts("{");
	for(int i = 0; i < tag_count; i++){
		printf("  [%u]%s'\n", i, db_entry_get_tag(entry, i));
	}
	puts("}");
}

// TODO: suporte para UTF-8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // getuid
#include <pwd.h> //getpwuid
#include <sys/stat.h> // mkdir
#include <stdarg.h>
#include "db.h"

static struct {
	char config_path[PATH_MAX];
	char db_path[PATH_MAX];
	char cache_path[PATH_MAX];
} init_data;

#define strempty(str) ((str)[0]==0)
#define path_join(base_size, base, ...) __path_join(base_size, base, __VA_ARGS__, NULL)
#define path_join_to(dest_size, dest, base, ...) __path_join((dest_size), strcpy((dest), (base)), __VA_ARGS__, NULL)
#define open_file_in_db_path(filename, mode) open_file_in_path(init_data.db_path, (filename), (mode))
#define open_file_in_cache_path(filename, mode) open_file_in_path(init_data.cache_path, (filename), (mode))

static void write_file_to_another(FILE *dest, FILE *src);
static FILE *open_file_in_path(const char *path, const char *filename, const char *mode);
static int db_entry_write_struct(DbEntry *entry);
static int db_entry_write_content(DbEntry *entry);
static int db_entry_update_file_struct(DbEntry * entry);
static char *__path_join(size_t base_size, char *base, ...);

static char error_string[128];

static char *__path_join(size_t base_size, char *base, ...){
	va_list args;
	va_start(args, base);

	char *p;
	char *base_adv = base;
	int avaliable = base_size - (strlen(base));

	while((p = va_arg(args, char*)) != NULL){

		size_t p_len = strlen(p);
		size_t adv_len = strlen(base_adv);

		avaliable -= p_len;

		if(base_adv[adv_len - 1] == '/'){
			if(avaliable <= 0)
				break;

			strcat(base_adv, p);
			base_adv += p_len;
		} else {
			if(--avaliable <= 0)
				break;

			base_adv[adv_len] = '/';
			base_adv[adv_len + 1] = 0;

			strcat(base_adv, p);
			base_adv += p_len + 1;
		}
		
	}

	va_end(args);
	return base;
}

void db_init(void){

	struct passwd *pw = getpwuid(getuid());
	char *home = pw->pw_dir;

	char *config_path = path_join_to(PATH_MAX, init_data.config_path,
		home, ".thingdb");
	char *db_path = path_join_to(PATH_MAX, init_data.db_path,
		config_path, "db");
	char *cache_path = path_join_to(PATH_MAX, init_data.cache_path,
		home, ".cache", "thingdb");

	puts(config_path);
	puts(db_path);
	puts(cache_path);

	mkdir(config_path, S_IRWXU);
	mkdir(db_path, S_IRWXU);
	mkdir(cache_path, S_IRWXU);

}

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

int db_entry_written(DbEntry *entry){
	FILE *fp = open_file_in_db_path(entry->db_filename, "r");
	if(fp == NULL)
		return 0;
	fclose(fp);
	return 1;
}

static void write_file_to_another(FILE *dest, FILE *src){

	char buffer[BUFSIZ];
	size_t nbytes;

	while((nbytes = fread(buffer, 1, sizeof(buffer), src)))
		fwrite(buffer, nbytes, 1, dest);

}

static FILE *open_file_in_path(const char *path, const char *filename, const char *mode){
	char filepath[PATH_MAX];
	strcpy(filepath, path);
	path_join(PATH_MAX, filepath, filename);

	FILE *fp = fopen(filepath, mode);

	return fp;
}

static int db_entry_write_struct(DbEntry *entry){

	FILE *fp = open_file_in_db_path(entry->db_filename, "w");
	
	if(fp == NULL)
		return FAIL;

	fwrite((char*)entry, sizeof(DbEntry), 1, fp);

	fclose(fp);

	return SUCCESS;
}

static int db_entry_write_content(DbEntry *entry){

	FILE *db_file = open_file_in_db_path(entry->db_filename, "a");
	if(db_file == NULL)
		return CANNOT_OPEN_DB_FILE;

	FILE *content_file = fopen(entry->content_filepath, "r");
	if(content_file == NULL){
		fclose(db_file);
		return CANNOT_OPEN_CONTENT_FILE;
	}

	write_file_to_another(db_file, content_file);

	fclose(db_file);
	fclose(content_file);

	return SUCCESS;

}

static int db_entry_cache_content(DbEntry *entry){
	if(!db_entry_written(entry))
		return NOT_WRITTEN;

	FILE *db_file = open_file_in_db_path(entry->db_filename, "r");
	if(db_file == NULL)
		return CANNOT_OPEN_DB_FILE;

	FILE *cache_file = open_file_in_cache_path(entry->db_filename, "w");
	if(cache_file == NULL){
		fclose(db_file);
		return CANNOT_OPEN_CACHE_FILE;
	}

	fseek(db_file, sizeof(DbEntry), SEEK_SET);

	write_file_to_another(cache_file, db_file);

	fclose(db_file);
	fclose(cache_file);

	return SUCCESS;
}

static int db_entry_update_file_struct(DbEntry *entry){

	int ret;
	if((ret = db_entry_cache_content(entry)) != SUCCESS)
		return ret;

	if((ret = db_entry_write_struct(entry)) != SUCCESS)
		return ret;

	FILE *dbfp = open_file_in_db_path(entry->db_filename, "a");
	if(dbfp == NULL)
		return CANNOT_OPEN_DB_FILE;

	FILE *cachefp = open_file_in_cache_path(entry->db_filename, "r");
	if(cachefp == NULL){
		fclose(dbfp);
		return CANNOT_OPEN_CACHE_FILE;
	}

	write_file_to_another(dbfp, cachefp);

	fclose(dbfp);
	fclose(cachefp);

	return SUCCESS;
}

int db_entry_update(DbEntry *entry, int only_struct){

	if(only_struct)
		return db_entry_update_file_struct(entry);

	int ret;

	if((ret = db_entry_write_struct(entry)) != SUCCESS)
		return ret;
	if((ret = db_entry_write_content(entry)) != SUCCESS)
		return ret;

	return SUCCESS;

}

int db_entry_read(char *filename, DbEntry **ret){
	
	FILE *fp = open_file_in_db_path(filename, "rb");

	if(fp == NULL)
		return CANNOT_OPEN_DB_FILE; 

	*ret = db_entry_new();

	fread(*ret, sizeof(DbEntry), 1, fp);

	fclose(fp);
	
	return SUCCESS;
}


char *db_entry_get_error(int error){
	
	switch(error){
		case FAIL:
			strcpy(error_string, "Failed");
			break;
		case SUCCESS:
			strcpy(error_string, "Success");
			break;
		case TAG_LIST_INVALID_TAG:
			strcpy(error_string, "The given tag is invalid");
			break;
		case TAG_LIST_IS_FULL:
			strcpy(error_string, "The tag list is full");
			break;
		case TAG_ALREADY_EXISTS:
			strcpy(error_string, "The given tag already exists in the tag list");
			break;
		case CANNOT_OPEN_DB_FILE:
			strcpy(error_string, "Cannot open the file of the given entry");
			break;
		case CANNOT_OPEN_CONTENT_FILE:
			strcpy(error_string, "Cannot open the file of the given entry's content");
			break;
		case CANNOT_OPEN_CACHE_FILE:
			strcpy(error_string, "Cannot open the cache file of the given entry");
			break;
		case MISSING_DB_FILENAME:
			strcpy(error_string, "The given entry is missing `db_filename` field");
			break;
		case MISSING_CONTENT_FILEPATH:
			strcpy(error_string, "The given entry is missing `content_filepath` field");
			break;
		case NOT_WRITTEN:
			strcpy(error_string, "The given entry is not written to a file inside de database folder");
			break;
		default:
			return NULL;
	}

	return error_string;

}

// TODO: suporte para UTF-8
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // getuid
#include <pwd.h> //getpwuid
#include <sys/stat.h> // mkdir
#include <dirent.h>
#include <errno.h>
#include "db.h"
#include "dbentry.h"
#include "common.h"
#include "strarr.h"

#define open_file_in_db_path(filename, mode) 	open_file_in_path(\
	g_database.db_path, (filename), (mode))
#define open_file_in_cache_path(filename, mode) open_file_in_path(\
	g_database.cache_path, (filename), (mode))

static struct {
	char config_path[PATH_MAX];
	char db_path[PATH_MAX];
	char cache_path[PATH_MAX];
	StrArr *filenames;
} g_database;

static int db_write_entry_struct(DbEntry *entry);
static int db_write_entry_content(DbEntry *entry);
static int db_update_entry_file_struct(DbEntry * entry);
static int db_cache_entry_content(DbEntry *entry);
static int db_update(void);

void db_finish(void){
	strarr_free(g_database.filenames);
}

int db_init(const char *dbname){

	struct passwd *pw = getpwuid(getuid());
	char *home = pw->pw_dir;

	char *config_path = path_join_auto(g_database.config_path,
		home, ".thingdb");
	char *db_path = path_join_auto(g_database.db_path,
		config_path, dbname);
	char *cache_path = path_join_auto(g_database.cache_path,
		home, ".cache", "thingdb");

	if(mkdir(config_path, S_IRWXU) != 0 && errno != EEXIST)
		return CANNOT_CREATE_CONFIG_DIR;
	if(mkdir(db_path, S_IRWXU) != 0 && errno != EEXIST)
		return CANNOT_CREATE_DB_DIR;
	if(mkdir(cache_path, S_IRWXU) != 0 && errno != EEXIST)
		return CANNOT_CREATE_CACHE_DIR;

	g_database.filenames = strarr_new(MAX_DB_FILES, NAME_MAX);
	db_update();

	strarr_print(g_database.filenames);

	return SUCCESS;
}

static int db_update(void){
	DIR *d;
	struct dirent *ent;

	if((d = opendir(g_database.db_path)) == NULL)
		return CANNOT_OPEN_DB_DIR;

	while((ent = readdir(d)) != NULL){
		if(strcmp(ent->d_name, "..") == 0 ||
			strcmp(ent->d_name, ".") == 0)
			continue;

		strarr_append(g_database.filenames, ent->d_name);
	}

	closedir(d);

	return SUCCESS;
}

int db_check_entry_written(DbEntry *entry){
	return strarr_fetch(g_database.filenames, entry->db_filename) != FAIL;
}

static int db_write_entry_struct(DbEntry *entry){

	FILE *fp = open_file_in_db_path(entry->db_filename, "w");
	
	if(fp == NULL)
		return CANNOT_OPEN_DB_FILE;

	fwrite((char*)entry, sizeof(DbEntry), 1, fp);

	fclose(fp);

	return SUCCESS;
}

static int db_write_entry_content(DbEntry *entry){

	if(path_is_dir(entry->content_filepath))
		return CANNOT_OPEN_CONTENT_FILE;

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

static int db_cache_entry_content(DbEntry *entry){
	if(!db_check_entry_written(entry))
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

static int db_update_entry_file_struct(DbEntry *entry){

	int ret;
	if((ret = db_cache_entry_content(entry)) != SUCCESS)
		return ret;

	if((ret = db_write_entry_struct(entry)) != SUCCESS)
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

int db_update_entry(DbEntry *entry, int only_struct){
	int ret;

	if(only_struct){
		if((ret = db_update_entry_file_struct(entry)) != SUCCESS)
			return ret;
	} else {
		if((ret = db_write_entry_struct(entry)) != SUCCESS)
			return ret;
		db_write_entry_content(entry);
	}
	
	strarr_append(g_database.filenames, entry->db_filename);
	return SUCCESS;
}

int db_get_entry_index(DbEntry *entry){
	return strarr_fetch(g_database.filenames, entry->db_filename);
}

// TODO
int db_delete_entry(unsigned i){
	NOT_IMPL;
}

DbEntry *db_load_entry(unsigned i){
	NOT_IMPL;
}

int db_n_entries(void){
	return strarr_item_count(g_database.filenames);
}

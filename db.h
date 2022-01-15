#pragma once

#include <linux/limits.h>
#include <string.h>

#define TAG_LIST_LENGTH 16
#define TAG_NAME_SIZE 26
#define ENTRY_NAME_SIZE 26
#define ENTRY_DESC_SIZE 256

typedef struct _DbEntry DbEntry;

struct _DbEntry {
	struct {
		char tags[TAG_LIST_LENGTH][TAG_NAME_SIZE];
		unsigned int tag_count;
	} _private;

	char db_filename[NAME_MAX];
	char content_filepath[NAME_MAX];
	char name[ENTRY_NAME_SIZE];
	char description[ENTRY_DESC_SIZE];
};

enum {
	FAIL = -1,
	SUCCESS = 0,

	TAG_LIST_INVALID_TAG,
	TAG_LIST_IS_FULL,
	TAG_ALREADY_EXISTS,

	CANNOT_OPEN_DB_FILE,
	CANNOT_OPEN_CONTENT_FILE,
	CANNOT_OPEN_CACHE_FILE,

	MISSING_DB_FILENAME,
	MISSING_CONTENT_FILEPATH,

	NOT_WRITTEN,
};

#define _db_entry_set_string_field(entry, field, value) do {\
	if(!(strlen(value) + 1 > sizeof((entry)->field)))\
		strcpy((entry)->field, value);\
} while(0)

#define db_entry_set_db_filename(entry, value)		_db_entry_set_string_field(entry, db_filename, value)
#define db_entry_set_content_filepath(entry, value) _db_entry_set_string_field(entry, content_filepath, value)
#define db_entry_set_src_filepath(entry, value)		_db_entry_set_string_field(entry, src_filepath, value)
#define db_entry_set_name(entry, value)				_db_entry_set_string_field(entry, name, value)
#define db_entry_set_description(entry, value)		_db_entry_set_string_field(entry, description, value)

void db_init(void);

DbEntry *db_entry_new(void);
// getters
int db_entry_tag_count(DbEntry *entry);
int db_entry_tag_list_is_full(DbEntry *entry);
int db_entry_tag_list_is_empty(DbEntry *entry);
int db_entry_tag_list_has_index(DbEntry *entry, unsigned int index);
// esta operações alteram a DbEntry de alguma maneira
// neste caso a lista de tags
void db_entry_clear_tags(DbEntry *entry);
int db_entry_append_tag(DbEntry *entry, const char *tag);
int db_entry_remove_tag(DbEntry *entry, unsigned int index);
// --
char *db_entry_get_tag(DbEntry *entry, unsigned int index);
int db_entry_fetch_tag(DbEntry *entry, const char *tag);
// ficheiros
int db_entry_update(DbEntry *entry, int only_struct);
int db_entry_read(char *filename, DbEntry **ret);
int db_entry_written(DbEntry *entry);

void debug_db_entry_print_tags(DbEntry *entry);
char *db_entry_get_error(int error);

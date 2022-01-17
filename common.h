#pragma once
#include <string.h>
#include <stdio.h>
#include <limits.h>

typedef enum {
	_DUMMY = INT_MIN,
	TAG_LIST_INVALID_TAG,
	TAG_LIST_IS_FULL,
	TAG_ALREADY_EXISTS,
	CANNOT_OPEN_DB_FILE,
	CANNOT_OPEN_CONTENT_FILE,
	CANNOT_OPEN_CACHE_FILE,
	NOT_WRITTEN,
	CANNOT_OPEN_DB_DIR,
	CANNOT_CREATE_CONFIG_DIR,
	CANNOT_CREATE_DB_DIR,
	CANNOT_CREATE_CACHE_DIR,
	FAIL = -1,
	SUCCESS = 0,
} ERRORS_ENUM;

#define strempty(str) ((str)[0]==0)
#define path_join_auto(dest, base, ...) path_join(sizeof(dest), strncpy(dest, base, sizeof(dest)), __VA_ARGS__, NULL)

#define NOT_IMPL __not_impl(__func__)

char *path_join(size_t base_size, char *base, ...);

int path_is_dir(const char *pathname);

void write_file_to_another(FILE *dest, FILE *src);
FILE *open_file_in_path(const char *path, const char *filename, const char *mode);

char *get_error(ERRORS_ENUM error);

void __not_impl(const char *func);


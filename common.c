#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <signal.h> // kill()
#include <unistd.h> // getpid()
#include "common.h"

static char error_string[128];

void __not_impl(const char *func){

	printf("\033[1mNOT IMPLEMENTED:\033[0m %s()\n", func);
	kill(getpid(), SIGTERM);

}

char *path_join(size_t base_size, char *base, ...){
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

int path_is_dir(const char *pathname){
	struct stat st;
	stat(pathname, &st);

	return S_ISDIR(st.st_mode);
}

void write_file_to_another(FILE *dest, FILE *src){

	char buffer[BUFSIZ];
	size_t nbytes;

	while((nbytes = fread(buffer, 1, sizeof(buffer), src)))
		fwrite(buffer, nbytes, 1, dest);

}

FILE *open_file_in_path(const char *path, const char *filename, const char *mode){
	char filepath[PATH_MAX];
	path_join_auto(filepath, path, filename);

	// NOTE: `open_file` significa `abrir ficheiro`, não `abrir diretório ou ficheiro` >:(
	if(path_is_dir(filepath))
		return NULL;

	FILE *fp = fopen(filepath, mode);

	return fp;
}

char *get_error(ERRORS_ENUM error){
	
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
			strcpy(error_string, "Cannot open the database entry file");
			break;
		case CANNOT_OPEN_CONTENT_FILE:
			strcpy(error_string, "Cannot open the file containing entry's content");
			break;
		case CANNOT_OPEN_CACHE_FILE:
			strcpy(error_string, "Cannot open the cache file of the given entry");
			break;
		case NOT_WRITTEN:
			strcpy(error_string, "The given entry is not written to a file");
			break;
		case CANNOT_OPEN_DB_DIR:
			strcpy(error_string, "Cannot open database directory");
			break;
		case CANNOT_CREATE_CONFIG_DIR:
			strcpy(error_string, "Cannot make configuration directory");
			break;
		case CANNOT_CREATE_DB_DIR:
			strcpy(error_string, "Cannot make database directory");
			break;
		case CANNOT_CREATE_CACHE_DIR:
			strcpy(error_string, "Cannot create cache directory");
			break;
		default:
			return NULL;
	}

	return error_string;

}


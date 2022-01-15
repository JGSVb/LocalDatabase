#include <stdio.h>
#include <stdlib.h>
#include "db.h"

#define _STR2(x) #x
#define print_db_entry_error(expr) do {\
	int ret;\
	if((ret = (expr)) != SUCCESS)\
		printf("%s: %s\n", _STR2(expr), db_entry_get_error(ret));\
} while(0)

int main(void){

	db_init();

	DbEntry *n = db_entry_new();
	
	print_db_entry_error(db_entry_append_tag(n, "queijo"));
	print_db_entry_error(db_entry_append_tag(n, "mortandela"));
	print_db_entry_error(db_entry_append_tag(n, "feijão"));
	print_db_entry_error(db_entry_append_tag(n, "melão"));
	
	db_entry_set_db_filename(n, "test.db");
	db_entry_set_content_filepath(n, "/home/asv/Pictures/Wallpapers/wp5.jpg");

	debug_db_entry_print_tags(n);

	print_db_entry_error(db_entry_update(n, 0));
	free(n);


	DbEntry *read;
	print_db_entry_error(db_entry_read("test.db", &read));

	debug_db_entry_print_tags(read);

	return 0;
}

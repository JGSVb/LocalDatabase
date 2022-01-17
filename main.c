#include <stdio.h>
#include <stdlib.h>
#include "db.h"
#include "dbentry.h"
#include "common.h"

#define _STR(x) #x
#define print_error(expr) do {\
	int ret;\
	if((ret = (expr)) < SUCCESS)\
		printf("\033[1m%s:%d:\033[0m %s -> %d: %s\n", __FILE__, __LINE__, _STR(expr), ret, get_error(ret));\
} while(0)

int main(void){

	print_error(db_init("database_legal"));

	DbEntry *n = db_entry_new();
	
	print_error(db_entry_append_tag(n, "queijo"));
	print_error(db_entry_append_tag(n, "mortandela"));
	print_error(db_entry_append_tag(n, "feijão"));
	print_error(db_entry_append_tag(n, "melão"));
	
	db_entry_set_db_filename(n, "test.db");
	db_entry_set_content_filepath(n, "/home/asv/Pictures/Wallpapers/wp5.jpg");

	debug_db_entry_print_tags(n);

	print_error(db_update_entry(n, 0));

	db_entry_set_db_filename(n, "test3232.db");
	print_error(db_update_entry(n, 0));

	free(n);


	DbEntry *loaded = db_entry_new();
	db_entry_set_db_filename(loaded, "test.db");
	print_error(db_load_entry(loaded));

	debug_db_entry_print_tags(loaded);

	int n_entries = db_n_entries();
	print_error(n_entries);
	printf("%d\n", n_entries);

	return 0;
}

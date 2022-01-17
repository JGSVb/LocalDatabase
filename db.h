#pragma once

#include <limits.h>
#include <linux/limits.h>
#include "dbentry.h"

#define MAX_DB_FILES 4096

int db_init(const char *dbname);
void db_finish(void);

int db_check_entry_written(DbEntry *entry);
int db_get_entry_index(DbEntry *entry);

int db_n_entries(void);

DbEntry *db_load_entry(unsigned i);

int db_delete_entry(unsigned i);
int db_update_entry(DbEntry *entry, int only_struct);

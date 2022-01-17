#pragma once

typedef struct _StrArr StrArr;

StrArr *strarr_init(StrArr *arr, char **items, unsigned int length, unsigned int itemsize);
StrArr *strarr_new(unsigned int length, unsigned int itemsize);
void strarr_free(StrArr *arr);

int strarr_item_count(StrArr *arr);
int strarr_is_full(StrArr *arr);
int strarr_is_empty(StrArr *arr);
int strarr_has_index(StrArr *arr, unsigned int index);

void strarr_clear(StrArr *arr);
int strarr_append(StrArr *arr, char *str);
int strarr_remove(StrArr *arr, unsigned int index);

int strarr_fetch(StrArr *arr, char *str);
char *strarr_get(StrArr *arr, unsigned int index);

void strarr_print(StrArr *arr);

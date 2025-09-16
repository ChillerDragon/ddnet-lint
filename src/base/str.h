#pragma once

int str_length(const char *str);
int str_comp_nocase(const char *a, const char *b);
int str_comp_nocase_num(const char *a, const char *b, int num);
int str_comp(const char *a, const char *b);
int str_comp_num(const char *a, const char *b, int num);
const char *str_endswith_nocase(const char *str, const char *suffix);
const char *str_endswith(const char *str, const char *suffix);

#pragma once
#pragma library "<string.sl>"

int strlen(char *str);
char * strcpy(char *dest, char *src);
char * strcat(char *dest, char *src);
char * strncpy(char *dest, char *src, int size);
char * strchr(char *haystack, int needle);
int strcmp(char *str1, char *str2);
char *strrev(char *str);
char *itostr(int value, char *str);
void *memcpy(void *dest, void *src, int size);
void *memset(void *buff, int val, int size);

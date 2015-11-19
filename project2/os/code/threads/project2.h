#ifndef PROJECT2_H
#define PROJECT2_H

#include <stdio.h>

// read one line from FILE, return the line or NULL if there's no more line
char* readline(FILE* fp);
// download the web pages from the url
void serve(char* url);

#endif
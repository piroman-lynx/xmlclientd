#include <string.h>

int strpos(char *needle, char *haystack)
{
   char *p = strstr(haystack, needle);
   if (p){
      return p-haystack;
   }
   return -1;   // Not found = -1.
}

void trim(char **str)
{
    char ptr [ strlen((*str)) + 1 ];
    int i, j = 0;
    for(i=0; (*str)[i] != '\0'; i++) {
	if ((*str)[i] != ' ' && (*str)[i] != '\t' && (*str)[i] != '\n' && (*str)[i] != '\r'){
	    ptr[j++] = (*str)[i];
	}
    }
    ptr[j] = '\0';
    strcpy((*str), ptr);
}

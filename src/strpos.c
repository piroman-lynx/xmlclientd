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
    int charBeen=0;
    for(i=0; (*str)[i] != '\0'; i++) {
	if (charBeen == 0){
	    if ((*str)[i] == ' ' || (*str)[i] == '\t'){
		debug("skip space");
		continue;
	    }
	}else{
	    if ((*str)[i] == '\n' || (*str)[i] == '\r'){
		continue;
	    }
	}
	ptr[j++] = (*str)[i];
	charBeen=1;
    }
    ptr[j] = '\0';
    strcpy((*str), ptr);
}

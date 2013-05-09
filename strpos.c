#include <string.h>

int strpos(char *needle, char *haystack)
{
   char *p = strstr(haystack, needle);
   if (p){
      return haystack-p;
   }
   return -1;   // Not found = -1.
}
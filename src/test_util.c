#include "test_util.h"

bool util_check_dep(char *files) {
    char *prev_index = files;
    char *index = NULL;
    while(prev_index != NULL) {
        index = strchr(prev_index, '\n');
        if(!index) index = files+strlen(files);
        int count = index-prev_index;
        char *filebuf = malloc(sizeof(char) * (count + 1));
        strncpy(filebuf, prev_index, count);
        filebuf[count] = '\0';
        FILE *temp = fopen(filebuf, "r+");
        if(! temp) return false;
        fclose(temp);
        prev_index = (index == files+strlen(files))? NULL : (index + 1);
        if(filebuf) free(filebuf);
    }
    return true;
} 

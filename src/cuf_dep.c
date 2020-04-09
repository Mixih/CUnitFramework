/**
 * @file cuf_dep.c
 * @brief CUnitFramework (CUF): Dependency Tracker Implementation
 * @details Dependency tracking for the cuf framework, allowing dependency
 * checking for skipping and other operations
 */
#include "cuf_dep.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static bool filedeps_met(char *files);


Dependency *dependency_create() {
    Dependency *deps = malloc(sizeof(Dependency));
    deps->filedeps = NULL;
    deps->has_filedeps = false;
    return deps;
}


Dependency *dependency_reg_filedeps(Dependency *deps, char *filedeps) {
    if(deps->has_filedeps) {
        deps->filedeps = realloc(deps->filedeps, 
                                sizeof(char) * (strlen(filedeps)+1));
    } else {
        deps->has_filedeps = true;
        deps->filedeps = malloc(sizeof(char) * (strlen(filedeps)+1));
    }
    strcpy(deps->filedeps, filedeps);
    return deps;
}

bool dependency_check(Dependency *deps) {
    int ret = true;
    if(deps->has_filedeps) ret &= filedeps_met(deps->filedeps);
    return ret;
}

void dependency_destroy(Dependency *deps) {
    if(deps->filedeps) free(deps->filedeps);
    free(deps);
}

static bool filedeps_met(char *files) {
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

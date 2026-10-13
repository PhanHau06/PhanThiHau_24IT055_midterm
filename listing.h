
#ifndef LISTING_H
#define LISTING_H

#include <sys/types.h>
#include <sys/stat.h>
#include "options.h"

/* Information about one filesystem entry. */
typedef struct {
    char *name;
    char *path;
    struct stat info;
} FileEntry;

/* List directory contents. */
int list_directory(const char *path,
                   const LsOptions *opts);

#endif

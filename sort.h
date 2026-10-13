
#ifndef SORT_H
#define SORT_H

#include <stddef.h>
#include "listing.h"

/* Sort filesystem entries according to options. */
void sort_entries(FileEntry *entries,
                  size_t count,
                  const LsOptions *opts);

#endif


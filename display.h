
#ifndef DISPLAY_H
#define DISPLAY_H

#include "listing.h"

/* Display one filesystem entry. */
void display_entry(const FileEntry *entry,
                   const LsOptions *opts);

#endif


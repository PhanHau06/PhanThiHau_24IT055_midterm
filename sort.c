
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sort.h"

/* Options used by qsort's comparison function. */
static const LsOptions *current_opts;

/* Select the timestamp used for sorting. */
static struct timespec get_time(const FileEntry *entry)
{
    if (current_opts->access_time)
        return entry->info.st_atim;

    if (current_opts->change_time)
        return entry->info.st_ctim;

    return entry->info.st_mtim;
}

/* Compare two filesystem entries. */
static int compare_entries(const void *a, const void *b)
{
    const FileEntry *first = a;
    const FileEntry *second = b;
    int result = 0;

    if (current_opts->sort_size) {
        if (first->info.st_size > second->info.st_size)
            result = -1;
        else if (first->info.st_size < second->info.st_size)
            result = 1;
    } else if (current_opts->sort_time) {
        struct timespec t1 = get_time(first);
        struct timespec t2 = get_time(second);

        if (t1.tv_sec > t2.tv_sec)
            result = -1;
        else if (t1.tv_sec < t2.tv_sec)
            result = 1;
        else if (t1.tv_nsec > t2.tv_nsec)
            result = -1;
        else if (t1.tv_nsec < t2.tv_nsec)
            result = 1;
    }

    /* Use filenames to break ties. */
    if (result == 0)
        result = strcmp(first->name, second->name);

    if (current_opts->reverse)
        result = -result;

    return result;
}

/* Sort unless -f was specified. */
void sort_entries(FileEntry *entries,
                  size_t count,
                  const LsOptions *opts)
{
    if (opts->no_sort || count < 2)
        return;

    current_opts = opts;

    qsort(entries, count, sizeof(FileEntry),
          compare_entries);
}


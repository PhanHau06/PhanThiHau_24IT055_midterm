
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "options.h"
#include "listing.h"
#include "display.h"

/*
 * Display a single filesystem entry.
 */
static int show_file(const char *path,
                     const LsOptions *opts)
{
    FileEntry entry;

    entry.name = (char *)path;
    entry.path = (char *)path;

    if (lstat(path, &entry.info) != 0) {
        fprintf(stderr, "myls: %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    display_entry(&entry, opts);
    return 0;
}

/*
 * Process either a file or a directory.
 */
static int process_path(const char *path,
                        const LsOptions *opts)
{
    struct stat st;

    if (lstat(path, &st) != 0) {
        fprintf(stderr, "myls: %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    /*
     * -d displays directories as entries,
     * without opening their contents.
     */
    if (opts->directory_only || !S_ISDIR(st.st_mode))
        return show_file(path, opts);

    return list_directory(path, opts);
}

int main(int argc, char *argv[])
{
    LsOptions opts;
    int status = 0;

    if (parse_options(argc, argv, &opts) != 0) {
        fprintf(stderr,
                "Usage: myls [-AacdFfhiklnqRrSstuw] "
                "[file ...]\n");
        return 1;
    }

    if (optind >= argc) {
        return process_path(".", &opts) == 0 ? 0 : 1;
    }

    for (int i = optind; i < argc; i++) {
        if (process_path(argv[i], &opts) != 0)
            status = 1;
    }

    return status;
}


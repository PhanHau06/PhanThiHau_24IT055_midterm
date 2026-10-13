
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "options.h"
#include "listing.h"
#include "display.h"
#include "sort.h"

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

/*
 * Main entry point.
 * Display non-directory operands before directories.
 */
int main(int argc, char *argv[])
{
    LsOptions opts;
    FileEntry *files;
    FileEntry *dirs;
    size_t nf = 0, nd = 0;
    size_t capacity;
    int status = 0;

    if (parse_options(argc, argv, &opts) != 0) {
        fprintf(stderr,
                "Usage: myls [-AacdFfhiklnqRrSstuw] [file ...]\n");
        return 1;
    }

    if (optind >= argc)
        return process_path(".", &opts) == 0 ? 0 : 1;

    capacity = (size_t)(argc - optind);

    files = calloc(capacity, sizeof(*files));
    dirs = calloc(capacity, sizeof(*dirs));

    if (files == NULL || dirs == NULL) {
        perror("myls: calloc");
        free(files);
        free(dirs);
        return 1;
    }

    /* Classify file and directory operands. */
    for (int i = optind; i < argc; i++) {
        struct stat st;
        FileEntry entry;

        if (lstat(argv[i], &st) != 0) {
            fprintf(stderr, "myls: %s: %s\n",
                    argv[i], strerror(errno));
            status = 1;
            continue;
        }

        entry.name = argv[i];
        entry.path = argv[i];
        entry.info = st;

        if (S_ISDIR(st.st_mode) && !opts.directory_only)
            dirs[nd++] = entry;
        else
            files[nf++] = entry;
    }

    /* Sort file and directory operands separately. */
    sort_entries(files, nf, &opts);
    sort_entries(dirs, nd, &opts);

    /* Display regular file operands first. */
    for (size_t i = 0; i < nf; i++)
        display_entry(&files[i], &opts);

    /* Display directory contents afterwards. */
    for (size_t i = 0; i < nd; i++) {
        if (nf > 0 || nd > 1) {
            if (i > 0 || nf > 0)
                putchar('\n');

            printf("%s:\n", dirs[i].name);
        }

        if (list_directory(dirs[i].path, &opts) != 0)
            status = 1;
    }

    free(files);
    free(dirs);

    return status;
}

#include <inttypes.h>
#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "listing.h"
#include "sort.h"

/* Determine whether an entry should be displayed. */
static int should_show(const char *name,
                       const LsOptions *opts)
{
    if (opts->show_all)
        return 1;

    if (opts->almost_all)
        return strcmp(name, ".") != 0 &&
               strcmp(name, "..") != 0;

    return name[0] != '.';
}

/* Release all allocated entry information. */
static void free_entries(FileEntry *entries, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].path);
    }

    free(entries);
}

/* Read, sort and display a directory. */
int list_directory(const char *path,
                   const LsOptions *opts)
{
    DIR *dir = opendir(path);
    struct dirent *item;
    FileEntry *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    int status = 0;

    if (dir == NULL) {
        fprintf(stderr, "myls: %s: %s\n",
                path, strerror(errno));
        return -1;
    }

    for (;;) {
        errno = 0;
        item = readdir(dir);

        if (item == NULL) {
            if (errno != 0) {
                perror("myls: readdir");
                status = -1;
            }
            break;
        }

        if (!should_show(item->d_name, opts))
            continue;

        if (count == capacity) {
            size_t new_capacity =
                capacity == 0 ? 16 : capacity * 2;

            if (new_capacity < capacity ||
                new_capacity > ((size_t)-1) /
                               sizeof(*entries)) {
                fprintf(stderr, "myls: too many entries\n");
                status = -1;
                break;
            }

            FileEntry *temp = realloc(
                entries, new_capacity * sizeof(*entries));

            if (temp == NULL) {
                perror("myls: realloc");
                status = -1;
                break;
            }

            entries = temp;
            capacity = new_capacity;
        }

        size_t path_len = strlen(path);
        size_t name_len = strlen(item->d_name);

        if (path_len > (size_t)-1 - name_len - 2) {
            fprintf(stderr, "myls: path too long\n");
            status = -1;
            break;
        }

        char *full_path = malloc(path_len + name_len + 2);
        char *name = malloc(name_len + 1);

        if (full_path == NULL || name == NULL) {
            perror("myls: malloc");
            free(full_path);
            free(name);
            status = -1;
            break;
        }

        sprintf(full_path, "%s/%s", path, item->d_name);
        memcpy(name, item->d_name, name_len + 1);

        entries[count].name = name;
        entries[count].path = full_path;

        if (lstat(full_path, &entries[count].info) != 0) {
            fprintf(stderr, "myls: %s: %s\n",
                    full_path, strerror(errno));
            free(name);
            free(full_path);
            status = -1;
            continue;
        }

        count++;
    }

    if (closedir(dir) != 0) {
        perror("myls: closedir");
        status = -1;
    }

    if (status == 0) {
        sort_entries(entries, count, opts);

        /* Total blocks used by directory entries. */
        if (opts->long_format || opts->show_blocks) {
            uintmax_t total = 0;

            for (size_t i = 0; i < count; i++)
                total += (uintmax_t)entries[i].info.st_blocks;

            if (opts->kilobytes)
                total = (total + 1) / 2;

            printf("total %ju\n", total);
        }

        /* Display entries in the current directory. */
        for (size_t i = 0; i < count; i++)
            display_entry(&entries[i], opts);

        /* Recursively visit subdirectories when -R is used. */
        if (opts->recursive && !opts->directory_only) {
            for (size_t i = 0; i < count; i++) {
                const char *name = entries[i].name;

                /* Avoid recursion through . and .. */
                if (strcmp(name, ".") == 0 ||
                    strcmp(name, "..") == 0)
                    continue;

                /* Do not follow symbolic links. */
                if (!S_ISDIR(entries[i].info.st_mode))
                    continue;

                printf("\n%s:\n", entries[i].path);

                if (list_directory(entries[i].path, opts) != 0)
                    status = -1;
            }
        }
    }

    free_entries(entries, count);

    return status;
}

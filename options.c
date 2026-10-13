
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "options.h"

/*
 * Parse command-line options using getopt().
 * Return 0 on success, -1 on invalid options.
 */
int parse_options(int argc, char *argv[], LsOptions *opts)
{
    int ch;

    memset(opts, 0, sizeof(*opts));

    while ((ch = getopt(argc, argv,
                        "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
            case 'A': opts->almost_all = 1; break;
            case 'a': opts->show_all = 1; break;
            case 'c': opts->change_time = 1; break;
            case 'd': opts->directory_only = 1; break;
            case 'F': opts->classify = 1; break;
            case 'f': opts->no_sort = 1; break;
            case 'h': opts->human_readable = 1; break;
            case 'i': opts->show_inode = 1; break;
            case 'k': opts->kilobytes = 1; break;
            case 'l': opts->long_format = 1; break;
            case 'n': opts->numeric_ids = 1; break;
            case 'q': opts->escape_names = 1; break;
            case 'R': opts->recursive = 1; break;
            case 'r': opts->reverse = 1; break;
            case 'S': opts->sort_size = 1; break;
            case 's': opts->show_blocks = 1; break;
            case 't': opts->sort_time = 1; break;
            case 'u': opts->access_time = 1; break;
            case 'w': opts->raw_names = 1; break;

            default:
                return -1;
        }
    }

    /* -n implies long format. */
    if (opts->numeric_ids)
        opts->long_format = 1;

    return 0;
}


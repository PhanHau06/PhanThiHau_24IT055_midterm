
#ifndef OPTIONS_H
#define OPTIONS_H

/* Command-line options supported by myls. */
typedef struct {
    int show_all;       /* -a */
    int almost_all;     /* -A */
    int change_time;    /* -c */
    int directory_only; /* -d */
    int classify;       /* -F */
    int no_sort;        /* -f */
    int human_readable; /* -h */
    int show_inode;     /* -i */
    int kilobytes;      /* -k */
    int long_format;    /* -l */
    int numeric_ids;    /* -n */
    int escape_names;   /* -q */
    int recursive;      /* -R */
    int reverse;        /* -r */
    int sort_size;      /* -S */
    int show_blocks;    /* -s */
    int sort_time;      /* -t */
    int access_time;    /* -u */
    int raw_names;      /* -w */
} LsOptions;

/* Parse command-line arguments. */
int parse_options(int argc, char *argv[], LsOptions *opts);

#endif


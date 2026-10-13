
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <inttypes.h>

#include <unistd.h>
#include "display.h"

void print_human_size(unsigned long long size);
static void print_filename(const char *name, const LsOptions *opts);

/* Print Unix file type and permission bits. */
static void print_permissions(mode_t mode)
{
    char p[11] = "----------";

    if (S_ISDIR(mode)) p[0] = 'd';
    else if (S_ISLNK(mode)) p[0] = 'l';
    else if (S_ISCHR(mode)) p[0] = 'c';
    else if (S_ISBLK(mode)) p[0] = 'b';
    else if (S_ISFIFO(mode)) p[0] = 'p';
    else if (S_ISSOCK(mode)) p[0] = 's';

    if (mode & S_IRUSR) p[1] = 'r';
    if (mode & S_IWUSR) p[2] = 'w';
    if (mode & S_IXUSR) p[3] = 'x';

    if (mode & S_IRGRP) p[4] = 'r';
    if (mode & S_IWGRP) p[5] = 'w';
    if (mode & S_IXGRP) p[6] = 'x';

    if (mode & S_IROTH) p[7] = 'r';
    if (mode & S_IWOTH) p[8] = 'w';
    if (mode & S_IXOTH) p[9] = 'x';

    if (mode & S_ISUID)
        p[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        p[6] = (mode & S_IXGRP) ? 's' : 'S';
    if (mode & S_ISVTX)
        p[9] = (mode & S_IXOTH) ? 't' : 'T';

    printf("%s ", p);
}

/* Print a single filesystem entry. */
/* Append a file type indicator for -F. */
static void print_indicator(mode_t mode)
{
    if (S_ISDIR(mode))
        putchar('/');
    else if (S_ISLNK(mode))
        putchar('@');
    else if (S_ISSOCK(mode))
        putchar('=');
    else if (S_ISFIFO(mode))
        putchar('|');
    else if (S_ISREG(mode) &&
             (mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
        putchar('*');
}
void display_entry(const FileEntry *entry,
                   const LsOptions *opts)
{
    const struct stat *st = &entry->info;

if (opts->show_inode)
        printf("%ju ", (uintmax_t)st->st_ino);
    /* Display allocated blocks for -s. */
    if (opts->show_blocks) {
        uintmax_t blocks = (uintmax_t)st->st_blocks;

        /* Convert 512-byte blocks to 1 KB units. */
        if (opts->kilobytes)
            blocks = (blocks + 1) / 2;

        printf("%ju ", blocks);
    }

    if (opts->long_format) {
        print_permissions(st->st_mode);

        printf("%ju ", (uintmax_t)st->st_nlink);

        if (opts->numeric_ids) {
            printf("%ju %ju ",
                   (uintmax_t)st->st_uid,
                   (uintmax_t)st->st_gid);
        } else {
            struct passwd *pw = getpwuid(st->st_uid);
            struct group *gr = getgrgid(st->st_gid);

            if (pw != NULL)
                printf("%s ", pw->pw_name);
            else
                printf("%ju ", (uintmax_t)st->st_uid);

            if (gr != NULL)
                printf("%s ", gr->gr_name);
            else
                printf("%ju ", (uintmax_t)st->st_gid);
        }

        if (opts->human_readable && st->st_size >= 0)
            print_human_size((unsigned long long)st->st_size);
        else
            printf("%jd ", (intmax_t)st->st_size);

        time_t t = st->st_mtime;

        if (opts->access_time)
            t = st->st_atime;
        else if (opts->change_time)
            t = st->st_ctime;

        struct tm *tm_info = localtime(&t);
        char time_buf[64] = "?";

        if (tm_info != NULL) {
            strftime(time_buf, sizeof(time_buf),
                     "%b %e %H:%M", tm_info);
        }

        printf("%s ", time_buf);
    }



    print_filename(entry->name, opts);
    /* Display symbolic link target in long format. */
    if (opts->long_format && S_ISLNK(st->st_mode)) {
        char target[4096];

        ssize_t len = readlink(entry->path, target,
                               sizeof(target) - 1);

        if (len >= 0) {
            target[len] = '\0';
            printf(" -> %s", target);
        }
    }

    if (opts->classify)
        print_indicator(st->st_mode);
    putchar('\n');
}

/* Print a byte count in a human-readable format. */
void print_human_size(unsigned long long size)
{
    const char *units[] = {"", "K", "M", "G", "T", "P"};
    double value = (double)size;
    int unit = 0;

    while (value >= 1024.0 && unit < 5) {
        value /= 1024.0;
        unit++;
    }

    if (unit == 0)
        printf("%llu ", size);
else if (value < 10.0)
    printf("%.1f%s ", value, units[unit]);
else
    printf("%.0f%s ", value, units[unit]);
}

/* Print file names and replace non-printable bytes for -q. */
static void print_filename(const char *name,
                           const LsOptions *opts)
{
    const unsigned char *p = (const unsigned char *)name;

    while (*p != '\0') {
        if (opts->escape_names && !isprint(*p))
            putchar('?');
        else
            putchar(*p);
        p++;
    }
}

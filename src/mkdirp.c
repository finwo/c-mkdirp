#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#  include <Windows.h>
#else
#  include <sys/stat.h>
#endif

#include "mkdirp.h"

#ifndef DIRECTORY_SEPARATOR
#if defined(_WIN32) || defined(_WIN64)
const char SEP = '\\';
#else
#define DIRECTORY_SEPARATOR "/"
const char SEP = '/';
#endif
#endif

bool mkdirp(const char *dirname) {
    const char *p;
    char       *temp;
    bool        ret = true;

    temp = calloc(1, strlen(dirname)+2);

    /* Skip Windows drive letter. */
#if defined(_WIN32) || defined(_WIN64)
    if ((p = strchr(dirname, ':') != NULL) {
        p++;
    } else {
#endif
        p = dirname;
#if defined(_WIN32) || defined(_WIN64)
    }
#endif

    // Add support for paths not ending in a separator
    // Because it checked for the next separator but didn't assign the whole path
    if (temp[strlen(dirname) - 1] != SEP) {
      temp[strlen(dirname)] = SEP;
    }

    while ((p = strchr(p, SEP)) != NULL) {
        /* Skip empty elements. Could be a Windows UNC path or
           just multiple separators which is okay. */
        if (p != dirname && *(p-1) == SEP) {
            p++;
            continue;
        }
        /* Put the path up to this point into a temporary to
           pass to the make directory function. */
        memcpy(temp, dirname, p-dirname);
        temp[p-dirname] = '\0';
        p++;
        if (!strlen(temp)) {
          continue;
        }
#if defined(_WIN32) || defined(_WIN64)
        if (CreateDirectory(temp, NULL) == FALSE) {
            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                ret = false;
                break;
            }
        }
#else
        if (mkdir(temp, 0774) != 0) {
            if (errno != EEXIST) {
                ret = false;
                break;
            }
        }
#endif
    }
    free(temp);
    return ret;
}


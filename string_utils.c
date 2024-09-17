//
// Created by zhivko-kocev on 9/17/24.
//
#include "string_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char *decode(const char *filename) {
    if (!strcmp(filename, "")) return "";

    int len = strlen(filename);
    char *decoded = malloc(len + 1);
    int decoded_index = 0;
    for (int i = 0; i < len; i++) {
        if (filename[i] == '%' && i + 2 < len) {
            sscanf(filename + i + 1, "%2x", &decoded[decoded_index++]);
            i += 2;
            continue;
        }

        decoded[decoded_index++] = filename[i];
    }

    decoded[decoded_index] = '\0';

    return decoded;
}

char *get_mime(const char *file_ext) {
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0) return "text/html";
    if (strcasecmp(file_ext, "txt") == 0) return "text/plain";
    if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0) return "image/jpeg";
    if (strcasecmp(file_ext, "png") == 0) return "image/png";

    return "application/octet-stream";
}

char *get_file(const char *filename) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir("./static")) == NULL) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, filename)) {
            return entry->d_name;
        }
    }

    closedir(dir);
    return NULL;
}

char *get_extension(const char *filename) {
    char *ext = strrchr(filename, '.');
    if (!ext || ext == filename) return "";
    return ext + 1;
}

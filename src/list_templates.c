#include "template.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define COLOR_RESET  "\x1b[0m"
#define COLOR_BOLD   "\x1b[1m"
#define COLOR_CYAN   "\x1b[36m"

void list_templates_xml(const char *dir_path) {
    DIR *dir;
    struct dirent *ent;

    dir = opendir(dir_path);
    if (dir == NULL) {
        fprintf(stderr, "Error: cannot open the directory '%s'\n", dir_path);
        return;
    }

    printf(COLOR_CYAN COLOR_BOLD "Available Templates:" COLOR_RESET "\n");

    int count = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) { // Regular file
            const char *extension = strrchr(ent->d_name, '.');

            if (extension != NULL && strcmp(extension, ".xml") == 0) {
                printf("  " COLOR_BOLD "»" COLOR_RESET " %s\n", ent->d_name);
                count++;
            }
        }
    }

    if (count == 0) {
        printf("  No .xml templates found in '%s'\n", dir_path);
    }

    closedir(dir);
}

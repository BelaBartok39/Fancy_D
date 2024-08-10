#ifndef FANCY_H // prevents crazy header collisions from ever happening
#define FANCY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <cjson/cJSON.h>
#include <signal.h>

#define MAX_PATH 4096
#define MAX_EXTENSIONS 1000

typedef struct {
    char *extension;
    char *category;
} ExtensionMapping;

// Function prototypes
void print_usage(const char *program_name);
void ensure_config_folder(const char *config_folder);
void create_default_configs(const char *config_folder);
void load_configs(const char *config_folder);
void organize_files(const char *directory, int extreme_sort);
void add_extension(const char *config_folder, const char *extension, const char *category);
void print_string_details(const char* str);

// Global variables (if needed)
extern ExtensionMapping *mappings;
extern int mapping_count;

#endif // FANCY_H
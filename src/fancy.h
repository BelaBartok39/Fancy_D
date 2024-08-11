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
#include <ftw.h> // old school baby

#define MAX_PATH 4096
#define MAX_EXTENSIONS 1000
#define _XOPEN_SOURCE 500
#define FTW_DEPTH 1
#define FTW_PHYS 2 // protects against leaving directory

typedef struct {
    char *extension;
    char *category;
} ExtensionMapping;

// Function prototypes
void print_usage(const char *program_name);
void ensure_config_folder(const char *config_folder);
void create_default_configs(const char *config_folder);
void load_configs(const char *config_folder);
void organize_files(const char *directory);
void add_extension(const char *config_folder, const char *extension, const char *category);
void print_string_details(const char* str);

#endif // FANCY_H

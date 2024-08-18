// ============================================================================= //
// Program Name: FancyD (Fancy Directory Organizer)
// Author: Nicholas D. Redmond (3A3YN1CKY)
// Date: 8/13/2024
// Description: Simple program to organize files in a directory based on their ext
// ============================================================================= //


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
#include <strings.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_EXTENSIONS 1000
#define FALLBACK_PREFIX "/tmp/fancyD_fallback_"

#ifndef FTW_DEPTH
#define FTW_DEPTH 1
#endif

#ifndef FTW_PHYS
#define FTW_PHYS 2 // protects against leaving directory when sorting
#endif

#ifdef PATH_MAX
    // PATH_MAX is defined, we can use it
    #define MAX_PATH PATH_MAX
#else
    // PATH_MAX is not defined, fall back to a default value
    #define MAX_PATH 5008
#endif

typedef struct {
    char *extension;
    char *category;
} ExtensionMapping;

// Function prototypes
void print_usage(const char *program_name);
void ensure_config_folder(const char *config_folder);
int create_default_configs(const char *config_folder);
void load_configs(const char *config_folder);
void organize_files(const char *directory);
void add_extension(const char *config_folder, const char *extension, const char *new_category);
void print_string_details(const char* str);
void reload_mappings(const char *config_folder);
void reset_mappings(const char *config_folder);
void delete_config_files(const char *config_folder);
int delete_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

int check_path_length(const char *path);
char *create_fallback_path(const char *original_path);
int move_file_with_fallback(const char *src, const char *dest);

void remove_extension_from_config(const char *config_path, const char *extension);
void list_extensions(const char *config_folder);
int check_duplicate_extension(const char *config_folder, const char *extension, const char *new_category);

extern ExtensionMapping *mappings;
extern int mapping_count;
extern int verbose;

#endif // FANCY_H

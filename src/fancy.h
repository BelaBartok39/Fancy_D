/* =============================================================================
   Program Name: FancyD (Fancy Directory Organizer)
   Author: Nicholas D. Redmond (3A3YN1CKY)
   Date: 8/13/2024
   Description: Simple program to organize files in a directory based on their ext
   ============================================================================= */

#ifndef FANCY_H
#define FANCY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <cjson/cJSON.h>
#include <signal.h>
#include <ftw.h>
#include <strings.h>
#include <limits.h>
#include <stdbool.h>

#ifndef PROJECT_ROOT
#error "PROJECT_ROOT is not defined. Make sure to compile with the correct Makefile."
#endif

#define PATH_TO_ROOT PROJECT_ROOT

#define MAX_EXTENSIONS 1000
#define FALLBACK_PREFIX "/tmp/fancyD_fallback_"

#ifndef FTW_DEPTH
#define FTW_DEPTH 1
#endif

#ifndef FTW_PHYS
#define FTW_PHYS 2
#endif

#ifdef PATH_MAX
    #define MAX_PATH PATH_MAX
#else
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
char* create_fallback_path(const char *original_path);
int move_file_with_fallback(const char *src, const char *dest);
void remove_extension_from_config(const char *config_path, const char *extension);
void list_extensions(const char *config_folder);
int check_duplicate_extension(const char *config_folder, const char *extension, const char *new_category);
char* find_project_root();
char* get_default_config_path();

// New function prototypes
bool is_config_file(const char *filename);
char* read_file_content(const char *filepath);
void print_category_extensions(const char *filename, cJSON *json);
char* construct_file_path(const char *folder, const char *filename);
void process_default_config(const char *config_folder, const char *file_path);
void free_existing_mappings();
void initialize_mappings();
void handle_missing_configs(const char *config_folder);
void process_config_file(const char *file_path);
void handle_json_parse_error(const char *file_path);
void add_mappings_from_json(cJSON *json);
void add_mapping(const char *extension, const char *category);
bool check_for_uncategorized_files(const char *directory);
bool is_special_directory(const char *name);
bool is_regular_file(const char *path);
char* get_file_extension(const char *filename);
char* get_category_for_extension(const char *extension);
bool prompt_for_misc_category();
void process_directory(const char *directory, bool handle_misc);
void process_file(const char *file_path, const char *directory, bool handle_misc);
bool prompt_for_extension_move(const char *extension, const char *old_category, const char *new_category);
void remove_extension_from_category(const char *config_folder, const char *extension, const char *category);
char* construct_config_path(const char *config_folder, const char *category);
cJSON* load_or_create_json(const char *config_path);
void add_extension_to_json(cJSON *json, const char *extension, const char *category);
void save_json_to_file(cJSON *json, const char *config_path, const char *extension, const char *category);
void move_file_to_category(const char *file_path, const char *directory, const char *category);

extern ExtensionMapping *mappings;
extern int mapping_count;
extern int verbose;

#endif // FANCY_H

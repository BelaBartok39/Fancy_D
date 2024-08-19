/* =============================================================================
   Program Name: FancyD (Fancy Directory Organizer)
   Author: Nicholas D. Redmond (3A3YN1CKY)
   Date: 8/13/2024
   Description: Simple program to organize files in a directory based on their ext
   ============================================================================= */

#include <fancy.h>
#include <color_utils.h>

ExtensionMapping *mappings = NULL;
int mapping_count = 0;
int verbose = 0;

bool is_config_file(const char *filename) {
    return strstr(filename, "_config.json") != NULL;
}

char* read_file_content(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "Unable to open file: %s\n", filepath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(fsize + 1);
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(content, 1, fsize, file);
    fclose(file);
    content[fsize] = 0;

    return content;
}

void list_extensions(const char *config_folder) {
    DIR *dir;
    struct dirent *ent;
    char file_path[MAX_PATH];
    
    dir = opendir(config_folder);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open config folder: %s\n", config_folder);
        return;
    }
    
    print_green("\nCurrent Extensions and Categories:\n");
    printf("----------------------------------\n");
    
    while((ent = readdir(dir)) != NULL) {
        if (!is_config_file(ent->d_name)) continue;
    
        snprintf(file_path, sizeof(file_path), "%s/%s", config_folder, ent->d_name);
        
        char *json_str = read_file_content(file_path);
        if (json_str == NULL) continue;
        
        cJSON *json = cJSON_Parse(json_str);
        free(json_str);

        if (json == NULL) {
            fprintf(stderr, "JSON parse error for file: %s\n", file_path);
            continue;
        }

        print_category_extensions(ent->d_name, json);

        cJSON_Delete(json);
    }

    closedir(dir);
    printf("\n");
}

void print_category_extensions(const char *filename, cJSON *json) {
    char *category = strdup(filename);
    char *underscore = strchr(category, '_');
    if (underscore != NULL) {
        *underscore = '\0';
    }
    
    print_blue("\nCategory: %s\n", category);
    
    cJSON *extension;
    cJSON_ArrayForEach(extension, json) {
        printf(" %s\n", extension->string);
    }

    free(category);
}


char* construct_file_path(const char *folder, const char *filename) {
    char *file_path = malloc(MAX_PATH);
    if (file_path == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    snprintf(file_path, MAX_PATH, "%s/%s", folder, filename);
    return file_path;
}

void process_default_config(const char *config_folder, const char *file_path) {
    char *json_str = read_file_content(file_path);
    if (json_str == NULL) return;

    cJSON *json = cJSON_Parse(json_str);
    free(json_str);

    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON in file: %s\n", file_path);
        return;
    }

    cJSON *extension;
    cJSON_ArrayForEach(extension, json) {
        if (check_duplicate_extension(config_folder, extension->string, extension->valuestring) == 0) {
            add_extension(config_folder, extension->string, extension->valuestring);
        } else {
            printf("Skipping conflicting extension %s\n", extension->string);
        }
    }

    cJSON_Delete(json);
}

void free_existing_mappings() {
    for (int i = 0; i < mapping_count; i++) {
        free(mappings[i].extension);
        free(mappings[i].category);
    }
    free(mappings);
}

void initialize_mappings() {
    mappings = malloc(sizeof(ExtensionMapping) * MAX_EXTENSIONS);
    if (!mappings) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(mappings, 0, sizeof(ExtensionMapping) * MAX_EXTENSIONS);
    mapping_count = 0;
}

void handle_missing_configs(const char *config_folder) {
    fprintf(stderr, "No configuration files found. Creating default categories.\n");
    create_default_configs(config_folder);
    DIR *dir = opendir(config_folder);
    if (dir == NULL) {
        fprintf(stderr, "Failed to create default configs.\n");
        exit(1);
    }
    closedir(dir);
}

void process_config_file(const char *file_path) {
    char *json_str = read_file_content(file_path);
    if (json_str == NULL) return;

    cJSON *json = cJSON_Parse(json_str);
    free(json_str);

    if (json == NULL) {
        handle_json_parse_error(file_path);
        return;
    }

    add_mappings_from_json(json);
    cJSON_Delete(json);
}

void handle_json_parse_error(const char *file_path) {
    fprintf(stderr, "JSON parse error for file: %s\n", file_path);
}

void add_mappings_from_json(cJSON *json) {
    cJSON *extension;
    cJSON_ArrayForEach(extension, json) {
        if (mapping_count >= MAX_EXTENSIONS) {
            fprintf(stderr, "Too many extensions, increase MAX_EXTENSIONS\n");
            break;
        }

        add_mapping(extension->string, extension->valuestring);
    }
}

void add_mapping(const char *extension, const char *category) {
    mappings[mapping_count].extension = strdup(extension);
    mappings[mapping_count].category = strdup(category);
    mapping_count++;
}

bool check_for_uncategorized_files(const char *directory) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open directory: %s\n", directory);
        return false;
    }

    struct dirent *entry;
    bool uncategorized_files_found = false;

    while ((entry = readdir(dir)) != NULL) {
        if (is_special_directory(entry->d_name)) continue;

        char file_path[MAX_PATH];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (!is_regular_file(file_path)) continue;

        char *extension = get_file_extension(entry->d_name);
        if (extension && !get_category_for_extension(extension)) {
            uncategorized_files_found = true;
            break;
        }
    }

    closedir(dir);

    if (uncategorized_files_found) {
        return prompt_for_misc_category();
    }

    return false;
}

bool is_special_directory(const char *name) {
    return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

bool is_regular_file(const char *path) {
    struct stat path_stat;
    if (lstat(path, &path_stat) != 0) {
        fprintf(stderr, "Unable to get file stats for %s\n", path);
        return false;
    }
    return S_ISREG(path_stat.st_mode);
}

char* get_file_extension(const char *filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    static char extension[256];  // Static buffer to store the extension
    snprintf(extension, sizeof(extension), ".%s", dot + 1);  // Add leading dot
    return extension;
}

char* get_category_for_extension(const char *extension) {
    
    // Add leading dot if not present
    char full_extension[256];
    if (extension[0] != '.') {
        snprintf(full_extension, sizeof(full_extension), ".%s", extension);
    } else {
        strncpy(full_extension, extension, sizeof(full_extension));
    }
    
    for (int i = 0; i < mapping_count; i++) {
        if (strcasecmp(mappings[i].extension, full_extension) == 0) {
            return mappings[i].category;
        }
    }
    return NULL;
}

bool prompt_for_misc_category() {
    char response;
    printf("Uncategorized files found. Do you want to put them in a 'misc' folder? (y/n): ");
    scanf(" %c", &response);
    return (response == 'y' || response == 'Y');
}

void process_directory(const char *directory, bool handle_misc) {
    
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open directory: %s\n", directory);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        
        if (is_special_directory(entry->d_name)) {
            continue;
        }
        
        char file_path[MAX_PATH];
        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);
        
        if (!is_regular_file(file_path)) {
            continue;
        }
        
        process_file(file_path, directory, handle_misc);
    }

    closedir(dir);
}

void process_file(const char *file_path, const char *directory, bool handle_misc) {
    
    char *extension = get_file_extension(file_path);
    
    char *category = get_category_for_extension(extension);

    if (category == NULL && handle_misc) {
        category = "misc";
    }

    if (category != NULL) {
        move_file_to_category(file_path, directory, category);
    } else if (verbose) {
        printf("Skipping uncategorized file: %s\n", basename((char *)file_path));
    }
}

bool prompt_for_extension_move(const char *extension, const char *old_category, const char *new_category) {
    char response;
    printf("Do you want to move extension %s from %s to %s? (y/n): ", extension, old_category, new_category);
    scanf(" %c", &response);
    return (response == 'y' || response == 'Y');
}

void remove_extension_from_category(const char *config_folder, const char *extension, const char *category) {
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s/%s_config.json", config_folder, category);
    
    char *content = read_file_content(config_path);
    if (content == NULL) {
        fprintf(stderr, "Failed to read config file: %s\n", config_path);
        return;
    }

    cJSON *json = cJSON_Parse(content);
    free(content);

    if (json == NULL) {
        fprintf(stderr, "Failed to parse JSON in file: %s\n", config_path);
        return;
    }

    cJSON_DeleteItemFromObject(json, extension);

    if (cJSON_GetArraySize(json) == 0) {
        // If the JSON object is now empty, delete the file
        cJSON_Delete(json);
        if (remove(config_path) == 0) {
            printf("Removed empty config file: %s\n", config_path);
        } else {
            fprintf(stderr, "Failed to remove empty config file: %s\n", config_path);
        }
    } else {
        // Otherwise, write the updated JSON back to the file
        char *updated_content = cJSON_Print(json);
        FILE *file = fopen(config_path, "w");
        if (file) {
            fputs(updated_content, file);
            fclose(file);
            printf("Removed extension %s from category %s\n", extension, category);
        } else {
            fprintf(stderr, "Failed to write updated config to %s\n", config_path);
        }
        cJSON_Delete(json);
        free(updated_content);
    }
}

char* construct_config_path(const char *config_folder, const char *category) {
    char *config_path = malloc(MAX_PATH);
    if (config_path == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    snprintf(config_path, MAX_PATH, "%s/%s_config.json", config_folder, category);
    return config_path;
}

cJSON* load_or_create_json(const char *config_path) {
    char *content = read_file_content(config_path);
    cJSON *json = NULL;
    
    if (content) {
        json = cJSON_Parse(content);
        free(content);
    }
    
    if (json == NULL) {
        json = cJSON_CreateObject();
    }
    
    return json;
}

void add_extension_to_json(cJSON *json, const char *extension, const char *category) {
    cJSON_AddStringToObject(json, extension, category);
}

void save_json_to_file(cJSON *json, const char *config_path, const char *extension, const char *category) {
    char *updated_content = cJSON_Print(json);
    FILE *file = fopen(config_path, "w");
    if (file) {
        fputs(updated_content, file);
        fclose(file);
        printf("Added extension %s to category %s\n", extension, category);
    } else {
        fprintf(stderr, "Failed to write updated config to %s\n", config_path);
    }
    free(updated_content);
}

void move_file_to_category(const char *file_path, const char *directory, const char *category) {
    char category_path[MAX_PATH];
    char dest_path[MAX_PATH];
    
    snprintf(category_path, sizeof(category_path), "%s/%s", directory, category);
    
    if (mkdir(category_path, 0777) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to create category directory: %s\n", category_path);
        return;
    }

    strncpy(dest_path, category_path, MAX_PATH - 1);
    dest_path[MAX_PATH - 1] = '\0';
    strncat(dest_path, "/", MAX_PATH - strlen(dest_path) - 1);
    strncat(dest_path, basename((char *)file_path), MAX_PATH - strlen(dest_path) - 1);

    if (move_file_with_fallback(file_path, dest_path) != 0) {
        fprintf(stderr, "Failed to move %s to %s\n", file_path, dest_path);
    } else if (verbose) {
        printf("Moved %s to %s\n", basename((char *)file_path), category);
    }
}

int check_duplicate_extension(const char *config_folder, const char *extension, const char *new_category) {
    for (int i = 0; i < mapping_count; i++) {
        if (strcasecmp(mappings[i].extension, extension) == 0) {
            printf("Extension %s already exists in category %s.\n", extension, mappings[i].category);
            if (prompt_for_extension_move(extension, mappings[i].category, new_category)) {
                remove_extension_from_category(config_folder, extension, mappings[i].category);
                return 0;
            }
            return 1;
        }
    }
    return 0;
}

int create_default_configs(const char *config_folder) {
    const char *default_config_folder = get_default_config_path();
    if (default_config_folder == NULL) {
        fprintf(stderr, "Unable to determine default config folder path\n");
        return 1;
    }

    load_configs(config_folder);

    DIR *dir = opendir(default_config_folder);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open default config folder: %s\n", default_config_folder);
        return 1;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (!is_config_file(ent->d_name)) continue;

        char *file_path = construct_file_path(default_config_folder, ent->d_name);
        process_default_config(config_folder, file_path);
        free(file_path);
    }

    closedir(dir);
    return 0;
}

int move_file_with_fallback(const char *src, const char *dest) {
    if (check_path_length(src) || check_path_length(dest)) {
        char *fallback_dest = create_fallback_path(dest);
        if (!fallback_dest) {
            return -1;
        }

        fprintf(stderr, "Using fallback path: %s\n", fallback_dest);
        int result = rename(src, fallback_dest);
        free(fallback_dest);
        return result;
    }

    return rename(src, dest);
}

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [DIRECTORY]\n", program_name);
    printf("Options:\n");
    printf("  -a, --add EXT CATEGORY  Add a file extension to a category\n");
    printf("  -l, --list          Display current categories\n");
    printf("  -h, --help          Display this help message\n");
    printf("  -d, --default       Create default categories\n");
    printf("  -r, --reset         Reset categories\n");
    printf("  -v, --verbose       Enable verbose output\n");
}

void ensure_config_folder(const char *config_folder) {
    struct stat st = {0};
    if (stat(config_folder, &st) == -1) {
        if (mkdir(config_folder, 0700) == -1) {
            fprintf(stderr, "Error creating config folder: %s\n", strerror(errno));
            exit(1);
        }
    }
}

void add_extension(const char *config_folder, const char *extension, const char *new_category) {
    load_configs(config_folder);

    char *current_category = get_category_for_extension(extension);
    if (current_category != NULL) {
        if (strcmp(current_category, new_category) == 0) {
            printf("Extension %s is already in category %s.\n", extension, new_category);
            return;
        }

        printf("Extension %s already exists in category %s.\n", extension, current_category);
        printf("Do you want to move it to %s? (y/n): ", new_category);
        
        char response;
        scanf(" %c", &response);
        
        if (response != 'y' && response != 'Y') {
            printf("Extension %s will remain in category %s.\n", extension, current_category);
            return;
        }

        remove_extension_from_category(config_folder, extension, current_category);
    }

    char *config_path = construct_config_path(config_folder, new_category);
    cJSON *json = load_or_create_json(config_path);

    add_extension_to_json(json, extension, new_category);
    save_json_to_file(json, config_path, extension, new_category);

    cJSON_Delete(json);
    free(config_path);

    reload_mappings(config_folder);
}

char* get_default_config_path() {
    static char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s/default_configs", PATH_TO_ROOT);
    
    if (access(config_path, F_OK) != 0) {
        fprintf(stderr, "Default config directory not found: %s\n", config_path);
        return NULL;
    }

    return config_path;
}

void load_configs(const char *config_folder) {
    
    free_existing_mappings();
    initialize_mappings();

    DIR *dir = opendir(config_folder);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open config folder: %s\n", config_folder);
        handle_missing_configs(config_folder);
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (!is_config_file(ent->d_name)) continue;
        
        char *file_path = construct_file_path(config_folder, ent->d_name);
        
        process_config_file(file_path);
        free(file_path);
    }

    closedir(dir);
}

int check_path_length(const char *path) {
    if (strlen(path) >= PATH_MAX) {
        fprintf(stderr, "Warning: Path exceeds maximum length (%d): %s\n", PATH_MAX, path);
        return 1;
    }
    return 0;
}

char* create_fallback_path(const char *original_path) {
    char *fallback = malloc(PATH_MAX);
    if (!fallback) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    const char *filename = strrchr(original_path, '/');
    filename = filename ? filename + 1 : original_path;

    snprintf(fallback, PATH_MAX, "%s%s", FALLBACK_PREFIX, filename);
    return fallback;
}

void delete_config_files(const char *config_folder) {
    if (nftw(config_folder, delete_callback, 64, FTW_DEPTH | FTW_PHYS) == -1) {
        fprintf(stderr, "Error deleting config files: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void organize_files(const char *directory) {
    
    char config_folder[MAX_PATH];
    snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", getenv("HOME"));
    
    load_configs(config_folder);

    bool handle_misc = check_for_uncategorized_files(directory);
    
    process_directory(directory, handle_misc);
}

// Make sure this function is already implemented or add it if it's missing
int delete_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)sb;
    (void)typeflag;
    (void)ftwbuf;
    int rv = remove(fpath);
    if (rv) {
        perror(fpath);
    }
    return rv;
}

void reload_mappings(const char *config_folder) {
    // Free existing mappings
    for (int i = 0; i < mapping_count; i++) {
        free(mappings[i].extension);
        free(mappings[i].category);
    }
    free(mappings);
    mappings = NULL;
    mapping_count = 0;

    // Reload mappings
    load_configs(config_folder);
}

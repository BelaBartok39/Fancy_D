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

int check_path_length(const char *path) {
    if (strlen(path) >= PATH_MAX) {
        fprintf(stderr, "Warning: Path exceeds maximum length (%d): %s\n", PATH_MAX, path);
        return 1;
    }
    return 0;
}

void list_extensions(const char *config_folder){
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
        if (strstr(ent->d_name, "_config.json") == NULL) {
            continue;
        }
    
        // Get file path safely (no buffer overflow) 
        snprintf(file_path, sizeof(file_path), "%s/%s", config_folder, ent->d_name);
        
        FILE *f = fopen(file_path, "r");
        if (f == NULL) {
            fprintf(stderr, "Unable to open file: %s\n", file_path);
            continue;
        }
        
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *json_str = malloc(fsize + 1);
        if (json_str == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(f);
            continue;
        }
        
        fread(json_str, fsize, 1, f);
        fclose(f);
        json_str[fsize] = 0; // set to '\0' DONT FORGET
        
        cJSON *json = cJSON_Parse(json_str);
        free(json_str);

        if (json_str == NULL){
            fprintf(stderr, "JSON parse error for file: %s\n", file_path);
            continue;
        }

        char *category = strdup(ent->d_name);
        if (category == NULL) {
            fprintf(stderr, "Memory allocation failed for category name\n");
            cJSON_Delete(json);
            continue;
        }
        
        char *underscore = strchr(category, '_');
        if (underscore != NULL) {
            *underscore = '\0'; // null-terminate underscore
        }
        
        print_blue("\nCategory: %s\n", category);
        
        cJSON *extension;
        cJSON_ArrayForEach(extension, json) {
            printf(" %s\n", extension->string);
        }

        cJSON_Delete(json);
        free(category);
    }

    closedir(dir);
    printf("\n");
}

char *create_fallback_path(const char *original_path) {
    char *fallback = malloc(PATH_MAX);
    if (!fallback) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    const char *filename = strrchr(original_path, '/');
    if (filename == NULL) {
        filename = original_path;
    } else {
        filename++; // Skip the slash
    }

    snprintf(fallback, PATH_MAX, "%s%s", FALLBACK_PREFIX, filename);
    return fallback;
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

int delete_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void)sb;       // Suppress unused parameter
    (void)typeflag; // Suppress unused parameter 
    (void)ftwbuf;   // Suppress unused parameter
    int rv = remove(fpath);
    if (rv) {
        perror(fpath);
    }
    return rv;
}


void delete_config_files(const char *config_folder) {
    if (nftw(config_folder, delete_callback, 64, FTW_DEPTH | FTW_PHYS) == -1) {
        fprintf(stderr, "Error deleting config files: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int create_default_configs(const char *config_folder) {
    printf("Creating default configs in folder: %s\n", config_folder);
    
    // Load existing configurations
    load_configs(config_folder);

    const char *default_configs[] = {
        "Documents_config.json", "{\".txt\": \"Documents\", \".doc\": \"Documents\", \".pdf\": \"Documents\"}",
        "Images_config.json", "{\".jpg\": \"Images\", \".png\": \"Images\", \".gif\": \"Images\"}",
        "Audio_config.json", "{\".mp3\": \"Audio\", \".wav\": \"Audio\", \".flac\": \"Audio\"}",
        "Video_config.json", "{\".mp4\": \"Video\", \".avi\": \"Video\", \".mkv\": \"Video\"}"
    };

    // Need to check for conflicts here.
            
    for(int i = 0; i < 8; i+=2){
        cJSON *root = cJSON_Parse(default_configs[i+1]);
        if(!root){
            printf("Error parsing JSON\n");
            return 1;
        }
        
        for (cJSON *item = root->child; item; item = item->next){
            if(check_duplicate_extension(config_folder, item->string, default_configs[i]) == 1){
                cJSON_Delete(root);
                print_red("Error: Duplicate extension found\n");
                // need to handle this case
                return 1;
            }
        }
    }

  
    // No conflicts, create default configs
    
    for (size_t i = 0; i < sizeof(default_configs) / sizeof(default_configs[0]); i += 2) {
        char file_path[MAX_PATH];
        snprintf(file_path, sizeof(file_path), "%s/%s", config_folder, default_configs[i]);
        
        printf("Attempting to create config file: %s\n", file_path);

        FILE *f = fopen(file_path, "w");
        if (f == NULL) {
            printf("Failed to create config file: %s (errno: %d)\n", file_path, errno);
            perror("Error details");
            continue;
        }
        
        fputs(default_configs[i+1], f);
        fclose(f);
        
        printf("Successfully created config file: %s\n", file_path);
    }

    // Reload configurations after changes
    load_configs(config_folder);

    printf("Finished creating default configs. Current mappings:\n");
    for (int i = 0; i < mapping_count; i++) {
        printf("%s -> %s\n", mappings[i].extension, mappings[i].category);
    }
    return 0;
}

void load_configs(const char *config_folder) {
    DIR *dir;
    struct dirent *ent;
    char file_path[MAX_PATH];

    // Free existing mappings if any
    for (int i = 0; i < mapping_count; i++) {
        free(mappings[i].extension);
        free(mappings[i].category);
    }
    free(mappings);

    mappings = malloc(sizeof(ExtensionMapping) * MAX_EXTENSIONS);
    if (!mappings) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(mappings, 0, sizeof(ExtensionMapping) * MAX_EXTENSIONS);
    mapping_count = 0;

    dir = opendir(config_folder);
    if (dir == NULL) {
        fprintf(stderr, "No configuration files found. Creating default categories.\n");
        create_default_configs(config_folder);
        dir = opendir(config_folder);
        if (dir == NULL) {
            fprintf(stderr, "Failed to create default configs.\n");
            exit(1);
        }
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, "_config.json") == NULL) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", config_folder, ent->d_name);
        
        FILE *f = fopen(file_path, "r");
        if (f == NULL) {
            fprintf(stderr, "Unable to open file: %s\n", file_path);
            continue;
        }

        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *json_str = malloc(fsize + 1);
        if (json_str == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(f);
            continue;
        }

        if (fread(json_str, fsize, 1, f) != 1) {
            fprintf(stderr, "Failed to read file content: %s\n", file_path);
            free(json_str);
            fclose(f);
            continue;
        }

        json_str[fsize] = 0;
        fclose(f);

        cJSON *json = cJSON_Parse(json_str);
        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "JSON parse error before: %s\n", error_ptr);
            }
            fprintf(stderr, "JSON parse error for file: %s\n", file_path);
            free(json_str);
            continue;
        }

        cJSON *extension;
        cJSON_ArrayForEach(extension, json) {
            if (mapping_count >= MAX_EXTENSIONS) {
                fprintf(stderr, "Too many extensions, increase MAX_EXTENSIONS\n");
                break;
            }

            const char *ext_str = extension->string;
            const char *cat_str = extension->valuestring;
            
            if (ext_str == NULL || cat_str == NULL) {
                fprintf(stderr, "Invalid JSON structure in file: %s\n", file_path);
                continue;
            }

            mappings[mapping_count].extension = strdup(ext_str);
            mappings[mapping_count].category = strdup(cat_str);

            if (mappings[mapping_count].extension == NULL || mappings[mapping_count].category == NULL) {
                fprintf(stderr, "Memory allocation failed for mapping %d\n", mapping_count);
                exit(1);
            }

            mapping_count++;
        }

        cJSON_Delete(json);
        free(json_str);
    }

    closedir(dir);
    if (verbose == 1) {
        printf("Total mappings loaded: %d\n", mapping_count);
    }
}

// Only used for debugging
void print_string_details(const char* str) {
    printf("String: '");
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%c", str[i]);
    }
    printf("', Length: %zu, ASCII values: ", strlen(str));
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%d ", (unsigned char)str[i]);
    }
    printf("\n");
}


void organize_files(const char *directory) {
    char config_folder[MAX_PATH];
    snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", getenv("HOME"));
    load_configs(config_folder);

    DIR *dir;
    struct dirent *entry;
    char file_path[MAX_PATH];
    struct stat file_stat;
    bool uncategorized_files_found = false;
    bool handle_misc = false;
    

    dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open directory: %s\n", directory);
        return;
    }

    // First pass: check for uncategorized files
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (lstat(file_path, &file_stat) != 0) {
            fprintf(stderr, "Unable to get file stats for %s\n", file_path);
            continue;
        }

        if (S_ISREG(file_stat.st_mode)) {
            char *extension = strrchr(entry->d_name, '.');
            bool categorized = false;

            if (extension != NULL) {
                for (int i = 0; i < mapping_count; i++) {
                    if (strcasecmp(mappings[i].extension, extension) == 0) {
                        categorized = true;
                        break;
                    }
                }
            }

            if (!categorized) {
                uncategorized_files_found = true;
                break;
            }
        }
    }

    // If uncategorized files are found, prompt the user
    if (uncategorized_files_found) {
        char response;
        printf("Uncategorized files found. Do you want to put them in a 'misc' folder? (y/n): ");
        scanf(" %c", &response);
        handle_misc = (response == 'y' || response == 'Y');
    }

    // Reset directory stream for second pass
    rewinddir(dir);

    // Second pass: organize files
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (lstat(file_path, &file_stat) != 0) {
            fprintf(stderr, "Unable to get file stats for %s\n", file_path);
            continue;
        }

        if (S_ISREG(file_stat.st_mode)) {
            char *extension = strrchr(entry->d_name, '.');
            char *category = NULL;

            if (extension != NULL) {
                for (int i = 0; i < mapping_count; i++) {
                    if (strcasecmp(mappings[i].extension, extension) == 0) {
                        category = mappings[i].category;
                        break;
                    }
                }
            }

            if (category == NULL && handle_misc) {
                category = "misc";
            }

            if (category != NULL) {
                char category_path[MAX_PATH];
                if ((size_t)snprintf(category_path, sizeof(category_path), "%s/%s", directory, category) >= sizeof(category_path)) {
                    fprintf(stderr, "Category path too long: %s/%s\n", directory, category);
                    continue;
                }
                mkdir(category_path, 0777);

                char dest_path[MAX_PATH];
                if ((size_t)snprintf(dest_path, sizeof(dest_path), "%s/%s", category_path, entry->d_name) >= sizeof(dest_path)) {
                    fprintf(stderr, "Destination path too long: %s/%s\n", category_path, entry->d_name);
                    continue;
                }

                if (move_file_with_fallback(file_path, dest_path) != 0) {
                    fprintf(stderr, "Failed to move %s to %s\n", file_path, dest_path);
                } else {
                    if (verbose == 1) {
                        printf("Moved %s to %s\n", entry->d_name, category);
                    }
                }
            } else {
                if (verbose == 1) { 
                    printf("Skipping uncategorized file: %s\n", entry->d_name);
                }
            }
        } else {
            if (verbose == 1) {
                printf("Skipping non-regular file: %s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
}

int check_duplicate_extension(const char *config_folder, const char *extension, const char *new_category){

    for (int i = 0; i < mapping_count; i++) {
        if (strcasecmp(mappings[i].extension, extension) == 0) {
            printf("Extension %s already exists in category %s.\n", extension, mappings[i].category);
            printf("Do you want to move it to %s? (y/n): ", new_category);
                
            char response;
            scanf(" %c", &response);

            if (response != 'y' && response != 'Y') {
                printf("Extension %s will remain in category %s.\n", extension, mappings[i].category);
                return 1;
            }

            // Remove the extension from the existing category file
            char existing_config_path[MAX_PATH];
            snprintf(existing_config_path, sizeof(existing_config_path), "%s/%s_config.json", config_folder, mappings[i].category);
            remove_extension_from_config(existing_config_path, extension);
            break;
        }
    }
    return 0;
}

void add_extension(const char *config_folder, const char *extension, const char *new_category) {
    // Load current mappings
    load_configs(config_folder);
    // Check if the extension already exists in any category

    if (check_duplicate_extension(config_folder, extension, new_category) == 1) {
        return;
    }  

    char new_config_path[MAX_PATH];
    snprintf(new_config_path, sizeof(new_config_path), "%s/%s_config.json", config_folder, new_category);

    cJSON *json = NULL;
    char *content = NULL;
    long file_size = 0;

    FILE *file = fopen(new_config_path, "r");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        content = malloc(file_size + 1);
        if (content) {
            fread(content, 1, file_size, file);
            content[file_size] = '\0';
        }
        fclose(file);

        json = cJSON_Parse(content);
        free(content);
    }

    if (json == NULL) {
        json = cJSON_CreateObject();
    }

    cJSON_AddStringToObject(json, extension, new_category);

    char *updated_content = cJSON_Print(json);
    file = fopen(new_config_path, "w");
    if (file) {
        fputs(updated_content, file);
        fclose(file);
        printf("Added extension %s to category %s\n", extension, new_category);
    } else {
        fprintf(stderr, "Failed to write updated config to %s\n", new_config_path);
    }

    cJSON_Delete(json);
    free(updated_content);

    // Reload mappings to reflect the changes
    reload_mappings(config_folder);
}

void remove_extension_from_config(const char *config_path, const char *extension) {
    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open config file: %s\n", config_path);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = malloc(file_size + 1);
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);

    cJSON *json = cJSON_Parse(content);
    free(content);

    if (json == NULL) {
        fprintf(stderr, "Failed to parse JSON in file: %s\n", config_path);
        return;
    }

    cJSON_DeleteItemFromObject(json, extension);

    if (cJSON_GetArraySize(json) == 0) {
        // If the JSON object is empty, delete the file
        cJSON_Delete(json);
        if (remove(config_path) == 0) {
            printf("Removed empty config file: %s\n", config_path);
        } else {
            fprintf(stderr, "Failed to remove empty config file: %s\n", config_path);
        }
    } else {
        char *updated_content = cJSON_Print(json);
        file = fopen(config_path, "w");
        if (file) {
            fputs(updated_content, file);
            fclose(file);
            printf("Removed extension %s from %s\n", extension, config_path);
        } else {
            fprintf(stderr, "Failed to write updated config to %s\n", config_path);
        }
        cJSON_Delete(json);
        free(updated_content);
    }
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

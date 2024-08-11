// ============================================================================= //
// Program Name: FancyD (Fancy Directory Organizer)
// Author: Nicholas D. Redmond (bAbYnIcKy)
// Date: 8/10/2024
// Description: Simple program to organize files in a directory based on their file
// ============================================================================= //

#include "fancy.h"

// What is -v verbose actually doing???

ExtensionMapping *mappings = NULL;
int mapping_count = 0;

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [DIRECTORY]\n", program_name);
    printf("Options:\n");
    printf("  -a, --add EXT CATEGORY  Add a file extension to a category\n");
    printf("  -v, --verbose       Enable verbose logging\n");
    printf("  -h, --help          Display this help message\n");
    printf("  -d, --default       Create default categories\n");
    printf("  -r, --reset         Reset categories\n");
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

// Need to implement this function or figure out why its not being called in Main
void create_default_configs(const char *config_folder) {
    const char *default_configs[] = {
        "document_config.json", "{\".txt\": \"Documents\", \".doc\": \"Documents\", \".pdf\": \"Documents\"}",
        "image_config.json", "{\".jpg\": \"Images\", \".png\": \"Images\", \".gif\": \"Images\"}",
        "audio_config.json", "{\".mp3\": \"Audio\", \".wav\": \"Audio\", \".flac\": \"Audio\"}",
        "video_config.json", "{\".mp4\": \"Video\", \".avi\": \"Video\", \".mkv\": \"Video\"}"
    };
    
    for (size_t i = 0; i < sizeof(default_configs) / sizeof(default_configs[0]); i += 2) {
        char file_path[MAX_PATH];
        snprintf(file_path, sizeof(file_path), "%s/%s", config_folder, default_configs[i]);
        
        FILE *f = fopen(file_path, "w");
        if (f == NULL) {
            fprintf(stderr, "Unable to create default config file: %s\n", file_path);
            continue;
        }
        
        fputs(default_configs[i+1], f);
        fclose(f);
    }
}

void load_configs(const char *config_folder) {
    DIR *dir;
    struct dirent *ent;
    char file_path[MAX_PATH];

    mappings = malloc(sizeof(ExtensionMapping) * MAX_EXTENSIONS);
    if (!mappings) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    memset(mappings, 0, sizeof(ExtensionMapping) * MAX_EXTENSIONS);  // Initialize to zero

    dir = opendir(config_folder);
    if (dir == NULL) {
        fprintf(stderr, "No configuration files found. Creating default categories.\n");
        exit(1);
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
      
            // OPTIONAL INFO

            // printf("Loaded mapping:\n");
            // print_string_details(mappings[mapping_count].extension);
            // print_string_details(mappings[mapping_count].category);

            mapping_count++;
        }

        cJSON_Delete(json);
        free(json_str);
    }

    closedir(dir);
    printf("Total mappings loaded: %d\n", mapping_count);
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
    DIR *dir;
    struct dirent *entry;
    char file_path[MAX_PATH];
    char dest_path[MAX_PATH];
    struct stat file_stat;

    dir = opendir(directory);
    if (dir == NULL) {
        fprintf(stderr, "Unable to open directory: %s\n", directory);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (stat(file_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            char *extension = strrchr(entry->d_name, '.');
            if (extension != NULL) {
                // Do not increment extension or the world will end
        
                // printf("Processing file: %s with extension: %s\n", entry->d_name, extension);  // *DEBUG
                // print_string_details(extension); // *DEBUG       

                char *category = NULL;

                for (int i = 0; i < mapping_count; i++) {
                    // printf("Comparing '%s' with mapping: '%s' -> '%s'\n", extension, mappings[i].extension, mappings[i].category);  // *DEBUG
                    int cmp_result = strcasecmp(mappings[i].extension, extension);
                    // printf("strcasecmp result: %d\n", cmp_result);  // *DEBUG Print the result of strcasecmp
                    if (cmp_result == 0) {
                        category = mappings[i].category;
                        // printf("Match found! Category: %s\n", category); *OPTIONAL INFO
                        break;
                    }
                }

                if (category == NULL) {
                    category = "misc";
                    printf("No match found, using misc category\n");
                }

                char category_path[MAX_PATH];
                snprintf(category_path, sizeof(category_path), "%s/%s", directory, category);
                mkdir(category_path, 0777);

                snprintf(dest_path, sizeof(dest_path), "%s/%s", category_path, entry->d_name);
                if (rename(file_path, dest_path) != 0) {
                    fprintf(stderr, "Failed to move %s to %s\n", file_path, dest_path);
                } else {
                    printf("Moved %s to %s\n", entry->d_name, category);
                }
            }
        }
    }

    closedir(dir);
}

void add_extension(const char *config_folder, const char *extension, const char *category) {
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s/%s_config.json", config_folder, category);

    FILE *file = fopen(config_path, "r");
    char *content = NULL;
    long file_size = 0;

    if (file) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        content = malloc(file_size + 1);
        if (content) {
            if (fread(content, 1, file_size, file) != file_size) {
                fprintf(stderr, "Failed to read file content: %s\n", config_path);
                free(content);
                fclose(file);
                return;
            }
            content[file_size] = '\0';
        }
        fclose(file);
    }

    cJSON *json = content ? cJSON_Parse(content) : cJSON_CreateObject();
    free(content);

    if (json == NULL) {
        fprintf(stderr, "Failed to parse config file or create new JSON object\n");
        return;
    }

    char *lowercase_ext = strdup(extension);
    for (int i = 0; lowercase_ext[i]; i++) {
        lowercase_ext[i] = tolower(lowercase_ext[i]);
    }

    if (cJSON_HasObjectItem(json, lowercase_ext)) {
        fprintf(stderr, "Extension %s already exists in category %s\n", lowercase_ext, category);
        cJSON_Delete(json);
        free(lowercase_ext);
        return;
    }

    cJSON_AddStringToObject(json, lowercase_ext, category);

    char *updated_content = cJSON_Print(json);
    file = fopen(config_path, "w");
    if (file) {
        fputs(updated_content, file);
        fclose(file);
        printf("Added extension %s to category %s\n", lowercase_ext, category);
    } else {
        fprintf(stderr, "Failed to write updated config to %s\n", config_path);
    }

    cJSON_Delete(json);
    free(updated_content);
    free(lowercase_ext);
}

void segfault_handler(int signal) {
    (void)signal; // Suppress unused parameter warning
    fprintf(stderr, "Segmentation fault caught. Exiting...\n");
    exit(1);
}

                                  // PROGRAM START
int main(int argc, char *argv[]) {
    signal(SIGSEGV, segfault_handler);
  
    int verbose = 0;
    char *directory = ".";
    char *extension = NULL;
    char *category = NULL;

    // Initialize config_folder at the beginning
    char config_folder[MAX_PATH];
    const char *home = getenv("HOME");
    if (home) {
        snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", home);
    } else {
        fprintf(stderr, "Unable to determine home directory\n");
        return 1;
    }

    static struct option long_options[] = {
        {"add", required_argument, 0, 'a'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"default", no_argument, 0, 'd'},
        {"reset", no_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "va:hdr", long_options, &option_index)) != -1) {
        switch (opt) {
            
            case 'v':
                verbose = 1;
                break;
            case 'a':
                if (optind < argc) {
                    extension = optarg;
                    category = argv[optind++];
                } else {
                    fprintf(stderr, "Error: --add requires two arguments\n");
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'd':
                ensure_config_folder(config_folder);
                create_default_configs(config_folder);
                printf("Default categories created\n");
                return 0;
            case 'r':
                ensure_config_folder(config_folder);
                printf("Resetting configuration files...\n");
                delete_config_files(config_folder);
                printf("Configuration files have been reset\n");
                return 0;
            default:
                fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                return 1;
        }
    }

    if (optind < argc) {
        directory = argv[optind];
    }


    ensure_config_folder(config_folder);
    
    if (extension && category) {
        add_extension(config_folder, extension, category);
    } else {
        // Check if any config files exist
        DIR *dir = opendir(config_folder);
        struct dirent *ent;
        int config_count = 0;
        if (dir != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strstr(ent->d_name, "_config.json") != NULL) {
                    config_count++;
                    break;
                }
            }
            closedir(dir);
        }

        if (config_count == 0) {
            char response;
            printf("There are no categories added. Do you want to put everything in 'misc'? (y/n): ");
            if (scanf(" %c", &response) != 1) {
                fprintf(stderr, "Failed to read user input\n");
                return 1;
            }
            if (response == 'y' || response == 'Y') {
                // Create a misc category
                char misc_config_path[MAX_PATH];
                snprintf(misc_config_path, sizeof(misc_config_path), "%s/misc_config.json", config_folder);
                FILE *misc_file = fopen(misc_config_path, "w");
                if (misc_file) {
                    fprintf(misc_file, "{\n  \"*\": \"misc\"\n}");
                    fclose(misc_file);
                    printf("Created 'misc' category for all files.\n");
                } else {
                    fprintf(stderr, "Failed to create 'misc' category.\n");
                    return 1;
                }
            } else {
                printf("No categories available. Use --default to set up categories.\n");
                return 0;
            }
        }

        load_configs(config_folder);
        organize_files(directory);
    }

    // Free allocated memory
    for (int i = 0; i < mapping_count; i++) {
        free(mappings[i].extension);
        free(mappings[i].category);
    }
    free(mappings);

    return 0;
}

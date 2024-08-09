#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include "cJSON.h"

#define MAX_PATH 4096
#define MAX_EXTENSIONS 1000

typedef struct {
    char *extension;
    char *category;
} ExtensionMapping;

ExtensionMapping *mappings = NULL;
int mapping_count = 0;

// TODO
// organize_files function
// add_extension function

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS] [DIRECTORY]\n", program_name);
    printf("Options:\n");
    printf("  --extreme           Enable extreme sorting by individual extensions\n");
    printf("  --add EXT CATEGORY  Add a file extension to a category\n");
    printf("  -v, --verbose       Enable verbose logging\n");
    printf("  --uninstall         Uninstall Fancy Directory Sort\n");
    printf("  -h, --help          Display this help message\n");
}


int main(int argc, char *argv[]) {
    int extreme_sort = 0;
    int verbose = 0;
    char *directory = ".";
    char *extension = NULL;
    char *category = NULL;

    static struct option long_options[] = {
        {"extreme", no_argument, 0, 'e'},
        {"add", required_argument, 0, 'a'},
        {"verbose", no_argument, 0, 'v'},
        {"uninstall", no_argument, 0, 'u'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "eva:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'e':
                extreme_sort = 1;
                break;
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
            case 'u':
                // Implement uninstall functionality
                printf("Uninstall not implemented yet\n");
                return 0;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
                return 1;
        }
    }

    if (optind < argc) {directory = argv[optind];}

    // TODO: Implement main functionality
    if (extension && category) {
        // add_extension(config_folder, extension, category);
    } else {
        // organize_files(directory, extreme_sort);
    }

    return 0;
}


void organize_files(const char *directory, int extreme_sort) {
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
        // Skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);

        if (stat(file_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
            char *extension = strrchr(entry->d_name, '.');
            if (extension != NULL) {
                extension++; // Move past the dot
                char *category = NULL;

                // Find the category for this extension
                for (int i = 0; i < mapping_count; i++) {
                    if (strcasecmp(mappings[i].extension, extension) == 0) {
                        category = mappings[i].category;
                        break;
                    }
                }

                if (category == NULL) {
                    category = "misc";
                }

                // Create category directory if it doesn't exist
                char category_path[MAX_PATH];
                snprintf(category_path, sizeof(category_path), "%s/%s", directory, category);
                mkdir(category_path, 0777);

                // Move file to category directory
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

    // Read existing config file
    FILE *file = fopen(config_path, "r");
    char *content = NULL;
    long file_size = 0;

    if (file) {
        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        content = malloc(file_size + 1);
        if (content) {
            fread(content, 1, file_size, file);
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

    // Convert extension to lowercase
    char *lowercase_ext = strdup(extension);
    for (int i = 0; lowercase_ext[i]; i++) {
        lowercase_ext[i] = tolower(lowercase_ext[i]);
    }

    // Check if extension already exists
    if (cJSON_HasObjectItem(json, lowercase_ext)) {
        fprintf(stderr, "Extension %s already exists in category %s\n", lowercase_ext, category);
        cJSON_Delete(json);
        free(lowercase_ext);
        return;
    }

    // Add new extension
    cJSON_AddStringToObject(json, lowercase_ext, category);

    // Write updated JSON back to file
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
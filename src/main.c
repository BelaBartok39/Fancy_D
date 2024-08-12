// ============================================================================= //
// Program Name: FancyD (Fancy Directory Organizer)
// Author: Nicholas D. Redmond (bAbYnIcKy)
// Date: 8/10/2024
// Description: Simple program to organize files in a directory based on their file
// ============================================================================= //


#include <fancy.h>

void segfault_handler(int signal) {
    (void)signal; // Suppress unused parameter warning
    fprintf(stderr, "Segmentation fault caught. Exiting...\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, segfault_handler);
  
    int verbose = 0;
    char *directory = ".";
    char *extension = NULL;
    char *category = NULL;

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

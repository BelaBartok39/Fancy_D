// ============================================================================= //
// Program Name: FancyD (Fancy Directory Organizer)
// Author: Nicholas D. Redmond (6a6yN1cKy)
// Date: 8/13/2024
// Description: Simple program to organize files in a directory based on their file
// ============================================================================= //

#include <fancy.h>
#include <color_utils.h>
#include <utils.h>

void segfault_handler(int signal) {
    (void)signal; // Suppress unused parameter warning
    print_red("Segmentation fault caught. Exiting...\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    signal(SIGSEGV, segfault_handler);
  
    char *directory = ".";
    char *extension = NULL;
    char *category = NULL;

    char config_folder[MAX_PATH];
    const char *home = getenv("HOME");
    if (home) {
        snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", home);
    } else {
        print_red("Unable to determine home directory\n");
        return 1;
    }

    static struct option long_options[] = {
        {"add", required_argument, 0, 'a'},
        {"list", no_argument, 0, 'l'},
        {"help", no_argument, 0, 'h'},
        {"default", no_argument, 0, 'd'},
        {"reset", no_argument, 0, 'r'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "a:hdrl", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                if (optind < argc) {
                    extension = optarg;
                    category = argv[optind++];
                } else {
                    print_red("Error: --add requires two arguments\n");
                    return 1;
                }
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'd':
                ensure_config_folder(config_folder);
                create_default_configs(config_folder);
                print_green("Default categories created\n");
                return 0;
            case 'r':
                ensure_config_folder(config_folder);
                print_yellow("Resetting configuration files...\n");
                delete_config_files(config_folder);
                print_green("Configuration files have been reset\n");
                return 0;
            case 'l':
                list_extensions(config_folder);
                return 0;
            default:
                print_yellow("Try '%s --help' for more information.\n", argv[0]);
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
            print_yellow("There are no categories added. Do you want to put everything in 'misc'? (y/n): ");
            if (scanf(" %c", &response) != 1) {
                print_red("Failed to read user input\n");
                return 1;
            }
            if (response == 'y' || response == 'Y') {
                // Create a misc category
                char* misc_config_path = safe_path_join(config_folder, "misc_config.json");
                if (misc_config_path == NULL) {
                    fprintf(stderr, "Failed to construct path for misc_config.json\n");
                    return 1;
                }
// Use misc_config_path...

                FILE *misc_file = fopen(misc_config_path, "w");
                if (misc_file) {
                    fprintf(misc_file, "{\n  \"*\": \"misc\"\n}");
                    fclose(misc_file);
                    free(misc_config_path);
                    print_green("Created 'misc' category for all files.\n");
                } else {
                    print_red("Failed to create 'misc' category.\n");
                    free(misc_config_path);
                    return 1;
                }
            } else {
                print_yellow("No categories available. Use --default to set up categories.\n");
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


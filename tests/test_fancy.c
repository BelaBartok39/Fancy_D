#define _POSIX_C_SOURCE 200809L
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "../src/fancy.h"

// Helper function to create a temporary directory
char* create_temp_dir() {
    char template[] = "/tmp/fancyD_test_XXXXXX";
    char* temp_dir = mkdtemp(template);
    if (temp_dir == NULL) {
        perror("mkdtemp");
        ck_abort_msg("Failed to create temporary directory");
    }
    return strdup(temp_dir);
}

// ... (existing includes and setup)

// Add this new test function
START_TEST(test_add_extension_and_move)
{
    char *test_dir = create_temp_dir();
    char config_folder[MAX_PATH];
    snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", test_dir);

    // Ensure config folder exists
    ensure_config_folder(config_folder);

    // Add a custom extension
    add_extension(config_folder, ".png", "Images");

    // Check if the extension was added correctly
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s/Images_config.json", config_folder);
    
    FILE *file = fopen(config_path, "r");
    ck_assert_ptr_nonnull(file);
    
    char content[256];
    size_t read = fread(content, 1, sizeof(content), file);
    fclose(file);
    content[read] = '\0';
    
    ck_assert_str_eq(content, "{\n\t\".png\":\t\"Images\"\n}");

    // Now try to move the extension to a different category
    // We need to simulate user input for this test
    FILE *input = fmemopen("y", 1, "r");
    FILE *old_stdin = stdin;
    stdin = input;

    add_extension(config_folder, ".png", "Photos");

    stdin = old_stdin;
    fclose(input);

    // Check if the extension was moved correctly
    snprintf(config_path, sizeof(config_path), "%s/Photos_config.json", config_folder);
    
    file = fopen(config_path, "r");
    ck_assert_ptr_nonnull(file);
    
    read = fread(content, 1, sizeof(content), file);
    fclose(file);
    content[read] = '\0';
    
    ck_assert_str_eq(content, "{\n\t\".png\":\t\"Photos\"\n}");

    // Check if the extension was removed from the old category
    snprintf(config_path, sizeof(config_path), "%s/Images_config.json", config_folder);
    
    file = fopen(config_path, "r");
    ck_assert_ptr_null(file);  // File should not exist

    // Clean up
    delete_config_files(config_folder);
    rmdir(config_folder);
    rmdir(test_dir);
}
END_TEST

START_TEST(test_add_extension_and_sort)
{
    // Create a temporary directory for the test
    char *test_dir = create_temp_dir();
    
    // Set up the config folder path
    char config_folder[MAX_PATH];
    snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", test_dir);

    // Set the HOME environment variable to our test directory
    setenv("HOME", test_dir, 1);

    // Ensure config folder exists
    ensure_config_folder(config_folder);

    // Add a custom extension
    add_extension(config_folder, ".xyz", "TestCategory");

    // Create a test file with the new extension
    char test_file_path[MAX_PATH];
    snprintf(test_file_path, sizeof(test_file_path), "%s/testfile.xyz", test_dir);
    FILE *test_file = fopen(test_file_path, "w");
    ck_assert_ptr_nonnull(test_file);
    fclose(test_file);

    // Run the organize_files function
    organize_files(test_dir);

    // Check if the file was moved to the correct category
    char sorted_file_path[MAX_PATH];
    snprintf(sorted_file_path, sizeof(sorted_file_path), "%s/TestCategory/testfile.xyz", test_dir);
    
    struct stat st = {0};
    int result = stat(sorted_file_path, &st);
    
    // If the file is not found, print debug information
    if (result != 0) {
        printf("Debug: File not found at expected location: %s\n", sorted_file_path);
        
        // Check if the file is still in the original location
        result = stat(test_file_path, &st);
        if (result == 0) {
            printf("Debug: File is still in the original location\n");
        } else {
            printf("Debug: File is not in the original location either\n");
        }
        
        // List contents of the test directory
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(test_dir)) != NULL) {
            printf("Debug: Contents of test directory:\n");
            while ((ent = readdir(dir)) != NULL) {
                printf("%s\n", ent->d_name);
            }
            closedir(dir);
        }
    }

    ck_assert_msg(result == 0, "File was not moved to TestCategory. stat() returned %d", result);

    // Clean up
    remove(sorted_file_path);
    char config_file_path[MAX_PATH];
    snprintf(config_file_path, sizeof(config_file_path), "%s/.fancyD/newcategory_config.json", test_dir);
    remove(config_file_path);
    rmdir(config_folder);
    rmdir(test_dir);

    // Reset HOME environment variable
    unsetenv("HOME");
}
END_TEST

// Test ensure_config_folder function
START_TEST(test_ensure_config_folder)
{
    char* test_path = create_temp_dir();
    ensure_config_folder(test_path);
    struct stat st = {0};
    int result = stat(test_path, &st);
    ck_assert_int_eq(result, 0);
    ck_assert(S_ISDIR(st.st_mode));
    rmdir(test_path);
    free(test_path);
}
END_TEST

// Test create_default_configs function
START_TEST(test_create_default_configs)
{
    char* test_path = create_temp_dir();
    create_default_configs(test_path);
    
    // Check if default config files were created
    char file_path[MAX_PATH];
    snprintf(file_path, sizeof(file_path), "%s/document_config.json", test_path);
    ck_assert_int_eq(access(file_path, F_OK), 0);
    
    // Check content of a config file
    FILE* file = fopen(file_path, "r");
    ck_assert_ptr_nonnull(file);
    char content[1024];
    size_t read = fread(content, 1, sizeof(content), file);
    fclose(file);
    ck_assert_int_gt(read, 0);
    ck_assert_str_ne(content, "");

    // Clean up
    remove(file_path);
    rmdir(test_path);
    free(test_path);
}
END_TEST

// Test load_configs function
START_TEST(test_load_configs_function)
{
    char* test_path = create_temp_dir();
    create_default_configs(test_path);
    load_configs(test_path);
    
    // Check if mappings were loaded
    ck_assert_int_gt(mapping_count, 0);
    ck_assert_ptr_nonnull(mappings);

    // Check a specific mapping
    bool found_txt = false;
    for (int i = 0; i < mapping_count; i++) {
        if (strcmp(mappings[i].extension, ".txt") == 0) {
            found_txt = true;
            ck_assert_str_eq(mappings[i].category, "Documents");  // Changed from "Text" to "Documents"
            break;
        }
    }
    ck_assert(found_txt);

    // Clean up
    rmdir(test_path);
    free(test_path);
}
END_TEST

// Test add_extension function
START_TEST(test_add_extension)
{
    char* test_path = create_temp_dir();
    create_default_configs(test_path);
    
    add_extension(test_path, ".xyz", "NewCategory");
    
    // Reload configs and check if new extension is added
    load_configs(test_path);
    bool found_xyz = false;
    for (int i = 0; i < mapping_count; i++) {
        if (strcmp(mappings[i].extension, ".xyz") == 0) {
            found_xyz = true;
            ck_assert_str_eq(mappings[i].category, "NewCategory");
            break;
        }
    }
    ck_assert(found_xyz);

    // Clean up
    rmdir(test_path);
    free(test_path);
}
END_TEST

// Test organize_files function
START_TEST(test_organize_files)
{
    char *test_dir = create_temp_dir();
    
    // Set up the config folder path
    char config_folder[MAX_PATH];
    snprintf(config_folder, sizeof(config_folder), "%s/.fancyD", test_dir);

    // Set the HOME environment variable to our test directory
    setenv("HOME", test_dir, 1);

    // Ensure config folder exists and create default configs
    ensure_config_folder(config_folder);
    create_default_configs(config_folder);

    // Create test files
    char file_path[MAX_PATH];
    snprintf(file_path, sizeof(file_path), "%s/test.txt", test_dir);
    FILE *file = fopen(file_path, "w");
    fclose(file);
    
    snprintf(file_path, sizeof(file_path), "%s/test.jpg", test_dir);
    file = fopen(file_path, "w");
    fclose(file);
    
    // Organize files
    organize_files(test_dir);

    // Check if the files were moved to correct categories
    snprintf(file_path, sizeof(file_path), "%s/Documents/test.txt", test_dir);
    ck_assert_int_eq(access(file_path, F_OK), 0);
    
    snprintf(file_path, sizeof(file_path), "%s/Images/test.jpg", test_dir);
    ck_assert_int_eq(access(file_path, F_OK), 0);

    // Clean up
    delete_config_files(config_folder);
    rmdir(config_folder);
    rmdir(test_dir);

    // Reset HOME environment variable
    unsetenv("HOME");
}
END_TEST

Suite * fancy_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("FancyD");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_ensure_config_folder);
    tcase_add_test(tc_core, test_create_default_configs);
    tcase_add_test(tc_core, test_load_configs_function);
    tcase_add_test(tc_core, test_add_extension);
    tcase_add_test(tc_core, test_organize_files);
    tcase_add_test(tc_core, test_add_extension_and_sort);
    tcase_add_test(tc_core, test_add_extension_and_move);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = fancy_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

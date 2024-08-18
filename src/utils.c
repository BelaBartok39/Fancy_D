/* =============================================================================
   Program Name: FancyD (Fancy Directory Organizer)
   Author: Nicholas D. Redmond (3A3YN1CKY)
   Date: 8/13/2024
   Description: Simple program to organize files in a directory based on their ext
   ============================================================================= */

#include "utils.h"

char* safe_path_join(const char* dir, const char* file) {
    size_t needed = snprintf(NULL, 0, "%s/%s", dir, file) + 1;
    if (needed > MAX_PATH) {
        fprintf(stderr, "Error: Path too long: %s/%s\n", dir, file);
        return NULL;
    }
    char* result = malloc(needed);
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    snprintf(result, needed, "%s/%s", dir, file);
    return result;
}

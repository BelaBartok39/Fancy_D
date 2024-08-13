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

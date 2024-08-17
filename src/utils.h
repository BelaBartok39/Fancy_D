// ============================================================================= //
// Program Name: FancyD (Fancy Directory Organizer)
// Author: Nicholas D. Redmond (3A3YN1CKY)
// Date: 8/13/2024
// Description: Simple program to organize files in a directory based on their ext
// ============================================================================= /

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fancy.h>

char* safe_path_join(const char* dir, const char* file);

#endif // UTILS_H


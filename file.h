#include <stdint.h>
#include <malloc.h>
#include <cstdlib>
#include <string.h>
#include <iostream>

typedef enum {
	VFS_FILE_TEXT
} VFS_FILE_TYPE;

typedef struct {
	VFS_FILE_TYPE filetype; //filetype ^^^^ 
	char* name; //filename

	char* bytes; //bytes in file
	uint32_t size; //size in bytes
} VFS_File;

VFS_File* new_text_file(const char* name, const char* text);
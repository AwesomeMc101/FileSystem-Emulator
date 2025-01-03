#include "file.h"

const unsigned short txt_len = strlen(".txt");

VFS_File* new_text_file(const char* name, const char* text) {
	VFS_File* newfile = static_cast<VFS_File*>(malloc(sizeof(VFS_File)));

	newfile->name = (char*)malloc(sizeof(char) * (strlen(name) + txt_len + 1));
	strcpy(newfile->name, name); //set file name
	strcpy(newfile->name + strlen(name), ".txt");
	newfile->name[strlen(name) + txt_len] = '\0';

	newfile->size = strlen(text);
	if (!newfile->size) {
		free(newfile);
		printf("oops");
		return nullptr;
	}

	newfile->bytes = static_cast<char*>(malloc(sizeof(char) * newfile->size)); //OK so technically its not a string. wtv, Yes it is
	memcpy(newfile->bytes, text, sizeof(char) * (newfile->size+1));
	newfile->bytes[newfile->size] = '\0';
	return newfile;
}
#include <iostream>
#include <string>

#include "file.h"

#include <Windows.h>
#include <vector>
#include <sstream>

VFS_File* text_edit_new_file(std::string filename = "", std::vector<std::string> full_buf = {""});

void edit_existing_file(VFS_File*);
#include <iostream>

/* Allow users to create folders and files. We want all data stored in one .data file. Probably with a header
outlining everything. */
#include "saveload.hpp"
#include <string>

void display(VFS_Folder* curr_folder) {
	//Find original folder.
	VFS_Folder* c_fold = curr_folder;
	std::vector<std::string> folders;
	while (c_fold->request_parent() != nullptr) {
		folders.push_back(c_fold->request_name());
		c_fold = c_fold->request_parent();
	} 

	//Add root as its skipped
	folders.push_back(c_fold->request_name());
	//Traverse vector backwards
	std::string fulldir;
	for (int i = folders.size() - 1; i >= 0; i--) {
		fulldir.append(folders[i] + "\\");
	}

	system("cls");
	std::cout << "Current directory -> " << fulldir << std::endl;
	std::cout << "\n\nAvailable directories: \n";
	VFS_Folder** included_children = curr_folder->get_children();
	for (int d = 0; d < curr_folder->child_count(); d++) {
		std::cout << (d+1) << ". " << (*(included_children + d))->request_name() << "\\" << std::endl;
	}

	std::cout << "\n\nFiles in \\" << folders[0] << "\\\n";
	VFS_File** included_files = curr_folder->get_files();
	for (int f = 0; f < curr_folder->file_count(); f++) {
		std::cout << f << ". " << (*(included_files + f))->name << std::endl;
	}
}

int main() {

	int r = 1;
	VFS_Folder* root = load_data(r);
	 created_folders = r; //bc for whatever reason it cant edit
	 
	VFS_Folder* cd = root;
	while (1) {
		display(cd);
		std::cout << "\n\nCommands:\ncd - Change directory\nnew - New text file\nnf - New folder\nread - Read file in cd\nedit - Edit file in cd\nsave - save data\n";
		//std::cout << "\n" << cd << " " << root << " " << kid1 << " " << kid2 << "\n";
		putchar('>');
		putchar(' ');
		std::string cmd;
		std::getline(std::cin, cmd);
		if (cmd == "cd") {
			std::cout << "Which folder #? (0 for parent) ";
			int f;
			std::cin >> f;
			if (f > cd->child_count()) { continue; }

			if (f == 0) {
				cd = (is_root_folder(cd) ? cd : cd->request_parent());
			}
			else {
				cd = *(cd->get_children() + f - 1); //we print with 1 extra lol. Child 0 is printed as 1. Therefore we can intake 0 for parent
				
			}
		}
		else if (cmd == "new") {
			VFS_File* new_file = text_edit_new_file();
			cd->push_file(new_file);
		}
		else if (cmd == "nf") {
			std::string n;
			std::cout << "Name? ";
			std::getline(std::cin, n);
			VFS_Folder* nf = new VFS_Folder(n.c_str(), cd); //set cd to parent :)
			
		}
		else if (cmd == "read") {
			std::cout << "Which file #? ";
			int f;
			std::cin >> f;
			if (f > cd->file_count()) { 
				std::cout << f << " > " << cd->file_count();
				continue; 
			}

			VFS_File* reader = *(cd->get_files() + f);
			std::cout << "\n\nReading: " << reader->name << "\n\n";
			std::cout << reader->bytes << std::endl;
			std::cout << "\nPRESS [ENTER] TO CONTINUE\n";
			std::cin.get(); //nobody can ever help me understand why you need two for it to wait
			std::cin.get();
		}
		else if (cmd == "edit") {
			std::cout << "Which file #? ";
			int f;
			std::cin >> f;
			if (f > cd->file_count()) {
				std::cout << f << " > " << cd->file_count();
				abort();
				continue;
			}

			VFS_File* reader = *(cd->get_files() + f);
			edit_existing_file(reader);
		}
		else if (cmd == "save") {
			save_data(root);
		}
	}
	

	return 0;
}
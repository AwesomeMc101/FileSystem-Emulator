#define _CRT_SECURE_NO_WARNINGS
#include "text_editor.hpp"
#include <vector>

class VFS_Folder;

#define is_root_folder(n) static_cast<bool>(n->request_parent() == nullptr)

static uint16_t created_folders = 1;

/* This would be a struct if I didn't get 200 errors when I made it one btw. */
class VFS_Folder {
private:
	char* name = (char*)"Unnamed Folder"; //name of folder

	VFS_Folder* parent = nullptr; //POINTER, set to nullptr if root

	/* VFS_FILE && VFS_FOLDER - WHY DUAL ARRAY? 
	
	We need to store an array of pointers to files, not array of files. Prevents unnecessary copying and memloss. (Probably)
	Basically means I don't have to write &file[0] or something when we edit, read, or cd.

	*/

	VFS_File** included_files = nullptr; //ARRAY, 
	uint32_t included_files_count = 0;

	VFS_Folder** children = nullptr; //ARRAY, set to nullptr if final
	uint32_t children_count = 0;

	uint16_t unique_id = 0;
public:
	//FINE! I got lazy and wrote constructors in a header. sue me
	VFS_Folder(char* n, VFS_Folder* p = nullptr) {
		name = (char*)malloc(sizeof(char) * strlen(n));
		strcpy(name, n);
		parent = p; //Autoset to nullptr lol
		if (p != nullptr) {
			//p->children
			p->push_child(this);
		}

		included_files = nullptr;
		children = nullptr;

		unique_id = created_folders;
		created_folders++;
	}
	VFS_Folder(const char* n, VFS_Folder* p = nullptr) {
		name = (char*)malloc(sizeof(char) * strlen(n));
		strcpy(name, n);
		printf("lmao");
		parent = p; //Autoset to nullptr lol
		if (p != nullptr) {
			//p->children
			p->push_child(this);
		}

		included_files = nullptr;
		children = nullptr;

		unique_id = created_folders;
		created_folders++;
	}

	void alt_init();

	VFS_Folder* request_parent();
	char* request_name();
	void set_parent(VFS_Folder*);

	uint32_t child_count();
	uint32_t file_count();

	VFS_Folder** get_children();
	VFS_File** get_files();

	/* Normal Functions */
	void modify_name(char* n, bool nc = 0);

	/* Memory Utility Functions */
	[[deprecated]]
	bool expand_files(uint32_t by_what, VFS_File** files_to_add);
	[[deprecated]]
	bool expand_children(uint32_t by_what, VFS_Folder** children_to_add);

	bool push_child(VFS_Folder*);

	void set_unique_id(uint16_t id = 0);
	uint16_t get_unique_id() {
		return unique_id;//fuc
	}

	/* File ops */
	void push_file(VFS_File*);
};


//Alternate Constructors for Different Purposes
VFS_Folder* create_child_folder(char* n, VFS_Folder* p); //No auto nullptr

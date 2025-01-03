#include "folder.h"
void VFS_Folder::alt_init() { //Alt init for when we use malloc. Yeah, i prefer using malloc cuz it looks cooler btw.
	children_count = 0; 
	included_files_count = 0;

	/* I prefer to not free uninitialized pointers. */

	included_files = nullptr;
	children = nullptr;

	name = nullptr;
}

//If you dk this go relearn C++
/* NOCHECK for when we load from files ONLY. Prevents freeing uninitialized pointers for whatever reason they could be still uninit. */
void VFS_Folder::modify_name(char* n, bool NOCHECK) {
	if (!NOCHECK && name != nullptr) {
		free(name);
	}
	name = (char*)malloc(sizeof(char) * (strlen(n)+1));
	memcpy(name, n, strlen(n) * sizeof(char));
	name[strlen(n)] = '\0';
}

//Return parent of folder. Root will return nullptr upon invoking this.
VFS_Folder* VFS_Folder::request_parent() {
//	std::cout << "\nCall " << this << " RP -> " << parent;
	//std::cout << parent << "\n";
	return parent;
}

//Yeah, probs didnt need these vars to be private
char* VFS_Folder::request_name() {
	return name;
}
void VFS_Folder::set_parent(VFS_Folder* np) {
	parent = np;
}
uint32_t VFS_Folder::child_count() {
	return children_count;
}
uint32_t VFS_Folder::file_count() {
	return included_files_count;
}
VFS_Folder** VFS_Folder::get_children() {
	return children;
}
VFS_File** VFS_Folder::get_files() {
	return included_files;
}

//B-but, realloc()! 
bool VFS_Folder::push_child(VFS_Folder* nc) {
	//I should probably add a failsafe here instead of hoping children_count is initialized...
	//Impossible tho!
	//isUnitializedPointer() lmao
	VFS_Folder** nvfs = (VFS_Folder**)malloc(sizeof(VFS_Folder*) * (children_count + 1));
	memcpy(nvfs, children, sizeof(VFS_Folder*) * children_count);
	nvfs[children_count] = nc;
	free(children);
	children = nvfs;
	children_count++;
	return 1;
}


void VFS_Folder::set_unique_id(uint16_t id) {
	if (id != 0) { //This is only true when we are loading from saved data. 
		unique_id = id;
		return;
	}

	//This all executes during reg
	if (unique_id != 0) {
		printf("CANT SET NEW UNIQUE ID");
		abort();
	}
	//True during automatic creation
	unique_id = created_folders;
	created_folders++;
	
	
}

/* Alt constructors */
//Looking back, no idea why I wrote this. It's genuinely just the constructor
VFS_Folder* create_child_folder(char* n, VFS_Folder* p) {
	VFS_Folder* nfolder = static_cast<VFS_Folder*>(malloc(sizeof(VFS_Folder))); //Yay!
	nfolder->alt_init();
	nfolder->modify_name(n);
	nfolder->set_parent(p);
	p->push_child(nfolder);
	nfolder->set_unique_id();
	return nfolder;
}

void VFS_Folder::push_file(VFS_File* file) {
	//Yes, an actually "safe" function (cap asf)
	//Explanation for why array of pointers at var declaration 
	VFS_File** nvfs = (VFS_File**)malloc(sizeof(VFS_File*) * (included_files_count + 1));
	if (included_files_count) {
		memcpy(nvfs, included_files, sizeof(VFS_File*) * included_files_count);
	}
	nvfs[included_files_count] = file;
	if (included_files_count) {
		free(included_files);
	}
	included_files = nvfs;
	included_files_count++;

}
#include "saveload.hpp"

namespace EncodeDecodeText{

	//Create an encoding of uniformed bytes with 3 numbers for each!
	void simpleEncode(std::string& fbuf, std::string data) {
		for (int j = 0; j < data.size(); j++) {
			fbuf.append(std::to_string(
				static_cast<int>(
					100 +
					(data[j] * 2)
					)
			));
		}
	}
	
	//Decode
	std::string simpleRead(std::string line) {
		size_t p = line.find_first_of('|');
		std::string L = line.substr(p + strlen("|"));

		std::string read_data;

		for (int i = 0; i < L.size(); i += 3) {
			int fd = ((100 * (L[i] - '0')) + (10 * (L[i + 1] - '0')) + (L[i + 2] - '0'));
			//MM yes wtf? Looks cleaner than some weird substring buddy.
			//Digit 0 -> *= 100 to make x00
			//Digit 1 -> *= 10 to make xy0
			//Digit 2 -> *= 1 to make xyz

			fd -= 100; //sub the 100 that we add to ensure 3 digits 
			read_data.push_back(static_cast<char>(fd / 2)); //divide by 2 cuz i decided to make it mult by 2
		}
		return read_data;
	}

}

static void write_folder_files(std::string& full_buff, VFS_Folder* F) {
	full_buff.append("NF-");
	full_buff.append(F->request_name());
	full_buff.append("\nID-");
	full_buff.append(std::to_string(F->get_unique_id()));
	full_buff.append("\nPAR-");
	full_buff.append(is_root_folder(F) ? "$ROOT" : std::to_string(F->request_parent()->get_unique_id()));

	VFS_File** files = F->get_files();
	for (int i = 0; i < F->file_count(); i++) {
		VFS_File* file = *(files + i);
		full_buff.append("\n-");
		full_buff.append(file->name);
		full_buff.append(":");
		full_buff.append(std::to_string(file->size));
		full_buff.append("|");
		EncodeDecodeText::simpleEncode(full_buff, file->bytes);
	}
	
	full_buff.append("\nEOF\n");

	VFS_Folder** children = F->get_children();
	for (int j = 0; j < F->child_count(); j++) {
		write_folder_files(full_buff, *(children + j));
	}
}

void save_data(VFS_Folder* root) {
	//Utilize recursion to write out every folder
	std::string buffer_dat = "";
	write_folder_files(buffer_dat, root);

	std::ofstream outdat("saved.data");
	outdat << buffer_dat;
	outdat.close();
}

typedef struct {
	VFS_Folder* allocated_folder;
	int16_t supposed_parent_id;
} _LOAD_DAT_;

VFS_Folder* load_data(int& id_store) {
	std::ifstream data("saved.data");
	std::map<uint16_t, _LOAD_DAT_> folders; //{unique_id, LoadData}

	VFS_Folder* buffer_ptr = nullptr;
	int16_t parent_id_buffer = 0;

	std::string line;
	while (std::getline(data, line)) {
		if (line.find("NF-") != std::string::npos) {
			//New file
			std::string fname = line.substr(line.find("NF-") + strlen("NF-")); //find name
			buffer_ptr = (VFS_Folder*)malloc(sizeof(VFS_Folder)); //malloc
			buffer_ptr->alt_init(); //Alt init to set data to 0s
			buffer_ptr->modify_name(const_cast<char*>(fname.c_str()), 1); //Set name to located data. Push 1 to avoid unnecessary free.
			id_store += 1; //Attempt 1 at ensuring id is correct. Failsafe at ID
		}
		else if (line.find("ID-") != std::string::npos) {
			//Set fol;der id
			std::string id = line.substr(line.find("ID-") + strlen("ID-"));
			//std::cout << "\nID: " << id;
			buffer_ptr->set_unique_id(std::stoul(id));
			id_store = id_store < buffer_ptr->get_unique_id() ? buffer_ptr->get_unique_id() : id_store; //Failsafe for IDs. It's a huge problem if it isnt set correctly.
		}
		else if (line.find("PAR-") != std::string::npos) {
			std::string id = line.substr(line.find("PAR-") + strlen("PAR-")); //Locate parent id
		//	std::cout << "\nPar: " << id;
			if (id == "$ROOT") { //If we have it labeled as $ROOT, we set it to -1 (thats why its uint)
				//Yeah, it could be 0 since id's start at 1, but, i, do, not, care:)
				buffer_ptr->set_parent(nullptr); //Set parent to nullptr just bc
				parent_id_buffer = -1;//-1 id
			}
			else {
				parent_id_buffer = std::stoul(id); //Assume the file isnt touched by a third party and call stoul
			}
		}
		else if (line.find("EOF") != std::string::npos) { //File over. Push buffer ptr to new LOAD_DAT struct
			_LOAD_DAT_ ld; //new struct :)
			ld.allocated_folder = buffer_ptr; //Set pptr
			//std::cout << "Allocated: " << buffer_ptr << "\nLD: " << ld.allocated_folder << "\n";
			ld.supposed_parent_id = parent_id_buffer; //Within the struct is the ID of the folder we need to link to as our parent. This is where we save that/
			parent_id_buffer = 0; //reset pid buffer. dont bother changing folder buf
			folders[buffer_ptr->get_unique_id()] = (ld); //Attach our folder to the map. The key is the folder's true ID to help with linking.
			//Dont free..
		}
		else if (parent_id_buffer != 0) { //No keywords found.. but we're within a folder. (File)
			printf("E");
			std::string fileName = line.substr(line.find("-") + strlen("-"), line.find(":") - 1); //sub 1 cuz dont include - yay
			std::string bytes = line.substr(line.find(":") + strlen(":"), line.find("|") - line.find(":") -strlen("|"));
			std::string data = EncodeDecodeText::simpleRead(line); //Decode uniform bytes
			VFS_File* nf = (VFS_File*)malloc(sizeof(VFS_File)); //Allocate for new file
			//Set name
			nf->name = (char*)malloc(sizeof(char) * (fileName.size() + 1)); //Find name and allocate room for it + null term
			memcpy(nf->name, fileName.c_str(), sizeof(char) * fileName.size()); //Copy name data
			nf->name[fileName.size()] = '\0'; //Set null t
			nf->size = std::stoul(bytes); //Locate & set size (labeled bytes here, annoyingly, as thats meant to be the name of array)
			nf->bytes = (char*)malloc(sizeof(char) * (nf->size + 1)); //Alloc for byte array
			memcpy(nf->bytes, data.c_str(), sizeof(char) * (nf->size)); //Memcpy data
			nf->bytes[nf->size] = '\0'; //Add NT
			buffer_ptr->push_file(nf); //push file to current folder buffer
		}
	}
	printf("LInk");
	//Link folders to parents! :)
	auto b = folders.begin();
	while (b != folders.end()) {
		VFS_Folder* f = b->second.allocated_folder;
		if (f == nullptr) { printf("Attempt to push null folder as child or root."); abort(); }
		if (b->second.supposed_parent_id == -1) {
			buffer_ptr = f; //This is our root, and it is what we will return
		}
		else {
			if (folders.count(b->second.supposed_parent_id)) {
				//std::cout << "Push Child To " << b->second.supposed_parent_id << " @ " << folders[b->second.supposed_parent_id].allocated_folder << "\n";
				folders[b->second.supposed_parent_id].allocated_folder->push_child(f); //Push current folder f as a child of folder at requested parent id
				f->set_parent(folders[b->second.supposed_parent_id].allocated_folder); //Ensure current folder knows its parent
			}
			else {
				printf("Failure to find folder w/ id %d", b->second.supposed_parent_id);
				abort();
			}
		}
		
		b++;
	}
	
	return buffer_ptr; //Changed to root in linker
}
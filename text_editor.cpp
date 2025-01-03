#include "text_editor.hpp"

static bool are_we_in_editor = false;

static void print_lines() { //lol
	printf("\n");
	for (int i = 0; i < 20; i++) {
		printf("-");
	}
	printf("\n");
}

#define LONG_SLEEP 500
#define MID_SLEEP 250
#define SHORT_SLEEP 25

static void launch_line_editor(std::string& cline, int num) {
	num++;//bc we get 0-based and it isnt a reference!
	system("cls");
	std::cout << "Currently editing line " << num;
	print_lines();

	are_we_in_editor = true;
	std::cout << cline;
	SetCursorPos(cline.length(), 4);

	//Fuck it, i'm done using BS windows functions lmao.
	while (1) {
		for (int i = 0x20; i <= 0x5A; i++) {
			bool caps = GetKeyState(VK_CAPITAL) || (GetAsyncKeyState(VK_LSHIFT) & 0x8000);
			if (GetAsyncKeyState(i) & 0x8000) {
				if (!caps && (i >= 0x41)) {
					cline.push_back((char)(i + 32));
					std::cout << (char)(i + 32);
				}
				else if ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) && i >= 0x30 && i < 0x40) {
					if (i == 0x32) {
						cline.push_back((char)(0x40));
						std::cout << (char)(0x40);
					}
					else {
						cline.push_back((char)(i - 16));
						std::cout << (char)(i-16);
					}
				}
				else {
					cline.push_back((char)(i));
					std::cout << (char)(i);
				}
				Sleep(SHORT_SLEEP);
			}
		}
		
		if (GetAsyncKeyState(VK_BACK) & 0x8000) {
			if (cline.size()) {
				cline.pop_back();
				system("cls");
				std::cout << "Currently editing line " << num;
				print_lines();
				std::cout << cline;
				SetCursorPos(cline.length(), 4);
				Sleep(SHORT_SLEEP);
			}
		}
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
			Sleep(500);
			return;
		}
		Sleep(SHORT_SLEEP * 2);
	}

	
}

VFS_File* text_edit_new_file(std::string filename, std::vector<std::string> full_buf) {
	std::string fname;
	if (filename.empty()) {
		std::cout << "Enter new filename (dont add .txt): ";
		std::getline(std::cin, fname);
		fname.append(".txt");
	}
	else {
		fname = filename;
	}
	Sleep(700);
	bool run = true;
	
	std::cout << std::endl;

	int cline = 0;
	bool change = true; //Only reprint on change! Initialize so it prints first.
	bool allowed_to_edit = false;

	while (1) {
		if (change) {
			system("cls");
			std::cout << "Editing: " << fname << "\n";
			std::cout << "Line selected: " << (cline + 1) << "\n";
			print_lines();
			printf("\n");

			for (int L = 0; L < full_buf.size(); L++) {
				if (L == cline) {
					std::cout << "\x1b[30m\x1b[47m" << (L + 1) << ". " << full_buf[L] << "\x1b[0m\n";
				}
				else {
					std::cout << (L + 1) << ". " << full_buf[L] << "\n";
				}
			}

			print_lines();
			std::cout << "[ESC] To Save & Exit || [UP/DOWN] To Change Line || [ENTER] To Edit || [DEL] To Remove Line || [INS] To Add a Line";
			change = false;
		}

		/* Operations */
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) { //Edit line
			Sleep(LONG_SLEEP);
			launch_line_editor(full_buf[cline], cline);
			change = true;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			if (cline < (full_buf.size() - 1)) {
				cline++;
			}
			change = true;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000) {
			if (cline > 0) {
				cline--;
			}
			change = true;
		}
		else if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
			if (full_buf.size() >= 2) { //No empty files smh
				Sleep(MID_SLEEP);
				full_buf.erase(full_buf.begin() + cline);
				Sleep(MID_SLEEP);
				change = true;

				cline = (cline == full_buf.size() ? cline - 1 : cline);
			}
		}
		else if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
			full_buf.insert(full_buf.begin() + cline + 1, std::string());
			change = 1;
			Sleep(LONG_SLEEP);
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			std::string final_s;
			for (auto& s : full_buf) {
				final_s.append(s);
				final_s.push_back('\n');
			}
			VFS_File* nf = (VFS_File*)malloc(sizeof(VFS_File));

			nf->bytes = (char*)malloc(final_s.size() * sizeof(char));
			memcpy(nf->bytes, final_s.data(), sizeof(char) * final_s.size());
			nf->bytes[final_s.size()] = '\0';
			nf->size = final_s.size();

			nf->filetype = VFS_FILE_TEXT;
			nf->name = (char*)malloc(sizeof(char) * fname.size());
			memcpy(nf->name, fname.data(), sizeof(char) * fname.size());
			nf->name[fname.size()] = '\0';
			return nf;
		}

		Sleep(SHORT_SLEEP * 4);
		allowed_to_edit = true;
	}
}

void edit_existing_file(VFS_File* vfile) {

	std::vector<std::string> lines;
	std::istringstream iss(vfile->bytes);
	std::string buf;
	while (std::getline(iss, buf)) {
		lines.emplace_back(buf);
	}

	VFS_File* nvf = text_edit_new_file(vfile->name, lines);

	//Call me lazy lmaoo

	//Memcpy nvf data
	free(vfile->bytes);
	vfile->bytes = (char*)malloc(sizeof(char) * nvf->size);
	memcpy(vfile->bytes, nvf->bytes, sizeof(char) * nvf->size);
	vfile->size = nvf->size;
	vfile->bytes[vfile->size] = '\0';
	
	free(nvf->bytes);
	free(nvf->name);
	free(nvf);
}
/* Concept is:
- Save each folder's data in random order
- Link them at EoF
*/
#include "folder.h"
#include <fstream>
#include <map>
#include <algorithm>
void save_data(VFS_Folder* root);

VFS_Folder* load_data(int&); //returns Root!
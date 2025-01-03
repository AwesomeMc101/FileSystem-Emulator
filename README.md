# FileSystem-Emulator
Fully emulated filesystem which saves, has a built-in text editor, and supports everything a basic FS would.

Yeah it has a C++ main file which displays and has built in commands. Just learn from that if necessary.

# Insight
## Folders
* Folders are linked as a two-way linked-tree with parents and children.
* 1 parent, multiple children
* 1 Root folder which has no parent. Can store files
* Folders contain pointer-to-pointer arrays filled with pointers to their children and included files, respectively

## Files
* Files can be generated and edited with the built-in text editor. (See below)
* Files are encoded into a small, uniformed 3-digit encoding when saved.
* Files are all stored in a folder but do not contain pointers to parent folders. Folders are expected to manage their files.
* No built-in file deletion ig. Would be easy to add though. Free the pointer in the folder array and realloc file array to 1 less and memcpy both sides.

## Text Editor
* Works based on KeyState. It's not great I know but I wanted something that works and I thought it was cool so yeah.
* Permits newlines and stuff.

## Saving
* All data is saved to one file. Nothing is encoded but file text. Data looks like this:
```
NF-foldername
ID-2
PAR-1
-random_file.txt:10|123456789012345678901234567890
EOF
```
* The 10 represents 10 chars. 30 Digits b/c each char is encoded into 3 digits.

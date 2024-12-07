/*	
The following program is an implametation of the linux tree command

Tree
The tree command in Linux is a utility that visually displays the directory structure of files and directories in a tree-like format. It helps users understand the hierarchy and organisation of files within a directory, making it easier to navigate and locate files.The tree command allows customization of its output using various options the scope of our project is limited to the options listed below:

1) -a: Show all files, including hidden files (those starting with a dot).
2) -d: List directories only.
3) -R : Rerun tree when max dir level reached
4) --ignore-case: Ignore case when pattern matching.
5) --filelimit #: Do not descend directories that contain more than # entries. 
6) -o filename: Send output to file name. 
7) -u: Print the username, or UID # if no username is available, of the file.
8) -L [n]: Limit the depth of the directory tree to n levels.
9) -s: Print the size of each file and directory.
10) -r: Reverse the order of the output (display in reverse order)
11) -t: Sort the output by last modification time instead of alphabetically.
12) -p: Print the protections for each file.
13) -J: generates a JSON representation of the directory hierarchy.
*/


#include "tree.h"



int main(int argc, char *argv[]){
	int *args_info = NULL;
	if (argc < 2){
		fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
		return 1;
	}
	//n-ary tree to store subdirectories and files	
	tree directory_tree;
	init_tree(&directory_tree);
	//stack to store branches that have ended
	stack branch_ends;
	init_stack(&branch_ends, 30);
	
	
	if(argc > 2){
		args_info = check_mode(argc, argv);
/*
		//for diagnostic purpose
		int i;
		printf("%d\n", args_info[0]);
		for(i = 1; i < 14 ; i++)
			printf("%d\t", args_info[i]);
		printf("\n");
		//end check contents of args_info
*/		
		store_directory(argv[args_info[0]], argv[args_info[0]], 0, &directory_tree, args_info, argv);
		sort_tree_alphabetically(&directory_tree, args_info, argv);
		display_tree(directory_tree, &branch_ends, args_info, argv);
	}
	else{
		store_directory(argv[1], argv[1], 0, &directory_tree, args_info, argv);
		sort_tree_alphabetically(&directory_tree, args_info, argv);
		display_tree(directory_tree, &branch_ends, args_info, argv);
	}

	return 0;
}

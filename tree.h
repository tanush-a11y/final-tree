#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define OPTIONS 13

enum options{HIDDEN = 1, DIRECTORY_ONLY, RERUN, IGNORE_CASE, FILE_LIMIT, 
		OUTPUT_FILE, USER_NAME, LIMIT, SIZE, REVERSE, TIME_SORT, PROTECTIONS, JSON};

typedef struct stack{
	int top;
	int size;
	int *A;
}stack;

typedef struct node{
	char full_path[1024];
	char name[256];
	struct node* next_sibling;
	struct node* first_child;
	int child_count;
	int level;
}node;

typedef struct node* tree;

//stack operations
void init_stack(stack *s, int size);
void push(stack *s, int val);
int isEmpty(stack s);
void pop(stack *s);
void display_stack(stack s);
//end stack operations


void red(FILE *fp);	//print red coloured output
void green(FILE *fp);	//print green coloured output
void blue(FILE *fp);	//print blue coloured output
void purple(FILE *fp);	//print purple coloured output
void reset(FILE *fp);	//print default coloured output
void set_file_colour(char *path, FILE *output);


void init_tree(tree* t);	//function to initialise directory tree structure
void store_directory(const char *path, char *name, int tree_level, tree* t, int *options, char* argv[]);	//function to store subdirectories and files in tree structure
void sort_tree_alphabetically(tree *t, int *options, char *argv[]);	//function to sort contents of tree (files and subdirectories) alphabetically using bubble sort
void sort_tree_lastModified(tree *t, int *options, char *argv[]);	//function to sort contents of tree based on last modified 
void display_tree(tree t, stack *branch_ends, int *options, char* argv[]);	//display contents of tree structure (subdirectories and files)
void regular_display(tree t, stack *branch_ends, int *options, char* argv[], FILE *fp);	//display contents of tree structure (subdirectories and files)
void regular_JSON_display(tree t, stack *branch_ends, int *options, char *argv[], FILE *fp);// displays tree in jason format
void print_permissions(mode_t mode, FILE *fp); // helper function for printing permission
int *check_mode(int argc, char *argv[]);	//function to update an array which contains status of options requested
int which_option(char *option);	//function to check options






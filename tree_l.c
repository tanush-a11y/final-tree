#include "tree.h"

static int directory_count = 0, file_count = 0;
static int  prev_childCount = 0;


//stack operations
void init_stack(stack *s, int size){
	s->A = (int *)malloc(sizeof(int) * size);
	s->size = size;
	s->top = -1;
}

void push(stack *s, int val){
	if(s->top < s->size){
		s->top++;
		s->A[s->top] = val;
	}
	else
		return;
}
int isEmpty(stack s){
	if(s.top == -1)
		return 1;
	else
		return 0;
}
void pop(stack *s){
	if(isEmpty(*s))
		return;
	else
		s->top--;
	return;
}	
//end stack operations




//colour operations
void red(FILE *fp){
	if(fp)
		fprintf(fp, "\033[0;31m");
	else
		fprintf(stdout, "\033[0;31m");
	return;
}
void green(FILE *fp){
	if(fp)
		fprintf(fp, "\033[0;32m");
	else
		fprintf(stdout, "\033[0;32m");
	return;
}
void blue(FILE *fp){
	if(fp)
		fprintf(fp, "\033[0;34m");
	else
		fprintf(stdout, "\033[0;34m");
	return;
}
void purple(FILE *fp){
	if(fp)
		fprintf(fp, "\033[0;35m");
	else
		fprintf(stdout, "\033[0;35m");
	return;
}
void reset(FILE *fp){
	if(fp)
		fprintf(fp, "\033[0m");
	else
		fprintf(stdout, "\033[0m");
	return;
}
//end colour operations



//function to initialise directory tree structure
void init_tree(tree* t){
	*t = NULL;
	return;
}

//function to store subdirectories and files in tree structure
void store_directory(const char *path, char *name, int tree_level, tree* t, int *options, char *argv[]){
	
	DIR *dir;
	struct dirent *entry;
	struct stat entry_info;
	int i = 0;
	
	//create a node named directory and store its corresponding information
	node* directory = (node*)malloc(sizeof(node));
	if(directory){
		while(path[i] != '\0'){
			directory->full_path[i] = path[i];
			i++;
		}
		directory->full_path[i] = '\0';
		i = 0;
		directory->next_sibling = NULL;
		directory->first_child = NULL;
		directory->child_count = 0;
		directory->level = tree_level - 1;
		while(name[i] != '\0'){
			directory->name[i] = name[i];
			i++;
		}
		directory->name[i] = '\0';
	}
	else
		return;
	
	
	// Open the directory
	dir = opendir(path);
	if (dir == NULL){
		perror("opendir failed\n");
		return;
	}
	
	if(*t == NULL){
		*t = directory;
	}
	
		
	// Read each entry in the directory
	while ((entry = readdir(dir)) != NULL){

		char full_path[1024];
		i = 0;
		
		// Skip the "." and ".." entries
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
			continue;
		}
		
		if(strcmp(entry->d_name, ".git") == 0){
			if(options != NULL && options[HIDDEN] == 1){
			}
			else
				continue;		
		}
		// Build the full path to the entry
		snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

		// Get file information using stat()
		if (stat(full_path, &entry_info) == -1) {
			perror("stat failed");
			continue;
		}
		
		//create a node named file to store contents of directory or subdirectory and store its corresponding information
		node* file = (node*)malloc(sizeof(node));
		if(file){
			while(full_path[i] != '\0'){
				file->full_path[i] = full_path[i];
				i++;
			}
			file->full_path[i] = '\0';
			i = 0;
			file->next_sibling = NULL;
			file->first_child = NULL;
			file->child_count = 0;
			file->level = tree_level;
			while(entry->d_name[i] != '\0'){
				file->name[i] = entry->d_name[i];
				i++;
			}
			file->name[i] = '\0';
		}
		else
			return;
			
		node* p;
		//check if file is the first child
		if(directory->first_child == NULL){
			p = directory;
			// Check if it's a directory
			if(S_ISDIR(entry_info.st_mode)){
				directory_count++;
				tree_level++;
				// Recursively list the subdirectory
				store_directory(full_path, file->name, tree_level, &p->first_child, options, argv);
				tree_level--;
				p = p->next_sibling;
			}
			else{
				if(options != NULL && options[DIRECTORY_ONLY] == 1)
					continue;
				//Its a File
				p->first_child = file;
				file_count++;
			}			
			directory->child_count++;
		}
		else{
			p = directory->first_child;
			while(p->next_sibling != NULL)
				p = p->next_sibling;
			// Check if it's a directory
			if(S_ISDIR(entry_info.st_mode)){
				directory_count++;
				tree_level++;
				// Recursively list the subdirectory
				store_directory(full_path, file->name, tree_level, &p->next_sibling, options, argv);
				tree_level--;
				p = p->next_sibling;
			}
			else{
				if(options != NULL && options[DIRECTORY_ONLY] == 1)
					continue;
				//Its a File
				p->next_sibling = file;
				file_count++;
			}	
			directory->child_count++;
		}
			
			
	}
	// Close the directory stream
	if (closedir(dir) == -1) {
		perror("closedir failed");
	}
}

// Function to perform bubble sort on the linked list
void sort_tree_alphabetically(tree *t, int *options, char *argv[]){
	if(options != NULL && options[TIME_SORT] != 0){
		sort_tree_lastModified(t, options, argv);
		return;
	}
	int len = (*t)->child_count;
	int i = 0;
	int swapped;
	// Iterating over the whole linked list
	while (i < len) {
		node *traverseNode = (*t)->first_child;
		node *prevNode = (*t)->first_child;
		swapped = false;
		while (traverseNode->next_sibling) {

			// Temporary pointer to store the next
			// pointer of traverseNode
			node *ptr = traverseNode->next_sibling;
			if(traverseNode->first_child != NULL){
				sort_tree_alphabetically(&traverseNode, options, argv);
			}
			else if(ptr->first_child != NULL){
				sort_tree_alphabetically(&ptr, options, argv);
			}
			if(options != NULL && options[REVERSE] != 0){
				if (strcasecmp(traverseNode->name, ptr->name) < 0) {
					swapped = true;
					if (traverseNode == (*t)->first_child) {
	
						// Performing swap operations and
						// updating the head of the linked list
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode = ptr;
						(*t)->first_child = prevNode;
					}
					else {
	
						// Performing swap operation
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode->next_sibling = ptr;
						prevNode = ptr;
					}
					continue;
				}
			}
			else{
				if (strcasecmp(traverseNode->name, ptr->name) > 0) {
					swapped = true;
					if (traverseNode == (*t)->first_child) {
	
						// Performing swap operations and
						// updating the head of the linked list
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode = ptr;
						(*t)->first_child = prevNode;
					}
					else {
	
						// Performing swap operation
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode->next_sibling = ptr;
						prevNode = ptr;
					}
					continue;
				}
			}
			prevNode = traverseNode;
			traverseNode = traverseNode->next_sibling;
		}
		// If no swap occurred, break the loop
		if (!swapped) {
			break;
		}
		i++;
	}	
}

void sort_tree_lastModified(tree *t, int *options, char *argv[]){
	int len = (*t)->child_count;
	int i = 0;
	int swapped;
	struct stat node1, node2;
	// Iterating over the whole linked list
	while (i < len) {
	
		node *traverseNode = (*t)->first_child;

		node *prevNode = (*t)->first_child;
   		
		swapped = false;
		
		while (traverseNode->next_sibling) {

			// Temporary pointer to store the next
			// pointer of traverseNode
			node *ptr = traverseNode->next_sibling;
			stat(traverseNode->full_path, &node1);
			stat(ptr->full_path, &node2);
	
			if(traverseNode->first_child != NULL){
				sort_tree_alphabetically(&traverseNode, options, argv);
			}
			else if(ptr->first_child != NULL){
				sort_tree_alphabetically(&ptr, options, argv);
			}
		
			if(options != NULL && options[REVERSE] != 0){
				if (node1.st_mtime < node2.st_mtime) {
					swapped = true;
					if (traverseNode == (*t)->first_child) {
	
						// Performing swap operations and
						// updating the head of the linked list
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode = ptr;
						(*t)->first_child = prevNode;
					}
					else {
	
						// Performing swap operation
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode->next_sibling = ptr;
						prevNode = ptr;
					}
					continue;
				}
			}
			else{
				if (node1.st_mtime > node2.st_mtime) {
					swapped = true;
					if (traverseNode == (*t)->first_child) {
						// Performing swap operations and
						// updating the head of the linked list
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode = ptr;
						(*t)->first_child = prevNode;
					}
					else {
	
						// Performing swap operation
						traverseNode->next_sibling = ptr->next_sibling;
						ptr->next_sibling = traverseNode;
						prevNode->next_sibling = ptr;
						prevNode = ptr;
					}
					continue;
				}
			}
			prevNode = traverseNode;
			traverseNode = traverseNode->next_sibling;
		}
		// If no swap occurred, break the loop
		if (!swapped) {
			break;
		}
		i++;
	}	
}


void set_file_colour(char *path, FILE *output){
	unsigned char magic_number[4];
	char *ext = strrchr(path, '.');
	FILE *fp = fopen(path, "rb");
	if(fp){
		fread(magic_number, 1, 4, fp);
		fclose(fp);
	}
	else
		printf("File not opened\n");
	//for executable and linkable files magic number is
	if(magic_number[0] == 0x7F && magic_number[1] == 0x45 && magic_number[2] == 0x4C && magic_number[3] == 0x46)
		green(output);
	//for PNG files magic number is
	else if(magic_number[0] == 0x89 && magic_number[1] == 0x50 && magic_number[2] == 0x4E && magic_number[3] == 0x47)
		purple(output);
	//for archive and zip files 
	else if(ext && (strcmp(ext + 1, "zip") == 0 || strcmp(ext + 1, "gz") == 0)) 
		red(output);
	else
		reset(output);
	return;
}

void display_tree(tree t, stack *branch_ends, int *options, char *argv[]){
	//check for -o option
	FILE *fp = NULL;
	struct stat attr;
	
	if(options != NULL && options[OUTPUT_FILE] != 0){
		fp = fopen(argv[options[OUTPUT_FILE]], "a");
		if(!fp){
			printf("Error opening file\n");
			return;
		}
	}
	if(options != NULL && options[JSON] != 0){
		directory_count++;
	}
	else{
		
		if(options != NULL){
			if(options[PROTECTIONS] || options[USER_NAME] || options[SIZE]){
				if (stat(argv[options[0]],&attr) == -1) {
					perror("stat failed");
					return;
				}
				if(fp)
					fprintf(fp, "[");
				else
					printf("[");
				if(options[PROTECTIONS] != 0){
					print_permissions(attr.st_mode, fp);
				}
				if(options[USER_NAME] != 0){
					 struct passwd *user = getpwuid(attr.st_uid);
					 if (user != NULL){
       						 if(fp)
       						 	fprintf(fp, "  %s", user->pw_name);
       						 else
       						 	fprintf(stdout, "  %s", user->pw_name);
					}
				}
				if(options[SIZE] != 0){
					 if(fp)
       						fprintf(fp, "\t%ld", attr.st_size);
       					else
       					 	fprintf(stdout, "\t%ld", attr.st_size);
				}
				if(fp)
					fprintf(fp, "]  ");
				else
					printf("]  ");
			}
			blue(NULL);
			if(fp){
				fprintf(fp, "%s", argv[options[0]]);
			}
			else{
				printf("%s", argv[options[0]]); 
			}
			reset(NULL);
		}
		else{
			blue(NULL);
			printf("%s", argv[1]); 
			reset(NULL);
		}
		printf("\n");
		directory_count++;
	}
	
	regular_display(t, branch_ends, options, argv, fp);
	
	if(options != NULL && options[JSON] != 0){
		if(fp)
			fclose(fp);
		return;
	}
	if(fp)
		fprintf(fp ,"\n%d directories, %d files\n", directory_count, file_count);
	else
		fprintf(stdout ,"\n%d directories, %d files\n", directory_count, file_count);
	if(fp)
		fclose(fp);
	return;
}



//display contents of tree structure (subdirectories and files)
void regular_display(tree t, stack *branch_ends, int *options, char *argv[], FILE *fp){
	struct stat entry_info;	
	//check for -J option
	if(options != NULL && options[JSON] != 0){
		if (stat(argv[options[0]], &entry_info) == -1) {
			perror("stat failed");
			return;
		}
		if(options[OUTPUT_FILE] != 0 && fp){
			fprintf(fp, "[\n");
			fprintf(fp, "  {\"type\":\"directory\",\"name\":\"%s\",",argv[options[0]]);
			if(options[PROTECTIONS]){
				fprintf(fp, "\"prot\":\"");
				print_permissions(entry_info.st_mode, fp);
				fprintf(fp, "\",");
			}
			if(options[USER_NAME]){
				fprintf(fp, "\"user\":\"");
				struct passwd *user = getpwuid(entry_info.st_uid);
				if (user != NULL)
       					fprintf(fp, "%s", user->pw_name);
       				else
       					fprintf(fp, "#");
				fprintf(fp, "\",");
			}
			if(options[SIZE]){
				fprintf(fp, "\"size\":\"%ld\",", entry_info.st_size);
			}
			fprintf(fp,"\"contents\":[\n");
			regular_JSON_display(t, branch_ends, options, argv, fp);
			fprintf(fp, "  ]}\n,\n");
			fprintf(fp, "  {\"type\":\"report\",\"directories\":%d,\"files\":%d}\n", directory_count, file_count);
			fprintf(fp, "]\n");
		}
		else{
			printf("[\n");
			printf("  {\"type\":\"directory\",\"name\":\"%s\",",argv[options[0]]);
			if(options[PROTECTIONS]){
				printf("\"prot\":\"");
				print_permissions(entry_info.st_mode, fp);
				printf("\",");
			}
			if(options[USER_NAME]){
				printf("\"user\":\"");
				struct passwd *user = getpwuid(entry_info.st_uid);
				if (user != NULL)
       					printf("%s", user->pw_name);
       				else
       					printf("#");
				printf("\",");
			}
			if(options[SIZE]){
				printf("\"size\":\"%ld\",", entry_info.st_size);
			}
			printf("\"contents\":[\n");
			regular_JSON_display(t, branch_ends, options, argv, fp);
			printf("  ]}\n,\n");
			printf("  {\"type\":\"report\",\"directories\":%d,\"files\":%d}\n", directory_count, file_count);
			printf("]\n");
		}
		return;
	}
	
	node* p = t->first_child;
	int i, j;
	int count = t->child_count;
	while(p){
		if (stat(p->full_path, &entry_info) == -1) {
			perror("stat failed");
			continue;
		}	
		if(p->first_child != NULL || S_ISDIR(entry_info.st_mode)){
				count--;
				j = 0;
				for(i = 0; i < p->level; i++){
					if(i == branch_ends->A[j] && j <= branch_ends->top){
						if(fp)
							fprintf(fp, "    ");
						else
							fprintf(stdout, "    ");
						j++;
					}
					else{
						if(fp)
							fprintf(fp, "|   ");
						else
							fprintf(stdout, "|   ");
					}
				}
				
				if(count == 0){
					if(fp)
						fprintf(fp, "`-- ");
					else
						fprintf(stdout, "`-- ");
					push(&(*branch_ends), p->level);
				}
				else{
					if(fp)
						fprintf(fp, "|-- ");
					else
						fprintf(stdout, "|-- ");
				}
				
				
				if(options != NULL && (options[PROTECTIONS] || options[USER_NAME] || options[SIZE])){
					if(fp)
						fprintf(fp, "[");
					else
						printf("[");
					if(options[PROTECTIONS] != 0)
						print_permissions(entry_info.st_mode, fp);
					if(options[USER_NAME] != 0){
						struct passwd *user = getpwuid(entry_info.st_uid);
						if (user != NULL){
       							if(fp)
       						 		fprintf(fp, "  %s", user->pw_name);
       							else
       							 	fprintf(stdout, "  %s", user->pw_name);
						}
					}
					if(options[SIZE] != 0){
						if(fp)
       							fprintf(fp, "\t%ld", entry_info.st_size);
       						else
       					 		fprintf(stdout, "\t%ld", entry_info.st_size);
					}
					if(fp)
						fprintf(fp, "]  ");
					else
						printf("]  ");
					
				}
				
				blue(fp);
				if(fp){
					fprintf(fp, "%s", p->name);
					reset(fp);
					fprintf(fp, "\n");
				}
				else{
					fprintf(stdout, "%s", p->name);
					reset(fp);
					fprintf(stdout, "\n");
				}
				
				
				prev_childCount = count;
				
		
				//Recursively call subdirectories
				regular_display(p, branch_ends, options, argv, fp);
				
				if(p->child_count != 0)
					pop(&(*branch_ends));
				p = p->next_sibling;
		}
		else{
				count--;
				j = 0;
				for(i = 0; i < p->level; i++){
					if(i == branch_ends->A[j] && j <= branch_ends->top){
						if(fp)
							fprintf(fp, "    ");
						else
							fprintf(stdout, "    ");
						j++;
					}
					else{
						if(fp)
							fprintf(fp, "|   ");
						else
							fprintf(stdout, "|   ");
					}
				
				}		
				if(count == 0){
					if(fp)
						fprintf(fp, "`-- ");
					else
						fprintf(stdout, "`-- ");
					push(&(*branch_ends), p->level);
				}
				else{
					if(fp)
						fprintf(fp, "|-- ");
					else
						fprintf(stdout, "|-- ");
				}	
	
				
				if(options != NULL && (options[PROTECTIONS] || options[USER_NAME] || options[SIZE])){
					if(fp)
						fprintf(fp, "[");
					else
						printf("[");
					if(options[PROTECTIONS] != 0)
						print_permissions(entry_info.st_mode, fp);
					if(options[USER_NAME] != 0){
						struct passwd *user = getpwuid(entry_info.st_uid);
						if (user != NULL){
       							if(fp)
       						 		fprintf(fp, "  %s", user->pw_name);
       							else
       							 	fprintf(stdout, "  %s", user->pw_name);
						}
					}
					if(options[SIZE] != 0){
						if(fp)
       							fprintf(fp, "\t%ld", entry_info.st_size);
       						else
       					 		fprintf(stdout, "\t%ld", entry_info.st_size);
					}
					if(fp)
						fprintf(fp, "]  ");
					else
						printf("]  ");
					
				}
					
				set_file_colour(p->full_path, fp);
				if(fp){
					
					fprintf(fp, "%s", p->name);
					reset(fp);
					fprintf(fp, "\n");
				}
				else{
					fprintf(stdout, "%s", p->name);
					reset(fp);
					fprintf(stdout, "\n");
				}
				p = p->next_sibling;
		}	
	}
	return;
}


/*-J commit*/
void regular_JSON_display(tree t, stack *branch_ends, int *options, char *argv[], FILE *fp){	

	node* p = t->first_child;
	struct stat entry_info;
	int i;
	int count = t->child_count;
	while(p){
		if (stat(p->full_path, &entry_info) == -1) {
			perror("stat failed");
			continue;
		}	
		if(p->first_child != NULL || S_ISDIR(entry_info.st_mode)){
				count--;
				for(i = 0; i < p->level + 1; i++){
					if(fp)
						fprintf(fp, "  ");
					else
						fprintf(stdout, "  ");
				}
				if(!isEmpty(*branch_ends) && count == 0){
					if(fp){
						fprintf(fp, "  {\"type\":\"directory\",\"name\":%s,",p->name);
						
						if(options[PROTECTIONS] != 0){
							fprintf(fp, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(fp, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(fp, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(fp, "%s", user->pw_name);
       							else
       								fprintf(fp, "#");
							fprintf(fp, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(fp, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(fp,"\"contents\":[\n");						
					}
					else{
						fprintf(stdout, "  {\"type\":\"directory\",\"name\":%s,",p->name);
						
						if(options[PROTECTIONS] != 0){
							fprintf(stdout, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(stdout, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(stdout, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(stdout, "%s", user->pw_name);
       							else
       								fprintf(stdout, "#");
							fprintf(stdout, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(stdout, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(stdout,"\"contents\":[\n");	
					}
					push(&(*branch_ends), p->level);
				}
				else 
				{
					if(fp){
						fprintf(fp, "  {\"type\":\"directory\",\"name\":%s,",p->name);
						
						if(options[PROTECTIONS] != 0){
							fprintf(fp, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(fp, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(fp, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(fp, "%s", user->pw_name);
       							else
       								fprintf(fp, "#");
							fprintf(fp, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(fp, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(fp,"\"contents\":[\n");						
					}
					else{
						fprintf(stdout, "  {\"type\":\"directory\",\"name\":%s,",p->name);
						
						if(options[PROTECTIONS] != 0){
							fprintf(stdout, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(stdout, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(stdout, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(stdout, "%s", user->pw_name);
       							else
       								fprintf(stdout, "#");
							fprintf(stdout, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(stdout, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(stdout,"\"contents\":[\n");	
					}
				}
				prev_childCount = count;
				
				regular_JSON_display(p, branch_ends, options, argv, fp);
				
				if(p->child_count != 0){
					pop(&(*branch_ends));
					for(i = 0; i < p->level+1; i++){
						if(fp)
							fprintf(fp, "  ");
						else
							fprintf(stdout, "  ");
					}
					if(fp)
						fprintf(fp, "  ]},\n");
					else
						fprintf(stdout, "  ]},\n");
				}
				p = p->next_sibling;
		}
		else{
				count--;
				for(i = 0; i < p->level+1; i++){
					if(fp)
						fprintf(fp, "  ");
					else
						fprintf(stdout, "  ");
				}			
				if(count == 0){
					if(fp){
						fprintf(fp, "  {\"type\":\"file\",\"name\":\"%s\"", p->name);
						
						if(options[PROTECTIONS] || options[USER_NAME] || options[SIZE])
							fprintf(fp, ",");
						if(options[PROTECTIONS] != 0){
							fprintf(fp, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(fp, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(fp, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(fp, "%s", user->pw_name);
       							else
       								fprintf(fp, "#");
							fprintf(fp, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(fp, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(fp, "}\n");
					}
					else{
						fprintf(stdout, "  {\"type\":\"file\",\"name\":\"%s\"", p->name);
						
						if(options[PROTECTIONS] || options[USER_NAME] || options[SIZE])
							fprintf(stdout, ",");
						if(options[PROTECTIONS] != 0){
							fprintf(stdout, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(stdout, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(stdout, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(stdout, "%s", user->pw_name);
       							else
       								fprintf(stdout, "#");
							fprintf(stdout, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(stdout, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(stdout, "}\n");
					}
					push(&(*branch_ends), p->level);
				}
				else{
					if(fp){
						fprintf(fp, "  {\"type\":\"file\",\"name\":\"%s\"", p->name);
						
						if(options[PROTECTIONS] || options[USER_NAME] || options[SIZE])
							fprintf(fp, ",");
						if(options[PROTECTIONS] != 0){
							fprintf(fp, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(fp, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(fp, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(fp, "%s", user->pw_name);
       							else
       								fprintf(fp, "#");
							fprintf(fp, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(fp, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(fp, "}\n");
					}
					else{
						fprintf(stdout, "  {\"type\":\"file\",\"name\":\"%s\"", p->name);
						
						if(options[PROTECTIONS] || options[USER_NAME] || options[SIZE])
							fprintf(stdout, ",");
						if(options[PROTECTIONS] != 0){
							fprintf(stdout, "\"prot\":\"");
							print_permissions(entry_info.st_mode, fp);
							fprintf(stdout, "\",");
						}
						if(options[USER_NAME] != 0){
							fprintf(stdout, "\"user\":\"");
							struct passwd *user = getpwuid(entry_info.st_uid);
							if (user != NULL)
       								fprintf(stdout, "%s", user->pw_name);
       							else
       								fprintf(stdout, "#");
							fprintf(stdout, "\",");
						}
						if(options[SIZE] != 0){
							fprintf(stdout, "\"size\":\"%ld\",", entry_info.st_size);
						}
						
						fprintf(stdout, "}\n");
					}
				}
				p = p->next_sibling;
		}	
	}	
	return;
}


/*
	args_info is an array that stores the options that are requested by the user
	0th index : stores the root directories index in argv[]
	1st to 13th index : store status of options , 1 meaning requested 0 meaning not requested
*/
int* check_mode(int argc, char *argv[]){
	int i, index;
	int *args_info = (int *)malloc(sizeof(int) * (OPTIONS + 1));
	for(i = 0; i < OPTIONS + 1; i++)
		args_info[i] = 0;
	if(args_info){
		for(i = 1; i < argc; i++){
			index = which_option(argv[i]);
			if(index == -1){
				args_info[0] = i;
				continue;
			}
			
			if(strcmp(argv[i], "-filelimit") == 0 || strcmp(argv[i], "-L") == 0){
				args_info[index] = atoi(argv[++i]);
			}
			else if(strcmp(argv[i], "-o") == 0){
				args_info[index] = ++i;
			}
			else{
				args_info[index] = 1;
			}
		}
		
	}
	else
		return NULL;
	return args_info;
}

int which_option(char *option){
	int i; 
	char *options[] = {"-a", "-d", "-R", "--ignore-case", "-filelimit", "-o", "-u", "-L", "-s", "-r", "-t", "-p", "-J"};
	const int length = 13;
	for(i = 0; i < length; i++){
		if(strcmp(options[i], option) == 0)
			return (i + 1);
	}
	return -1;
}


void print_permissions(mode_t mode, FILE *fp) {
	if(fp){	
    		fprintf(fp, (mode & S_IRUSR) ? "r" : "-");
		fprintf(fp, (mode & S_IWUSR) ? "w" : "-");
		fprintf(fp, (mode & S_IXUSR) ? "x" : "-");	
		fprintf(fp, (mode & S_IRGRP) ? "r" : "-");
		fprintf(fp, (mode & S_IWGRP) ? "w" : "-");
		fprintf(fp, (mode & S_IXGRP) ? "x" : "-");
		fprintf(fp, (mode & S_IROTH) ? "r" : "-");
		fprintf(fp, (mode & S_IWOTH) ? "w" : "-");
		fprintf(fp, (mode & S_IXOTH) ? "x" : "-");	
	}
	else{
    		printf((mode & S_IRUSR) ? "r" : "-");
		printf((mode & S_IWUSR) ? "w" : "-");
		printf((mode & S_IXUSR) ? "x" : "-");	
		printf((mode & S_IRGRP) ? "r" : "-");
		printf((mode & S_IWGRP) ? "w" : "-");
		printf((mode & S_IXGRP) ? "x" : "-");
		printf((mode & S_IROTH) ? "r" : "-");
		printf((mode & S_IWOTH) ? "w" : "-");
		printf((mode & S_IXOTH) ? "x" : "-");	
	}
}


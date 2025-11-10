#define _POSIX_C_SOURCE 200809L//to use strtok_r function
#include "types.h"
#include "string.h"
/*struct NODE {
    char name [64];
    char fileType;
    struct NODE* childPtr;
    struct NODE* siblingPtr;
    struct NODE* parentPtr;
};*/

extern struct NODE* root;
extern struct NODE* cwd;


//function that searches for a child node by name in a parent's node
struct NODE* search_child(struct NODE* parent, char* name) {

    //check if parent or name are null
    if (!parent || !name){
	 return NULL;
    }

    //starting the traversal of the parent's children from the first one
    struct NODE* child = parent->childPtr;

    // loop through children of the parent node
    while (child) {

        //return matching child if found
        if (!strcmp(child->name, name)) {
            return child;
        }
        //go to the next node
        child = child->siblingPtr;
    }
    //return null if no matching is found
    return NULL;
}

//make directory
void mkdir(char pathName[]){

    char baseName[64] = "";
    char dirName[124] = "";
    struct NODE* parent = NULL;
    struct NODE* newNode = NULL;

    //check for the case when the user gives no path name
    if (strcmp(pathName, "/") == 0) {
       
        printf("MKDIR ERROR: no path provided\n"); 
        return;
    }

    //get the parent directory's node and the new directory's name


    parent = splitPath(pathName, baseName, dirName);

    //check if splitPath failed 

    if (parent == NULL) {

        return;
    }
    
    //prevent creation of a directory with an empty name for example path that ends with '/'
    if (strlen(baseName) == 0) {
        printf("MKDIR ERROR: cannot create directory with empty name\n"); 
        return;
    }
    //check if the directory already exists in the parent's node

    if (search_child(parent, baseName) != NULL) {
        printf("MKDIR ERROR: directory %s already exists\n", pathName); 
        return;
    }

    //create and initialize newNODE

    newNode = (struct NODE*)malloc(sizeof(struct NODE));
    if (newNode == NULL) {
        printf("ERROR: Memory allocation failed.\n");
        return;
    }
    
    //initialize fields
    strcpy(newNode->name, baseName);
    newNode->fileType = 'D'; 
    newNode->parentPtr = parent;
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;

    //insert newNODE into the parent's children list
    //if the newNode is the first child

    if (parent->childPtr == NULL) {

        parent->childPtr = newNode;
    }
    //else go to the last sibling and attach newNode
    else {
        

        struct NODE* child = parent->childPtr;

        while (child->siblingPtr != NULL) {
            child = child->siblingPtr;
        }
        child->siblingPtr = newNode;
    }

   
    printf("MKDIR SUCCESS: node %s successfully created\n", pathName);
}

//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

   
    char* token = NULL;
    struct NODE* start_node = NULL;
    struct NODE* parent_node = NULL;
    char path_to_traverse[128] = "";//variable for the path that leads to the parent
    char temp_dir_path[128] = "";
    char* path_for_tokenization = NULL;
    char* saveptr = NULL;



    //separate baseName and dirName using strrchr which returns the last occurance of '/'
    char *last_slash = strrchr(pathName, '/');

    if (last_slash == NULL) {

        //case when "f1.txt" is relative with no '/')

        strcpy(dirName, "");
    
        strcpy(baseName, pathName);
       
        start_node = cwd;
    }
    //case when there is only one / which is the root directory 
    else if (last_slash == pathName) {
        if (strlen(pathName) == 1) {
            
            strcpy(dirName, "/");
            strcpy(baseName, ""); 
            return root; //

        }
        strcpy(dirName, "/");
        strcpy(baseName, last_slash + 1);
       
        strcpy(path_to_traverse, "");
        start_node = root;
   
    }
    //case when there is multiple nodes in the path 
    else {
   
        size_t dir_len = last_slash - pathName;
        strncpy(dirName, pathName, dir_len);
        dirName[dir_len] = '\0';

        strcpy(baseName, last_slash + 1);
        strcpy(path_to_traverse, dirName);
        
        //determining the start node
        if (pathName[0] == '/') {
            start_node = root;
        } else {
            start_node = cwd;
        }
    }
    
    //finding the parent node by traversing dirName
    
    //if path_to_traverse is empty then the parent is the starting node
    if (strlen(path_to_traverse) == 0 || strcmp(dirName, "/") == 0 || strcmp(dirName, ".") == 0) {
        return start_node;
    }

    //prepare path_to_traverse for tokenization
   
    strcpy(temp_dir_path, dirName);
    
    //declare a temporary pointer for tokenization
    path_for_tokenization = temp_dir_path;
    
    //skip the leading '/' for absolute paths by advancing the pointer
    if (path_for_tokenization[0] == '/') {
        path_for_tokenization++;
    }

    //tokenize the directory path
   
    //use strtok_r for multi arguments
    token = strtok_r(path_for_tokenization, "/", &saveptr); 
    
    parent_node = start_node;
    
    while (token) {
        struct NODE* next_node = search_child(parent_node, token);
        //check if there is no directory
        if (next_node == NULL) {
            printf("ERROR: directory %s does not exist\n", token);
            return NULL;
        }
        //check if the path has a file and not a directory
        if (next_node->fileType != 'D') {
            printf("ERROR: '%s' is not a directory\n", token);
            return NULL;
        }

        parent_node = next_node;
        token = strtok_r(NULL, "/", &saveptr);
    }
    
    //parent_node to return is the parent of baseName
    return parent_node;
}


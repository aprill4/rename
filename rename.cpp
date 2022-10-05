// $ clang++ -o # @ -std=c++11 -g
//TODO: hope it can rename files with more stars

#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<cstring>
#include<sys/types.h>

//struct contains a string and 2 integers for position recording
struct FileMatched{
	const char *filename;
	int star_begins;
	int star_length;
} files_matched[50];

int fc = 0;

struct Command{
	bool force_flag = 0;
	bool show_help = 0;
	const char *d = "."; 
	const char *old_name = NULL;
	const char *new_name = NULL;
    const char *old_a;
    const char *old_c;
    const char *new_a;
    const char *new_c;
};

void print_usage(){

	printf("Usage: rename [OPTION]... [OLD_FILENAME] [NEW_FILENAME]\n");
	printf("Rename old filename to new filename (the current directory by default).\n");

	printf("\nOptions:\n");
	printf("    -f,\tforce to rename\n");
	printf("    -d,\tfollowed by a directory name to specify a directory\n");
	printf("    -h,\tdisplay this help and exit\n");

	printf("\nHINT:\n");
    printf("\t1. \"*\" in filename can match any characters, but remember to double quote the filename when using it and only one star can be used so far.\n");
    printf("\t2. An old name must be followed by a new name!\n");

}


struct Command parsing_command(int argc, char *argv[]){
	struct Command cmd;
	
	if (argc == 1){
		cmd.show_help = 1;
		return cmd;
	}

	for (int i = 1; i < argc; i++){

		if (!strcmp(argv[i], "-h")){
			cmd.show_help = 1;
			return cmd;

		} else if (!strcmp(argv[i], "-f")){
			cmd.force_flag = 1;
			continue;

		} else if (!strcmp(argv[i], "-d")){
			if (!(i + 1 < argc)){
				cmd.show_help = 1;
				return cmd;

			} else if(!strcmp(argv[i+1], "-f") || !strcmp(argv[i+1], "-h")){
					cmd.show_help = 1;
					return cmd;

			} else{ 
				cmd.d = argv[i+1];
				i++;
				continue;
			}

		} else {
			cmd.old_name = argv[i];

			if (i + 1 < argc) {
				cmd.new_name = argv[i+1];
				i++;
                continue;

			} else{
				cmd.show_help = 1;
				return cmd;
			}
		}
	}

	if (cmd.old_name == NULL || cmd.new_name == NULL){
		cmd.show_help = 1;
		return cmd;
	}

    printf("pos\n");

    const char *old_star_pos = strchr(cmd.old_name, '*');
    const char *new_star_pos = strchr(cmd.new_name, '*');

    if (old_star_pos) {
        printf("found * in %s\n", cmd.old_name);
        int len_old_name = strlen(cmd.old_name);
        int len_pos = strlen(old_star_pos);

        char *a = new char[len_old_name-len_pos];
        // char a[len_old_name-len_pos];

        strncpy(a, cmd.old_name, len_old_name-len_pos);
        const char *c = (char *)old_star_pos + 1;
        cmd.old_a = a;
        cmd.old_c = c;

    } else {
        cmd.old_a = "";
        cmd.old_c = "";
    }

    if (new_star_pos) {
        printf("found * in new_name\n");
        int len_new_name = strlen(cmd.new_name);
        int len_pos = strlen(new_star_pos);

        char *a = new char[len_new_name-len_pos];

        //char a[len_new_name-len_pos];

        strncpy(a, cmd.new_name, len_new_name-len_pos);
        const char *c = (char *)new_star_pos + 1;
        cmd.new_a = a;
        cmd.new_c = c;

    } else {
        cmd.new_a = "";
        cmd.new_c = "";
    }
    
	return cmd;
}

struct FileMatched* matching_files(const char *filename, const char *a, const char *c) {

    printf("filename: %s, a: %s, c: %s\n", filename, a, c);

    struct FileMatched *matched_file = new struct FileMatched;

    int len_a = strlen(a);
    if (strncmp(a, filename, len_a)) {
        return nullptr;
    }

    matched_file->star_begins = len_a;
    matched_file->filename = filename;

    int len_filename = strlen(filename);
    int len_c = strlen(c);

    if (len_c == 0) {
        matched_file->star_length = len_filename - len_a;
        return matched_file;
    }

    int star_ends = len_filename - len_c;
    char *last_len_c_chars = (char *)filename + star_ends; 

    if (strncmp(c, last_len_c_chars, len_c)) {
        return nullptr;
    }

    matched_file->star_length = star_ends - matched_file->star_begins;
    return matched_file;
}

void find_files(const char *mydir, const char *a, const char *c){
	DIR *d;
	struct dirent *dir;
	d = opendir(mydir);

    printf("mydir: %s\n", mydir);

    if (!d) {
        printf("No such directory!\n");
        return;
    }

    struct FileMatched *fm = new FileMatched;

    while ((dir = readdir(d)) != NULL){
        fm = matching_files(dir->d_name, a, c);
        if (fm){
            printf("%s\n", fm->filename);
            files_matched[fc++] = *fm;
        }
    }

    if (fc == 0){
        printf("No such files\n");
    } else{
        printf("Found ");
        fc > 1 ? printf("%d files", fc) : printf("1 file");
        printf(" as above\n");
    }
    delete fm;
}

char *construct_new_name(const FileMatched &fm, Command &cmd){

	const char *star = fm.filename + fm.star_begins;
	char *new_name = new char[50];

    strcpy(new_name, cmd.new_a);
    strncat(new_name, star, fm.star_length);
    strcat(new_name, cmd.new_c);

	return new_name;
}

void preview(Command &cmd){

	if (fc == 0) return;

	printf("These changes would happen\n");
	for (int i = 0; i < fc; i++){
		const char *filename = files_matched[i].filename; 
		const char *new_name = construct_new_name(files_matched[i], cmd);

		printf("%s -> %s\n", filename, new_name);
	}
}

bool confirm(bool force_flag){
	if (fc == 0) return 0;
	if (force_flag)  return 1;
	printf("do you want to rename all of these files? (y/n) ");

	char confirm_flag;
	scanf("%c", &confirm_flag);
	
    return confirm_flag == 'n'? 0: 1;
}

void rename_files(bool cflag, Command &cmd){
	if (!cflag) return;

	char *path = new char[100];
	path = strcpy(path, cmd.d);

    if (path[strlen(path)-1] != '/') {
        path = strcat(path, "/");
    }

	for (int i = 0; i < fc; i++){
        
		char *filename_with_path = new char[200];
        strcpy(filename_with_path, path);
        strcat(filename_with_path, files_matched[i].filename);

		char *new_name_with_path = new char[200];
        strcpy(new_name_with_path, path);
        strcat(new_name_with_path, construct_new_name(files_matched[i], cmd));

        printf("rename: %s -> %s\n", filename_with_path, new_name_with_path);
		int r = rename(filename_with_path, new_name_with_path);
		if (r) {
			printf("failed to rename %s\n", filename_with_path);

		} else {
			printf("renamed %s successfully\n", filename_with_path);
		}
		delete []filename_with_path;
		delete []new_name_with_path;
	}
	delete []path;
} 

int main(int argc, char *argv[]){

	struct Command cmd = parsing_command(argc, argv);
	
    printf("cmd:\n");
    printf("force_flag: %d\n", cmd.force_flag);
    printf("show_help: %d\n", cmd.show_help);
    printf("d: %s\n", cmd.d);
    printf("old_name: %s\n", cmd.old_name);
    printf("new_name: %s\n", cmd.new_name);
    printf("old_a: %s\n", cmd.old_a);
    printf("old_c: %s\n", cmd.old_c);
    printf("new_a: %s\n", cmd.new_a);
    printf("new_c: %s\n", cmd.new_c);

	if (cmd.show_help){
		print_usage();
		return 0;
	}

	find_files(cmd.d, cmd.old_a, cmd.old_c);
	preview(cmd);
	bool cflag = confirm(cmd.force_flag);

	rename_files(cflag, cmd);

	return 0;
}	

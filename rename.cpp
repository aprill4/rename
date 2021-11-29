#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>

char *filenames[50];

struct command{
	bool force_flag = 0;
	bool show_help = 0;
	const char *d = ".";
	char *old_name;
	char *new_name;
};

struct command* parsing_command(int argc, char *argv[]){
	struct command *tmp = new struct command;
	
	if (argc == 1){
		return NULL;
	}

	for (int i = 1; i < argc; i++){

		if (strcmp(argv[i], "-h") == 0){
			tmp->show_help = 1;
			return tmp;
		}

		else if (strcmp(argv[i], "-f") == 0){
			tmp->force_flag = 1;
		}

		else {
			tmp->old_name = argv[i];

			if (i+1 < argc){
				tmp->new_name = argv[i+1];
				i += 1;
				printf("old name: %s\n", tmp->old_name);
				printf("new name: %s\n", tmp->new_name);
			}
			else{
				//TODO: error when no new filename
				//printf("");
			}
		}
	}

	printf("force_flag %d\n", tmp->force_flag);
	
	return tmp;
}

void find_files(const char *mydir, const char *old_name){
	DIR *d;
	struct dirent *dir;
	d = opendir(mydir);
	if (d){

		int fc = 0;

		while ((dir = readdir(d)) != NULL){
			if (strcmp(dir->d_name, old_name) == 0){
				printf("%s\n", dir->d_name);
				filenames[fc++] = dir->d_name;
			}
		}
		if (fc == 0){
			printf("no such files\n");
		}
		else{
			printf("found %d files as above\n", fc);
		}
	}
}

void rename_files(bool cflag, int len, const char *new_name){
	if (!cflag) return;
	for (int i = 0; i < len; i++){
		int r = rename((const char *)filenames[i], new_name);
		if (r != 0) {
			printf("failed to rename %s\n", filenames[i]);
		}
		else {
			printf("renamed %s successfully\n", filenames[i]);
		}
	}
} 

bool confirm(bool fflag){
	if (fflag) {
		return 1;
	}
	printf("do you want to rename all of these files? (y/n) ");

	char confirm_flag;
	scanf("%c", &confirm_flag);
	
	if (confirm_flag == 'n') return 0;

	return 1;
}

void print_usage(){

	printf("Usage: rename [OPTION]... [OLD_FILENAME] [NEW_FILENAME]\n");
	printf("Rename old filename to new filename (the current directory by default).\n");
	printf("\n");
	printf("only short options.\n");
	printf("    -f,\tforce to rename\n");
	printf("    -h,\tdisplay this help and exit\n");
	printf("can use \"*\" in filename to match any characters.\n");
}

int main(int argc, char *argv[]){

	struct command *c = parsing_command(argc, argv);
	
	if (!c || c->show_help){
		print_usage();
		return 0;
	}

	find_files(c->d, c->old_name);
	bool cflag = confirm(c->force_flag);

	rename_files(cflag, 1, c->new_name);

	return 0;
}	

// $ g++ -o # @

#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>

//struct contains a string and 2 integers for position recording
char *filenames[50];

struct command{
	bool force_flag = 0;
	bool show_help = 0;
	bool error_occurs = 0;
	const char *d = ".";
	char *old_name;
	char *new_name;
};

struct command* parsing_command(int argc, char *argv[]){
	struct command *tmp = new struct command;
	
	if (argc == 1){
		tmp->show_help = 1;
		return tmp;
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
				tmp->error_occurs = 1;
				printf("error, an old name must be followed by a new name\n");
				return tmp;
			}
		}
	}

	printf("force_flag %d\n", tmp->force_flag);
	
	return tmp;
}


//TODO: auto format
bool match_filename(const char *old_name, const char *filename){
	const char* f = filename;
	const char* o = old_name;

	while ( *o == *f  && *o != '\0' && *f != '\0'){
		o++;
		f++;
	}
	
	if (*o != '\0') {
		if (*o != '*') return 0;
		o++;
		if (*f == '\0' && *o != '\0') return 0;
		if (*f == '\0' && *o == '\0') return 1;
		int c = 0;
		while (*f != '\0') {
			if (*f == *o && c != 0) {
				f += 1;
				if (*f == '\0') return 0;
				f -= c;
				o -= c;
			}

			while (*f != *o && *f != '\0') {
				f++;
			}
			
			if (*f == '\0' && *o != '\0') return 0;
			if (*f == '\0' && *o == '\0') return 1;

			c = 0;
			while (*f == *o && *(f+1) != '\0' && *(o+1) != '\0') {
				f++;
				o++;
				c++;
			}
			if (*f == *o && *(f+1) == '\0' && *(o+1) == '\0') {
				return 1;
			}
		}
		return 1;
	}
	if (*f != '\0') return 0;
	return 1;
}

void find_files(const char *mydir, const char *old_name){
	DIR *d;
	struct dirent *dir;
	d = opendir(mydir);
	if (d){

		int fc = 0;
		//TODO: when * in filename, compares filename char by char
		while ((dir = readdir(d)) != NULL){
			if (match_filename(old_name, dir->d_name)){
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
	
	if (c->show_help){
		print_usage();
		return 0;
	}
	else if (c->error_occurs){
		return 0;
	}


	/*
	while (1) {
		scanf("%s %s", o, f);
		int tf = match_filename(o, f);
		printf("%d\n", tf);
	}
	*/	
	find_files(c->d, c->old_name);
	//bool cflag = confirm(c->force_flag);

	//rename_files(cflag, 1, c->new_name);

	return 0;
}	

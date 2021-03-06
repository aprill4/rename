// $ g++ -o # @
//TODO: hope it can rename files with more stars

#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>

//struct contains a string and 2 integers for position recording
struct file_matched{
	const char *filename;
	int star_begins = -1; 
	int star_ends = -1;
}files_matched[50];

int fc = 0;

struct command{
	bool force_flag = 0;
	bool show_help = 0;
	bool error_occurs = 0;
	const char *d = "."; 
	char *old_name = NULL;
	char *new_name = NULL;
};

void print_usage(){

	printf("Usage: rename [OPTION]... [OLD_FILENAME] [NEW_FILENAME]\n");
	printf("Rename old filename to new filename (the current directory by default).\n");
	printf("\n");
	printf("Options:\n");
	printf("    -f,\tforce to rename\n");
	printf("    -d,\tspecify a directory, must be followed by a directory name\n");
	printf("    -h,\tdisplay this help and exit\n");

	printf("\nHINT:\ncan use \"*\" in filename to match any characters, but when using \"*\" remember to quote the filename and only one star can be matched.\n");
}

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
			continue;
		}
		else if (strcmp(argv[i], "-d") == 0){
			if (!(i + 1 < argc)){
				tmp->show_help = 1;
				return tmp;
			}
			else if(!strcmp(argv[i+1], "-f") || !strcmp(argv[i+1], "-h")){
					tmp->show_help = 1;
					return tmp;
			}
			else{ 
				tmp->d = argv[i+1];
				i++;
				continue;
			}
		}
		else {
			tmp->old_name = argv[i];

			if (i + 1 < argc) {
				tmp->new_name = argv[i+1];
				i++;
			}
			else{
				tmp->error_occurs = 1;
				printf("error, an old name must be followed by a new name\n");
				return tmp;
			}
		}
	}

	if (tmp->old_name == NULL || tmp->new_name == NULL){
		tmp -> show_help = 1;
		return tmp;
	}
	return tmp;
}

struct file_matched* match_filename(const char *old_name, const char *filename){
	const char* f = filename;
	const char* o = old_name;
	struct file_matched *p = new struct file_matched;

	int s = 0;
	while ( *o == *f  && *o != '\0' && *f != '\0'){
		o++;
		f++;
		s++;
	}
	
	if (*o != '\0') {
		if (*o != '*') {
			return NULL;
		}

		o++;

		if (*f == '\0' && *o != '\0') return NULL;
		if (*f == '\0' && *o == '\0') {
			p -> filename = filename; 
			p -> star_begins = s;
			// star_begins = -1 and star_ends = -1
			// which means each character of the filename was matched with the specified pattern
		}
		int c = 0;
		int e = 0;
		while (*f != '\0') {
			if (c > 0){
				if (*f == *o) {
					f += 1;
					if (*f == '\0') return NULL;
					f -= c;
				}
				o -= c;
				e += c;
			}

			while (*f != *o && *f != '\0') {
				f++;
				e++;
			}
			
			if (*f == '\0' && *o != '\0') return NULL;
			if (*f == '\0' && *o == '\0') {
				p -> filename = filename;	
				p -> star_begins = (s == 0) ? -1 : s; 
				return p;
			}

			c = 0;
			while (*f == *o && *(f+1) != '\0' && *(o+1) != '\0') {
				f++;
				o++;
				c++;
			}
			if (*f == *o && *(f+1) == '\0' && *(o+1) == '\0') {
				p -> filename = filename;
				p -> star_begins = (s == 0) ? -1 : s; 
				p -> star_ends = e;
				return p;
			}
		}
	}
	if (*f != '\0') return NULL;
	p -> filename = filename;
	return p;
}

void find_files(const char *mydir, const char *old_name){
	DIR *d;
	struct dirent *dir;
	d = opendir(mydir);

	if (d){
		struct file_matched *fm = new file_matched;

		while ((dir = readdir(d)) != NULL){
			fm = match_filename(old_name, dir->d_name);
			if (fm != NULL){
				printf("%s\n", fm->filename);
				files_matched[fc++] = *fm;
			}
		}
		if (fc == 0){
			printf("No such files\n");
		}
		else{
			printf("Found ");
			fc > 1 ? printf("%d files", fc) : printf("1 file");
			printf(" as above\n");
		}
		delete fm;
	}
	else{
		printf("No such directory.\n");
	}
}

char *construct_new_name(file_matched fm, const char *new_name){

	const char *c = new_name;
	int s = 0;
	int e = 0;
	while (*c != '\0'){
		if (*c == '*'){
			e = s;
			break;
		}
		else{
			c++;
			s++;
		}
	}
	int star_begins = fm.star_begins;
	star_begins = (star_begins == -1) ? 0 : star_begins;
	int star_ends = fm.star_ends;

	const char *star = fm.filename + star_begins;
	char *c_new_name = new char[30];
	char *t = c_new_name;
	int i = 0;
	while (i < s){
		*t++ = *new_name++;
		i++;
	}

	if (e != 0){
		if (star_ends != -1){
			while (star_ends--){
				*t++ = *star++;
			}
		}
		else{
			while (*star != '\0'){
				*t++ = *star++;
			}
		}
		const char *after_star = new_name + 1;
		while (*after_star != '\0'){
			*t++ = *after_star++;
		}
	}
	*t= '\0';
	return c_new_name;
}

void preview(command *cmd){

	if (fc == 0) return;

	printf("These changes would happen\n");
	for (int i = 0; i < fc; i++){
		const char *c_new_name = construct_new_name(files_matched[i], cmd->new_name);

		const char *final_filename = files_matched[i].filename; 
		const char *final_new_name = c_new_name; 

		printf("%s -> %s\n", final_filename, final_new_name);
	}
}

bool confirm(bool fflag){
	if (fc == 0) return 0;
	if (fflag)  return 1;
	printf("do you want to rename all of these files? (y/n) ");

	char confirm_flag;
	scanf("%c", &confirm_flag);
	
	if (confirm_flag == 'n') return 0;

	return 1;
}

void rename_files(bool cflag, command *cmd){
	if (!cflag) return;

	char *d = (char *)cmd->d;
	char *path = new char[100];
	path = strcpy(path, d);

	while (*(d + 1)){ d++; }
	if (*d != '/'){
		path = strcat(path, "/");
	}

	for (int i = 0; i < fc; i++){
		const char *c_new_name = construct_new_name(files_matched[i], cmd->new_name);

		char *path_new_name = new char[100];
		path_new_name = strcpy(path_new_name, path);
		char *path_old_name = new char[100];
		path_old_name = strcpy(path_old_name, path);

		const char *final_filename = strcat(path_old_name, files_matched[i].filename);
		const char *final_new_name = strcat(path_new_name, c_new_name);

		int r = rename(final_filename, final_new_name);
		if (r != 0) {
			printf("failed to rename %s\n", files_matched[i].filename);
		}
		else {
			printf("renamed %s successfully\n", files_matched[i].filename);
		}
		delete []path_new_name;
		delete []path_old_name;
	}
	delete []path;
} 

int main(int argc, char *argv[]){
	///*
	struct command *c = parsing_command(argc, argv);
	
	if (c->show_help){
		print_usage();
		return 0;
	}
	else if (c->error_occurs){
		return 0;
	}
	//*/

	/*
	char o[20];
	char f[20];

	file_matched *fm = new file_matched;
	while (1) {
		scanf("%s %s", o, f);
		fm = match_filename(o, f);
		if (fm != NULL){
			printf("file matched: name: %s, star_begins: %d, star_ends: %d\n", fm->filename, fm->star_begins, fm->star_ends);
			files_matched[0] = *fm;
		}
		else{
			printf("no matched files\n");
		}
	}
	*/	
	find_files(c->d, c->old_name);
	preview(c);
	bool cflag = confirm(c->force_flag);

	rename_files(cflag, c);

	return 0;
}	

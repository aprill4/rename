// $ g++ -o # @ -g

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

			if (i + 1 < argc && !(i + 2 < argc)){
				tmp->new_name = argv[i+1];
				printf("old name: %s\n", tmp->old_name);
				printf("new name: %s\n", tmp->new_name);
			}
			else{
				tmp->error_occurs = 1;
				printf("error, options should come before filenames and an old name must be followed by a new name\n");
				return tmp;
			}
		}
	}

	printf("force_flag %d\n", tmp->force_flag);
	
	return tmp;
}


//TODO: auto format
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
			// star_begins = -1 and star_ends = -1
			// which means each character of the filename was matched with the specified pattern
		}
		int c = 0;
		int e = s;
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
				//printf("filename: %s, star_begins: %d\n", p->filename, p->star_begins);
				// p -> star_ends = -1, which means star starts at s until string ends
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
				p -> star_ends = e - 1;
				//printf("filename: %s, star_begins: %d, star_ends: %d\n", p->filename, p->star_begins, p->star_ends);
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

		int fc = 0;
		file_matched *fm = new file_matched;
		//TODO: when * in filename, compares filename char by char
		while ((dir = readdir(d)) != NULL){
			if (fm = match_filename(old_name, dir->d_name) != NULL){
				printf("%s\n", dir->d_name);
				files_matched[fc++] = *fm;
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

		int r = rename((const char *)files_matched[i], new_name);
		if (r != 0) {
			printf("failed to rename %s\n", files_matched[i].filename);
		}
		else {
			printf("renamed %s successfully\n", files_matched[i].filename);
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
	printf("can use \"*\" in filename to match any characters, but when using \"*\" remember to quote the filename!\n");
}

int main(int argc, char *argv[]){
	/*
	struct command *c = parsing_command(argc, argv);
	
	if (c->show_help){
		print_usage();
		return 0;
	}
	else if (c->error_occurs){
		return 0;
	}
	*/

	///*
	char o[20];
	char f[20];

	file_matched *fm = new file_matched;
	while (1) {
		scanf("%s %s", o, f);
		fm = match_filename(o, f);
		if (fm != NULL){
			printf("file matched: name: %s, star_begins: %d, star_ends: %d\n", fm->filename, fm->star_begins, fm->star_ends);
		}
		else{
			printf("no matched files\n");
		}
	}
	//*/	
	//find_files(c->d, c->old_name);
	//bool cflag = confirm(c->force_flag);

	//rename_files(cflag, 1, c->new_name);

	return 0;
}	

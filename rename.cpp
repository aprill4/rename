#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>

char *filenames[50];

void find_files(const char *old_name){
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	if (d){
		//printf("find files %s\n", old_name);
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

//TODO: CONFIRM 
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
int main(int argc, char *argv[]){

	bool force_flag = 0;
	bool show_help = 0;
	const char *old_name;
	const char *new_name;
	
	for (int i = 1; i < argc; i++){

		if (strcmp(argv[i], "-f") == 0){
			force_flag = 1;
		}
		
		else if (strcmp(argv[i], "-h") == 0){
			show_help = 1;
		}

		else {
			old_name = argv[i];
			new_name = argv[i+1];
			i += 1;
			printf("old name: %s\n", old_name);
			printf("new name: %s\n", new_name);
		}
	}

	printf("force_flag %d\n", force_flag);
	printf("show_help %d\n", show_help);

	find_files(old_name);
	bool cflag = confirm(force_flag);

	rename_files(cflag, 1, new_name);

	return 0;
}	

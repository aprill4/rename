// $ clang++ -o # @ -std=c++11 -g
// TODO: hope it can rename files with more stars

#include <cstring>
#include <dirent.h>
#include <map>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

class FileMatched {
public:
  const char *filename = nullptr;
  const char *new_name = nullptr;
  int star_begins = -1;
  int star_length = -1;

  ~FileMatched() { delete[] new_name; }
};

class Command {
public:
  bool force_flag = 0;
  bool show_help = 0;
  const char *dir = ".";
  const char *old_name = nullptr;
  const char *new_name = nullptr;
};

void print_usage() {

  printf("Usage: rename [OPTION]... [OLD_FILENAME] [NEW_FILENAME]\n");
  printf("Rename old filename to new filename (the current directory by "
         "default).\n");

  printf("\nOptions:\n");
  printf("    -f,\tforce to rename\n");
  printf("    -d,\tfollowed by a directory name string specify a directory\n");
  printf("    -h,\tdisplay this help and exit\n");

  printf("\nHINT:\n");
  printf("    1. \"*\" in filename can match any characters, but remember to "
         "double quote the filename when using it and only one star can be "
         "used so far.\n");
  printf("    2. An old name must be followed by a new name!\n");
}

Command parse_command(int argc, char *argv[]) {
  Command cmd;

  if (argc == 1) {
    cmd.show_help = 1;
    return cmd;
  }

  for (int i = 1; i < argc; i++) {

    if (!strcmp(argv[i], "-h")) {
      cmd.show_help = 1;
      return cmd;

    } else if (!strcmp(argv[i], "-f")) {
      cmd.force_flag = 1;
      continue;

    } else if (!strcmp(argv[i], "-d")) {
      if (!(i + 1 < argc)) {
        cmd.show_help = 1;
        return cmd;

      } else if (!strcmp(argv[i + 1], "-f") || !strcmp(argv[i + 1], "-h")) {
        cmd.show_help = 1;
        return cmd;

      } else {
        cmd.dir = argv[i + 1];
        i++;
        continue;
      }

    } else {
      cmd.old_name = argv[i];

      if (i + 1 < argc) {
        cmd.new_name = argv[i + 1];
        i++;
        continue;

      } else {
        cmd.show_help = 1;
        return cmd;
      }
    }
  }

  if (!cmd.old_name || !cmd.new_name) {
    cmd.show_help = 1;
    return cmd;
  }

  return cmd;
}

FileMatched *match_files(const char *filename, const char *pattern) {

  FileMatched *matched_file = new FileMatched;

  const char *star_pos = strchr(pattern, '*');

  if (!star_pos) {
    if (!strcmp(filename, pattern)) {
      matched_file->filename = filename;
      return matched_file;

    } else {
      return nullptr;
    }
  }

  size_t len_filename = strlen(filename);
  size_t a = len_filename - strlen(star_pos);
  size_t c = strlen(star_pos) - 1;

  if (strncmp(pattern, filename, a)) {
    return nullptr;
  }

  matched_file->star_begins = a;
  matched_file->filename = filename;

  if (c == 0) {
    matched_file->star_length = len_filename - a;
    return matched_file;
  }

  size_t star_ends = len_filename - c;

  if (strncmp(filename + star_ends, pattern + a + 1, c)) {
    return nullptr;
  }

  matched_file->star_length = star_ends - matched_file->star_begins;
  return matched_file;
}

std::pair<int, std::vector<FileMatched>> find_files(const char *mydir,
                                                    const char *pattern) {
  DIR *d;
  struct dirent *dir;
  d = opendir(mydir);

  std::vector<FileMatched> matched_files;

  if (!d) {
    // printf("No such directory!\n");
    return std::make_pair(-1, matched_files);
  }

  FileMatched *fm;

  while ((dir = readdir(d)) != NULL) {
    // ignore . and ..
    if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
      continue;
    }

    fm = match_files(dir->d_name, pattern);
    if (fm) {
      // printf("%s\n", fm->filename);
      matched_files.push_back(*fm);
      delete fm;
    }
  }

  if (matched_files.size() == 0) {
    return make_pair(0, matched_files);
    // printf("No such files\n");
  }

  return make_pair(1, matched_files);
}

bool exceptions_handler(int status_code) {
  if (status_code == -1) {
    printf("No such directory!\n");
    return 0;

  } else if (status_code == 0) {
    printf("No such files\n");
    return 0;

  } else {
    return 1;
  }
}

bool construct_new_name(std::vector<FileMatched> &matched_files,
                        const char *new_name) {

  const char *star_pos = strchr(new_name, '*');

  int files_count = matched_files.size();

  if (!star_pos) {
    if (files_count > 1) {
      return false;
    } else {
      matched_files[0].new_name = new char[strlen(new_name) + 1];
      strcpy((char *)matched_files[0].new_name, new_name);
      return true;
    }
  }

  for (int i = 0; i < files_count; i++) {

    const char *star = matched_files[i].filename + matched_files[i].star_begins;

    size_t len_a = strlen(new_name) - strlen(star_pos);
    size_t len_c = strlen(star_pos) - 1;

    matched_files[i].new_name =
        new char[len_a + len_c + matched_files[i].star_length + 1];

    strncpy((char *)matched_files[i].new_name, new_name, len_a);
    strncat((char *)matched_files[i].new_name, star,
            matched_files[i].star_length);
    strncat((char *)matched_files[i].new_name, new_name + len_a + 1, len_c);
  }

  return true;
}

void preview(std::vector<FileMatched> &matched_files,
             const char *new_name = nullptr) {

  int files_count = matched_files.size();

  if (files_count == 0)
    return;

  printf("\nThese changes would happen\n");
  for (int i = 0; i < files_count; i++) {
    if (new_name) {
      printf("%-10s ->  %-15s\n", matched_files[i].filename, new_name);

    } else {
      printf("%-10s ->  %-15s\n", matched_files[i].filename,
             matched_files[i].new_name);
    }
  }

  printf("\n");
}

bool confirm(bool force_flag, int files_count) {
  if (files_count == 0)
    return false;

  if (force_flag)
    return true;

  char confirm_flag;

  do {
    printf("do you want to rename all of these files? (y/n) ");
    while ((confirm_flag = getchar()) == '\n') {
    }

    switch (confirm_flag) {
    case 'y':
      return true;
    case 'n':
      return false;
    default:
      continue;
    }
  } while (1);
}

int rename_files(std::vector<FileMatched> &matched_files, const char *dir) {
  char *path;
  if (dir[strlen(dir) - 1] != '/') {
    path = new char[strlen(dir) + 2];
    strcpy(path, dir);
    strcat(path, "/");
  } else {
    path = new char[strlen(dir) + 1];
    strcpy(path, dir);
  }

  int path_len = strlen(path);

  int files_count = matched_files.size();
  for (int i = 0; i < files_count; i++) {

    int filename_len = strlen(matched_files[i].filename);
    int new_name_len = strlen(matched_files[i].new_name);

    char *filename_with_path = new char[filename_len + path_len + 1];
    strcpy(filename_with_path, path);
    strcat(filename_with_path, matched_files[i].filename);

    char *new_name_with_path = new char[new_name_len + path_len + 1];
    strcpy(new_name_with_path, path);
    strcat(new_name_with_path, matched_files[i].new_name);

    int r = rename(filename_with_path, new_name_with_path);

    if (r) {
      printf("failed to rename %s\n", filename_with_path);

    } else {
      printf("renamed %s successfully\n", filename_with_path);
    }

    delete[] filename_with_path;
    delete[] new_name_with_path;
  }

  delete[] path;

  return 1;
}

int main(int argc, char *argv[]) {

  // const char *array[] = {"./rename", "rename.cpp", "rename.cpp"};
  Command cmd = parse_command(argc, argv);

  if (cmd.show_help) {
    print_usage();
    return 0;
  }

  std::pair<int, std::vector<FileMatched>> result;
  result = find_files(cmd.dir, cmd.old_name);

  if (!exceptions_handler(result.first)) {
    return 1;
  }

  std::vector<FileMatched> matched_files = result.second;

  if (!construct_new_name(matched_files, cmd.new_name)) {
    preview(matched_files, cmd.new_name);
    printf("Rename more than one files to a single name is dangerous!\n");
    return 1;
  }

  preview(matched_files);

  if (confirm(cmd.force_flag, matched_files.size())) {

    if (!rename_files(matched_files, cmd.dir)) {
      return 1;
    }
  }

  return 0;
}

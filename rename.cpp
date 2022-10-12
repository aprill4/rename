// $ clang++ -o # @ -std=c++17 -g
// TODO: hope it can rename files with more stars

#include <cstring>
#include <dirent.h>
#include <iostream>
#include <map>
#include <optional>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

using std::string, std::cin, std::cout, std::endl;

struct FileMatched {
  string filename;
  size_t star_begins;
  int star_length = 0;

  void operator=(const FileMatched another) {
    filename.assign(another.filename);
    star_begins = another.star_begins;
    star_length = another.star_length;
  }
};

struct Command {
  bool force_flag = 0;
  bool show_help = 0;
  string dir = "."; // TODO: const
  string old_name;
  string new_name;
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
        cmd.dir.assign(argv[i]);
        i++;
        continue;
      }

    } else {
      cmd.old_name.assign(argv[i]);

      if (i + 1 < argc) {
        cmd.new_name.assign(argv[i + 1]);
        i++;
        continue;

      } else {
        cmd.show_help = 1;
        return cmd;
      }
    }
  }

  if (cmd.new_name.empty() || cmd.old_name.empty()) {
    cmd.show_help = 1;
    return cmd;
  }

  return cmd;
}

std::optional<FileMatched> match_files(const string filename,
                                       const string pattern) {

  size_t star_pos = pattern.find("*");

  std::optional<FileMatched> optfm;
  FileMatched fm;

  if (star_pos == string::npos) {
    if (filename == pattern) {
      fm.filename.assign(filename);

      optfm = fm;
      return optfm;

    } else {
      return std::nullopt;
    }
  }

  size_t star_following = pattern.size() - star_pos - 1;

  if (filename.compare(0, star_pos, pattern.substr(0, star_pos)) != 0) {
    return std::nullopt;
  }

  fm.star_begins = star_pos;
  fm.filename.assign(filename);

  if (star_following == 0) {
    fm.star_length = filename.size() - star_pos;
    optfm = fm;
    return optfm;
  }

  size_t star_ends = filename.size() - star_following;

  if (filename.compare(star_ends, star_following,
                       pattern.substr(star_pos + 1, star_following)) != 0) {
    return std::nullopt;
  }

  fm.star_length = star_ends - star_pos;
  optfm = fm;
  return optfm;
}

std::pair<int, std::vector<FileMatched>> find_files(string mydir,
                                                    string pattern) {
  DIR *d;
  struct dirent *dir;
  d = opendir(mydir.c_str());

  std::vector<FileMatched> matched_files;

  if (!d) {
    // printf("No such directory!\n");
    return std::make_pair(-1, matched_files);
  }

  while ((dir = readdir(d)) != NULL) {
    // ignore . and ..
    if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) {
      continue;
    } else if (auto fm = match_files(dir->d_name, pattern))
      // printf("%s\n", fm->filename);
      matched_files.push_back(fm.value());
  }

  if (matched_files.size() == 0) {
    return make_pair(0, matched_files);
    // printf("No such files\n");
  }

  return make_pair(1, matched_files);
}

bool if_has_file_matched(int status_code) {
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

std::optional<std::vector<std::string>>
construct_new_name(std::vector<FileMatched> &matched_files, string new_name) {

  size_t star_pos = new_name.find('*');

  int files_count = matched_files.size();
  std::optional<std::vector<string>> optvec = std::vector<string>(0);

  if (star_pos == string::npos) {
    if (files_count > 1) {
      return std::nullopt;

    } else {
      optvec.value().push_back(new_name);
      return optvec;
    }
  }

  for (int i = 0; i < files_count; i++) {

    const string star(matched_files[i].filename, matched_files[i].star_begins,
                      matched_files[i].star_length);

    string constructed_name(new_name, 0, star_pos);
    constructed_name += star + new_name.substr(star_pos + 1);

    optvec.value().push_back(constructed_name);
  }

  return optvec;
}

void preview(std::vector<FileMatched> matched_files,
             std::vector<std::string> new_names) {

  int files_count = matched_files.size();

  if (files_count == 0)
    return;

  printf("\nThese changes would happen\n");
  for (int i = 0; i < files_count; i++) {
    cout << matched_files[i].filename << " -> " << new_names[i] << endl;
  }

  printf("\n");
}

void preview(std::vector<FileMatched> matched_files,
             string new_name) {

  int files_count = matched_files.size();

  if (files_count == 0)
    return;

  printf("\nThese changes would happen\n");
  for (int i = 0; i < files_count; i++) {
    cout << matched_files[i].filename << " -> " << new_name << endl;
  }

  printf("\n");
}

bool confirm(bool force_flag) {
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

int rename_files(std::vector<FileMatched> &matched_files,
                 std::vector<string> &new_names, string dir) {

  char *path;

  if (dir.back() != '/') {
    dir += "/";
  }

  int files_count = matched_files.size();
  for (int i = 0; i < files_count; i++) {

    string filename_with_path = dir + matched_files[i].filename;
    string new_name_with_path = dir + new_names[i];

    int r = rename(filename_with_path.c_str(), new_name_with_path.c_str());

    if (r) {
      cout << "failed to rename " << filename_with_path << endl;
      return 0;

    } else {
      cout << "renamed " << filename_with_path << " successfully" << endl;
    }
  }

  return 1;
}

int main(int argc, char *argv[]) {

  // const char *array[] = {"./rename", "ha*.c", "haha*.c"};
  // Command cmd = parse_command(3, array);
  Command cmd = parse_command(argc, argv);

  if (cmd.show_help) {
    print_usage();
    return 0;
  }

  std::pair<int, std::vector<FileMatched>> result;
  result = find_files(cmd.dir, cmd.old_name);

  if (!if_has_file_matched(result.first)) {
    return 1;
  }

  std::vector<FileMatched> matched_files = result.second;

  /*
  for (auto file : matched_files) {
    cout << file.filename << endl;
  }
  */

  if (auto optvec = construct_new_name(matched_files, cmd.new_name)) {

    /*
    cout << "optvec\n";
    for (auto name: optvec.value()) {
      cout << name << endl;
    }
    */

    preview(matched_files, optvec.value());

    if (confirm(cmd.force_flag)) {

      if (!rename_files(matched_files, optvec.value(), cmd.dir)) {
        return 1;
      }
    }

  } else {
    preview(matched_files, cmd.new_name);
    printf("Rename more than one files to a single name is dangerous!\n");
    return 1;
  }

  return 0;
}

#include "src/file_util.h"
#include "boost/filesystem.hpp"
#include "glog/logging.h"
#include <dirent.h>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

using std::string;
using std::vector;

namespace protobuf_plugin_demo {

bool FileUtil::ListDir(const string &dir, vector<string> *files) {
  ::DIR *d;
  struct dirent *ent;
  vector<string> ret;
  if ((d = opendir(dir.c_str())) == nullptr) {
    LOG(ERROR) << "Fail to opendir " << dir;
    return false;
  }
  while ((ent = readdir(d)) != nullptr) {
    files->push_back(ent->d_name);
  }
  closedir(d);
  return true;
}

bool FileUtil::ListDirRecursive(const std::string &dir,
                                std::vector<std::string> *files) {
  ::DIR *d;
  struct dirent *ent;
  vector<string> ret;
  if ((d = opendir(dir.c_str())) == nullptr) {
    LOG(ERROR) << "Fail to opendir " << dir;
    return false;
  }
  while ((ent = readdir(d)) != nullptr) {
    string file_name = ent->d_name;
    if (file_name == "." || file_name == "..") {
      continue;
    }

    string full_path = dir + "/" + file_name;
    if (IsDirectory(full_path)) {
      ListDirRecursive(full_path, files);
    } else {
      files->push_back(full_path);
    }
  }
  closedir(d);
  return true;
}

bool FileUtil::IsDirectory(const string &dir) {
  struct stat st;
  return ::stat(dir.c_str(), &st) == 0 && ((st.st_mode & S_IFDIR) != 0);
}

string FileUtil::GetRealPath(const string &path) {
  return boost::filesystem::canonical(path).string();
}

string FileUtil::LoadContent(const string &file_name) {
  std::ifstream in(GetRealPath(file_name));
  if (!in) {
    LOG(ERROR) << "Fail to open " << file_name
               << ", rpath=" << GetRealPath(file_name);
    return "";
  }
  string ret;
  std::string line;
  while (getline(in, line)) {
    ret.append(line);
    ret.append("\n");
  }
  in.close();
  return ret;
}

vector<string> FileUtil::LoadLines(const string &file_name) {
  std::ifstream in(file_name);
  if (!in) {
    LOG(ERROR) << "Fail to open " << file_name;
    return vector<string>();
  }
  vector<string> ret;
  std::string line;
  while (getline(in, line)) {
    ret.push_back(line);
  }
  in.close();
  return ret;
}

void FileUtil::Replace(const string &from, const string &to, string *str) {
  size_t start_pos = str->find(from);
  if (start_pos == std::string::npos) {
    return;
  }
  str->replace(start_pos, from.length(), to);
}

string FileUtil::GetCurrentPath() {
  return boost::filesystem::current_path().string();
}

string FileUtil::TruncatePath(const string &src, const string &path) {
  string real_path = boost::filesystem::canonical(src).string();
  std::string::size_type n = real_path.find(path);
  if (n != string::npos) {
    return real_path.substr(0, n);
  }
  return "";
}
} // namespace protobuf_plugin_demo

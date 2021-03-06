#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
namespace protobuf_plugin_demo {
class FileUtil {
private:
  FileUtil() {}
  virtual ~FileUtil() {}

public:
  static bool ListDir(const std::string &dir, std::vector<std::string> *files);
  static bool ListDirRecursive(const std::string &dir,
                               std::vector<std::string> *files);
  static bool IsDirectory(const std::string &dir);
  static std::string GetRealPath(const std::string &path);
  static std::string LoadContent(const std::string &file_name);
  static std::vector<std::string> LoadLines(const std::string &file_name);
  static std::string GetCurrentPath();
  static void Replace(const std::string &from, const std::string &to,
                      std::string *str);
};

} /* namespace protobuf_plugin_demo*/

#endif /* FILE_UTIL_H_ */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <queue>
#include <filesystem>

//JSON  creating macros
#define JSON_INIT "{\n\t\"files\" : [\n"
#define JSON_ENDING "\n\t]\n}"
#define JSON_END_LINE "}"
#define JSON_OBJECT_START(url, name) "{\"url\":\"" url "\", \"name\":\"" name "\",\"contents\":[\n"
#define JSON_OBJECT_END "]}\n"

using std::string, std::vector, std::cout;
namespace fs = std::filesystem;

class Dir_Entry {
  std::vector<std::shared_ptr<Dir_Entry>> contents;
  bool is_dir;
  string file_path;
  string file_name;
  void m_createFileName();
public:
  Dir_Entry (string file_path, bool is_dir);
  inline void addFile(std::shared_ptr<Dir_Entry> file) { contents.push_back(file);}
  //getters
  inline const bool isDir() const { return is_dir; }
  inline const string getPath() const { return file_path; }
  inline const string getName() const { return file_name; }
};


class JSON_PARSER {
  string dir_path;
  std::shared_ptr<Dir_Entry> head_dir;
  void m_ParseDirs();
  void m_CreateJSON();
public:
  JSON_PARSER(string dir_path);
};
//
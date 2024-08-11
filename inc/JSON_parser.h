#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <queue>
#include <filesystem>
#include <sstream>
#include <fstream>
//JSON  creating macros
#define JSON_INIT "{\n\t\"files\" : [\n"
#define JSON_ENDING "\n\t]\n}"
#define JSON_END_LINE "}"
#define JSON_START_LINE "{\n"
#define JSON_OBJECT_START(url, name, tabulation) tabulation << "\"url\":\"" << url << "\",\n" << tabulation << "\"name\":\""  << name << "\",\n" << tabulation << "\"files\":["
#define JSON_OBJECT_END(tabulation) "]\n" << tabulation << "}"

using std::string, std::vector, std::cout;
namespace fs = std::filesystem; 

class Dir_Entry {
  std::vector<std::shared_ptr<Dir_Entry>> contents;
  bool is_dir;
  string file_path;
  string file_name;
  void m_createFileName();
public:
  Dir_Entry (const string& file_path, bool is_dir);
  inline void addFile(std::shared_ptr<Dir_Entry> file) { contents.push_back(file);}
  //getters
  inline const bool isDir() const { return is_dir; }
  inline const string getPath() const { return file_path; }
  inline const string getName() const { return file_name; }
  inline const size_t contentSize() const { return contents.size(); }
  inline std::shared_ptr<Dir_Entry> getContent(size_t index) { return contents[index]; }

  //debug
  inline void printContents()
  {
    for (const auto& file: contents)
      std::cout << file->file_path << "\n";
  }
};


class JSON_PARSER {
  string dir_path;
  std::shared_ptr<Dir_Entry> head_dir;
  void m_ParseDirs();
  void m_CreateJSON();
  void m_CreateJSONRec(std::shared_ptr<Dir_Entry> head, std::ostringstream& ss, int incantation, bool is_first = false);
public:
  JSON_PARSER(string dir_path);
};
//
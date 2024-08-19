#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <queue>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <algorithm>
//JSON  creating macros
#define JSON_INIT "{\n\t\"files\" : [\n"
#define JSON_ENDING "\n\t]\n}"
#define JSON_END_LINE "}"
#define JSON_START_LINE "{\n"
#define JSON_OBJECT_START(url, name, tabulation) tabulation << "\"url\":\"" << url << "\",\n" << tabulation << "\"name\":\""  << name << "\",\n" << tabulation << "\"files\":["
#define JSON_OBJECT_END(tabulation) "]\n" << tabulation << "}"

namespace fs = std::filesystem; 

class JSON_PARSER {
  std::string dir_path;
  void m_CreateJSON();
  void m_CreateJSONRec(std::string& head, std::ostringstream& ss, int incantation, bool is_first = false);
  std::string m_CreateFileName(std::string& url);
public:
  JSON_PARSER(std::string dir_path);
};

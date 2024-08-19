#include "../inc/JSON_parser.h"

JSON_PARSER::JSON_PARSER(std::string dir_path):dir_path(dir_path){
  m_CreateJSON();
}

void JSON_PARSER::m_CreateJSON()
{
  std::ostringstream ss;
  int incantation = 2;
  //add JSON header to a string stream 
  ss << JSON_INIT;
  m_CreateJSONRec(dir_path, ss, incantation, true);
  ss << JSON_ENDING;
  std::ofstream json_file("./.download.json");
  if (json_file.fail())
  {
    std::cout << "[ERROR]Could not create json file...\n[ERROR]Quiting program execution...\n";
  }
  json_file << ss.str();
  json_file.close();
}

void JSON_PARSER::m_CreateJSONRec(std::string& head_url, std::ostringstream& ss, int incantation, bool is_first)
{
  std::string tabulation(incantation, '\t');
  std::string name = m_CreateFileName(head_url);
  if (!is_first)
    ss << ",\n";
  ss << tabulation.substr(0, tabulation.length()-1) << JSON_START_LINE;
  ss << JSON_OBJECT_START(head_url, name, tabulation);
  fs::path entry{head_url};
  //if file is not directory, end json object and return
  if (!fs::is_directory(head_url))
  {
    ss << JSON_OBJECT_END(tabulation.substr(0, tabulation.length()-1));
    return;
  }
  ss << "\n";
  int counter = 0;
  //retrieve files urls
  std::vector<std::string> entries;
  for (auto& file : fs::directory_iterator(head_url))
  {
    entries.push_back(file.path().string());
    std::string child_url = file.path().string();
    
  }
  //recursively process directory entries in sorted order
  std::sort(entries.begin(), entries.end());
  for (auto& child_url : entries)
  {
    if(counter==0)
    {
      m_CreateJSONRec(child_url, ss, incantation+1,true);
    }
    else 
      m_CreateJSONRec(child_url, ss, incantation+1);
    counter++;
  }
  ss << "\n" << tabulation;
  ss << JSON_OBJECT_END(tabulation.substr(0, tabulation.length()-1));
}

std::string JSON_PARSER::m_CreateFileName(std::string& url)
{
  size_t file_len = url.length();
  std::string file_name;
  if (url[file_len-1] == '/')
  {
    url = url.substr(0, file_len-1);
    file_len--;
  }
  auto it = url.find_last_of('/');
  if (it != std::string::npos)
  {

    file_name = url.substr(it+1, file_len-1);
    return file_name;
  }  
  return url;
}

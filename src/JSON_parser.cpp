#include "../inc/JSON_parser.h"

JSON_PARSER::JSON_PARSER(string dir_path):dir_path(dir_path){
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
  string tabulation(incantation, '\t');
  string name = m_CreateFileName(head_url);
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
  //recursively process directory entries
  for (auto& file : fs::directory_iterator(head_url))
  {
    string child_url = file.path();
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

string JSON_PARSER::m_CreateFileName(string& url)
{
  size_t file_len = url.length();
  string file_name;
  if (url[file_len-1] == '/')
  {
    url = url.substr(0, file_len-1);
    file_len--;
  }
  auto it = url.find_last_of('/');
  if (it != string::npos)
  {

    file_name = url.substr(it+1, file_len-1);
    return file_name;
  }  
  return url;
}

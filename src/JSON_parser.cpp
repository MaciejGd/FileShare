#include "../inc/JSON_parser.h"


void Dir_Entry::m_createFileName()
{
  size_t file_len = file_path.length();
  if (file_path[file_len-1] == '/')
  {
    file_path = file_path.substr(0, file_len-1);
    file_len--;
  }
  auto it = file_path.find_last_of('/');
  if (it != string::npos)
  {

    file_name = file_path.substr(it+1, file_len-1);
    std::cout << "[INFO]File_name: " << file_name << "\n";
    return;
  }  
  file_name = file_path;
}

Dir_Entry::Dir_Entry(const string& file_path, bool is_dir):file_path(file_path),is_dir(is_dir)
{
  m_createFileName();
}

JSON_PARSER::JSON_PARSER(string dir_path):dir_path(dir_path),head_dir(std::make_shared<Dir_Entry>(Dir_Entry{dir_path, true})){
  m_ParseDirs();
  m_CreateJSON();
}

void JSON_PARSER::m_ParseDirs()
{
  //create dirs queue and add starting dir into it
  std::queue<std::shared_ptr<Dir_Entry>> dirs_queue;
  dirs_queue.push(head_dir);
  string temp_path;
  std::shared_ptr<Dir_Entry> temp_dir;
  bool is_dir = false;
  while (!dirs_queue.empty()) 
  {
    temp_dir = dirs_queue.front();
    temp_path = temp_dir->getPath();
    dirs_queue.pop();
    
    if (!temp_dir->isDir())
      continue;
    std::cout << "[INFO]Base dir path: " << temp_path << "\n";
    for (const auto& file : fs::directory_iterator(temp_path))
    {
      std::string file_path = temp_dir->getPath() + "/" + file.path().filename().string();
      //add end to check if dir entry is a directory (then it would be signalled inside of the JSON)
      if (file.is_directory())
        is_dir = true;
      else 
        is_dir = false;
      std::shared_ptr<Dir_Entry> entry = std::make_shared<Dir_Entry>(Dir_Entry{file_path, is_dir});
      temp_dir->addFile(entry);
      dirs_queue.push(entry);

    }
  }
} 

void JSON_PARSER::m_CreateJSON()
{
  std::ostringstream ss;
  int incantation = 2;
  //add JSON header to a string stream 
  ss << JSON_INIT;
  m_CreateJSONRec(head_dir, ss, incantation, true);
  ss << JSON_ENDING;
  std::ofstream json_file("./.download.json");
  if (json_file.fail())
  {
    std::cout << "[ERROR]Could not create json file...\n[ERROR]Quiting program execution...\n";
  }
  json_file << ss.str();
  json_file.close();
}



void JSON_PARSER::m_CreateJSONRec(std::shared_ptr<Dir_Entry> head, std::ostringstream& ss, int incantation, bool is_first)
{
  string tabulation(incantation, '\t');
  string path = head->getPath();
  string name = head->getName();
  if (!is_first)
    ss << ",\n";
  ss << tabulation.substr(0, tabulation.length()-1) << JSON_START_LINE;
  ss << JSON_OBJECT_START(path, name, tabulation);
  if (head->isDir())
  {
    ss << "\n";
  }
  for (size_t i = 0; i < head->contentSize(); i++)
  {
    if(i==0)
    {
      m_CreateJSONRec(head->getContent(i), ss, incantation+1,true);
    }
    else 
      m_CreateJSONRec(head->getContent(i), ss, incantation+1);
  }
  if (head->isDir())
    ss << "\n" << tabulation;
  ss << JSON_OBJECT_END(tabulation.substr(0, tabulation.length()-1));
}




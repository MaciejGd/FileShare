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

Dir_Entry::Dir_Entry(string file_path, bool is_dir):file_path(file_path),is_dir(is_dir)
{
  m_createFileName();
}

JSON_PARSER::JSON_PARSER(string dir_path):dir_path(dir_path){
  m_ParseDirs();
  m_CreateJSON();
}

void JSON_PARSER::m_ParseDirs()
{
  //create dirs queue and add starting dir into it
  std::queue<std::shared_ptr<Dir_Entry>> dirs_queue;
  dirs_queue.push(std::make_shared<Dir_Entry>(Dir_Entry{dir_path, true}));
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
      dirs_queue.push(std::make_shared<Dir_Entry>(Dir_Entry{file_path, is_dir}));
    }
  }
} 

void JSON_PARSER::m_CreateJSON()
{

}

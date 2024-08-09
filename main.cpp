#include "inc/http_tcpServer_linux.h"
#include "inc/JSON_parser.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

#define JSON_NAME ".download.json"

#ifdef _WIN32
#define FILE_CREATE
#elif __linux__
#define FILE_CREATE "touch " JSON_NAME
#endif

namespace fs = std::filesystem;

// //implementation of tree -> for parsing content of dir and filling JSON
// struct DirEntry {

// };



// std::string getFileName(const std::string& file_name)
// {
//   auto it = file_name.find_last_of('/');
//   if (it != std::string::npos)
//   {
//     return file_name.substr(it+1, file_name.length() - 1);
//   }
//   return file_name;
// }

// std::vector<std::vector<std::string>> parse_dirs(const std::string& dir)
// {
//   const fs::path input_dir{dir};
//   std::vector<std::vector<std::string>> recusrsive_dir;
//   std::vector<std::string> actual_dir;
//   for (const auto& file : fs::directory_iterator(input_dir))
//   {
//     std::string file_name = dir + file.path().filename().string();
//     //add end to check if dir entry is a directory (then it would be signalled inside of the JSON)
//     if (file.is_directory())
//       parse_dirs;
//     actual_dir.push_back(file_name);
//   }
// }



// //examine download directory and retrieved links to files stored in 
// std::vector<std::string> prepareArgsJSON()
// {
//   const fs::path dir{"download"};
//   std::vector<std::string> files_links;
//   for (const auto& file : fs::directory_iterator(dir))
//   {
//     std::string file_name = "./download/" + file.path().filename().string();
//     //add end to check if dir entry is a directory (then it would be signalled inside of the JSON)
//     if (file.is_directory())
//       file_name+="_dir";
//     files_links.push_back(file_name);
//   }
//   std::sort(files_links.begin(), files_links.end());
//   for (const auto& file : files_links)
//   {
//     std::cout << file << "\n";
//   }
//   return files_links;
// }

// void createJSON(const std::vector<std::string>& files)
// {
//   std::ofstream json_file(JSON_NAME);
//   if (json_file.fail())
//   {
//     std::cout << "[ERROR]Could not open a JSON file, quiting.\n";
//     exit(1);
//   }
//   json_file << "{\n";
//   json_file << "\t\"files\" : [\n";
//   std::string file_name;
//   bool is_dir;
//   for (int i = 0; i < files.size(); i++)
//   {
//     //directories handling
//     if (files[i].length() > 5)
//     {
//       std::string extension = files[i].substr(files[i].length() - 4, files[i].length() - 1);
//       if (extension == "_dir")
//       {
//         file_name = files[i].substr(0, files[i].length() - 4);
//         is_dir = true;
//       }
//       else {
//         file_name = files[i];
//         is_dir = false;
//       }
//     }
//     else {
//       file_name = files[i];
//       is_dir = false;
//     }
    
//     json_file << "\t\t{\"url\":\"" << file_name << "\"," << "\"name\":\""<< getFileName(file_name) << "\", \"dir\": "<< is_dir << "}";

//     if (i != files.size()-1)
//       json_file << ",\n";
//   }
//   json_file << "\n\t]\n";
//   json_file << "}";
//   json_file.close();
// }


int main(int argc, const char** argv)
{
  // createJSON(prepareArgsJSON());  
  // if (argc == 1)
  // {
  //   http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  // }
  // else if (argc == 2)
  // {
  //   http::TcpServer server = http::TcpServer("0.0.0.0", 8080, std::string(argv[1]));
  // }
  // else {
  //   std::cout << "Too much input arguments, terminating program execution...\n";
  // }
  JSON_PARSER parser("./download/");


  return 0;
}

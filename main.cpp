#include "inc/http_tcpServer_linux.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

#define JSON_NAME ".download.json"

#ifdef _WIN32
#define FILE_CREATE
#elif __linux__
#define FILE_CREATE "touch JSON"
#endif

namespace fs = std::filesystem;

//examine download directory and retrieved links to files stored in 
std::vector<std::string> prepareArgsJSON()
{
  const fs::path dir{"download"};
  std::vector<std::string> files_links;
  for (const auto& file : fs::directory_iterator(dir))
  {
    files_links.push_back("./download/" + file.path().filename().string());
  }
  std::sort(files_links.begin(), files_links.end());
  for (const auto& file : files_links)
  {
    std::cout << file << "\n";
  }
  return files_links;
}

void createJSON(const std::vector<std::string>& files)
{
  if (system(FILE_CREATE))
  {
    std::cout << "[ERROR]Could not create a JSON file containing downloads, quiting.\n";
    exit(1);
  }
  std::ofstream json_file(JSON_NAME);
  if (json_file.fail())
  {
    std::cout << "[ERROR]Could not open a JSON file, quiting.\n";
    exit(1);
  }
  json_file << "{\n";
  json_file << "\"options\" : [";
  for (int i = 0; i < files.size(); i++)
  {
    json_file << "\"" << files[i] << "\"";
    if (i != files.size()-1)
      json_file << ", ";
  }
  json_file << "]\n";
  json_file << "}";
  json_file.close();
}


int main(int argc, const char** argv)
{
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
  //testing of new mechanism
  createJSON(prepareArgsJSON());  
  return 0;
}

#include "inc/http_tcpServer_linux.h"
#include "inc/JSON_parser.h"

void inputAnalyze(int argc, char **argv)
{
  //if statements can be used in here or some other solution 
  for (int i = 0; i < argc; i++)
  {
    std::cout << argv[i];
  }
  std::cout << "\n";
}


int main(int argc, const char** argv)
{
  //createJSON(prepareArgsJSON());  
  //prepare JSON from a download dir content
  //need to implement some kind of input processing 
  JSON_PARSER parser("./download/");
  if (argc == 1)
  {
    http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  }
  else if (argc == 2)
  {
    http::TcpServer server = http::TcpServer("0.0.0.0", 8080, std::string(argv[1]));
  }
  else {
    std::cout << "Too much input arguments, terminating program execution...\n";
  }
  
  return 0;
}

#include "inc/http_tcpServer_linux.h"
#include "inc/JSON_parser.h"

int main(int argc, const char** argv)
{
  //createJSON(prepareArgsJSON());  
  //prepare JSON from a download dir content
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

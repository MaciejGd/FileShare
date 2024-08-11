#include "inc/http_tcpServer_linux.h"
#include "inc/JSON_parser.h"

//what type of flags would i need??? how do you want server for downloading files look alike???
// -> specify if it is a file or a directory nah i think it would be ok, just path specify??
//    -> no need for specifying if file is a dir or not, program does not diffrentiate between dir and regular file
//    -> take "./download" directory by default
// -> removing output json file after closing a server??? can be handy if we want to clean afterward, to be honest maybe cleaning json file should actually be a default option
//    -> cons of cleaning by deafult, directory after cleaning is left as it was, if we add some flag we can left json as it was to save some time 
//	  -> can add some flag which would let not removing json(is it even worth???) -> json has to be created every time i run a server so i dont think there is s aneed for that
//
//
//
//
//
//
//
// -f -> specify path
// --remove-json -> removes json file after execution
// - 
//
//
//
//
//


void inputAnalyze(const char* argv)
{
  //if statements can be used in here or some other solution 
  if (argv == "\n")
  {

  }
}


int main(int argc, const char** argv)
{
  for (int i = 1; i < argc; i++)
    inputAnalyze(argv[i]);
  JSON_PARSER parser("./download/");
  http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  system("rm -f ./download.json");
  return 0;
}

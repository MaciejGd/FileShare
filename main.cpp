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
bool HOSTAPP = false;
bool HELP = false;
std::string APP_PATH = "";
std::string DOWNLOAD_PATH = "";

void inputAnalyze(int argc, const char** argv)
{
  //if statements can be used in here or some other solution 
  std::string prev="";
  std::string actual="";
  for (int i = 1; i < argc; i++)
  {
    actual = argv[i];
    //hosting html file
    if (prev == "--serve" || prev == "-s")
    {
      APP_PATH = argv[i];
      break;
    }
    else if (prev == "--path" || prev == "-p")
    {
      DOWNLOAD_PATH = argv[i];
      break;
    }
    else if (actual == "--help" || actual == "-h")
    {
      HELP = true;
      break;
    }
    prev = argv[i];
    //std::cout << prev << std::endl;
  }
}

int main(int argc, const char** argv)
{
  //testing input analyzis
  for (int i = 1; i < argc; i++)
  {
    //std::cout << argv[i] << std::endl;
  }
  inputAnalyze(argc, argv);
  if (HELP)
  {
    std::cout << "\n*************\n";
    std::cout <<     "* FILESHARE *\n";
    std::cout <<     "*************\n\n";
    std::cout << "FileShare application is used for hosting server in local network. Simple web code has been embedded inside of the server code. Create directory named download and simply type fileServer to host server on your machine. On another device connected to the same network enter url \"server_ip\":8000 to access and download files from download directory from another device. If passed no arguments \"download\" directory is taken by default. You can add -p or --path flag to specify directory you want to share.\n\n" << std::endl;
    std::cout << "\t-p or --path  : specify path of hosted directory after the flag\n";
    std::cout << "\t-s or --serve : use application as server for website, specify index.html file after the flag\n";
    std::cout << "\t-h or --help  : show info about application\n\n";
    return 0;
  }
  else if (APP_PATH!="")
  {
    http::TcpServer server = http::TcpServer("0.0.0.0", 8080, std::string(argv[1]));
    return 0;
  }
  std::cout << DOWNLOAD_PATH << std::endl;
  if (DOWNLOAD_PATH!="")
  {
    JSON_PARSER parser(DOWNLOAD_PATH);
  }
  else {
    JSON_PARSER parser("./download/");
  }
  http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  
  // for (int i = 1; i < argc; i++)
  //   inputAnalyze(argv[i]);
  // JSON_PARSER parser("./download/");
  // if (argc == 2)
  //   http::TcpServer server = http::TcpServer("0.0.0.0", 8080, std::string(argv[1]));
  // else 
  //   http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  // system("rm -f ./download.json");
  return 0;
}

#include "../inc/http_tcpServer_linux.h"
#include "../inc/web_page.h"
#include <fcntl.h>

namespace http
{
//static instance initialization
TcpServer* TcpServer::s_instance = nullptr;

void TcpServer::m_log(const std::string& msg)
{
  std::cout << "[LOG] " << msg << "\n";
}

void TcpServer::m_exitWithError(const std::string& msg)
{
  std::cout << "[ERROR] " << msg << "\n";
  m_closeServer();
  exit(1);

}

std::string TcpServer::m_getMimeType(const std::string &filename)
{
  size_t dot;
  std::string extension;
  if ((dot = filename.find_last_of('.')))
    extension = filename.substr(dot+1, filename.size()-1);
  if (!extension.empty())
  {
    std::cout << "Mime type requested: " << mime_type[extension] << "\n";
    return mime_type[extension];
  }
  return "text/html";
}

http::TcpServer::TcpServer(const char* ip, uint32_t port, std::string file_name):m_ip_address(ip), m_port(port), 
  m_incomingMessage(), main_file(file_name), m_serverMessage()
{
  s_instance = this;
  //handling CTRL-C and CTRL-Z signals
  std::signal(SIGINT, m_signalHandler);
  //std::signal(SIGTSTP, m_signalHandler);
  #ifdef WIN
  int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0)
  {
    m_exitWithError("Failed to startup a WINSOCK, error code: " + std::to_string(iResult) + "\n");
  }
  #endif
  //init windows members
  m_fillSocketAddr();
  m_startServer();
  //setting socket options (reusing not used open socket)
  #ifdef LINUX
  int opt = 1;
  if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    m_exitWithError("Failed to set SO_REUSEADDR.");
  }
  #endif

  m_bind();

  
  m_startListen();
}

http::TcpServer::~TcpServer()
{
  m_cleanZippedDirs();
  m_closeServer();
}

//NOT TOUCH
void TcpServer::m_evaluateProjDir()
{
  int last_backslash_pos = main_file.find_last_of('/');
  if (last_backslash_pos != std::string::npos)
  {
    proj_dir = main_file.substr(0, last_backslash_pos);
    return;
  }
  //return empty if not project directory specified
  proj_dir = "";
  
}

uint8_t TcpServer::m_startServer()
{
  //linux implementation
  #ifdef LINUX
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0)
  {
    close(m_socket);
    m_exitWithError("Opening a server's socket failed... shutting down");
  }
  std::cout << "Properly opened server's socket, the socket file descriptor: " << m_socket << "\n";
  return 0;
  //windows implementation
  #elif defined(WIN)
  m_listenSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
  if (m_listenSocket == INVALID_SOCKET)
  {
    freeaddrinfo(m_result);
    WSACleanup();
    m_exitWithError("Error at socket() " + std::to_string(WSAGetLastError()) + "\n");
  }
  return 0;
  #endif
}

void TcpServer::m_closeServer()
{
  #ifdef LINUX
  close(m_socket);
  close(m_new_socket);
  #elif defined(WIN)
  closesocket(m_listenSocket);
  WSACleanup();
  #endif
}

void TcpServer::m_fillSocketAddr()
{
  #ifdef LINUX
  m_socketAddress_len = sizeof(m_socketAddress);
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address);
  #elif defined(WIN)
  ZeroMemory(&m_hints, sizeof(m_hints));
  m_hints.ai_family = AF_INET;
  m_hints.ai_socktype = SOCK_STREAM;
  m_hints.ai_protocol = IPPROTO_TCP;
  m_hints.ai_flags = AI_PASSIVE;
  int iResult = getaddrinfo(NULL, DEFAULT_PORT, &m_hints, &m_result);
  if (iResult != 0) {
    m_exitWithError("getaddrinfo failed: " + std::to_string(iResult) + "\n");
  }
  #endif
}

void TcpServer::m_bind()
{
  #ifdef LINUX
  if (bind(m_socket, (sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0)
  {
    close(m_socket);
    m_exitWithError("Failed to assign socket address to created socket.");
  }
  #elif defined(WIN)
  int iResult = bind(m_listenSocket, m_result->ai_addr, (int)m_result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(m_result);
    m_exitWithError("bind failed with error: " + std::to_string(WSAGetLastError()) + "\n");
  }
  freeaddrinfo(m_result);
  #endif
}

void TcpServer::m_startListen()
{
  std::string ss;
  #ifdef LINUX
  if (listen(m_socket, SOMAXCONN) < 0)
  {
    m_exitWithError("Socket listen failed");
  }
  ss = "\n*** Listening on ADDRESS: " + std::string(m_ip_address) + " PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)) + " ***\n"; 
  #elif defined(WIN)
  if ( listen( m_listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
    m_exitWithError( "Listen failed with error: " + std::to_string(WSAGetLastError()) + "\n");
  }
  ss = "\n*** Listening on ADDRESS: " + std::string(m_ip_address) + " PORT: " + std::to_string(ntohs(m_result->ai_addr->sa_family)) + " ***\n"; 
  #endif
   
  m_log(ss.c_str());

  while (true)
  {
    m_log("Waiting for a new connection...\n");
    #ifdef LINUX
    m_acceptConnection(m_new_socket);
    std::thread clientThread(&TcpServer::m_handleClient, this);
    #elif defined(WIN)
    m_clientSocket = accept(m_listenSocket, NULL, NULL);
    //actually here I am not sure i want to quit
    if (m_clientSocket == INVALID_SOCKET) {
      m_exitWithError("accept failed: " + std::to_string(WSAGetLastError()) + "\n");
    }
    //need to change that
    std::thread clientThread(&TcpServer::m_handleClient, this);
    #endif
    clientThread.detach();
  } 
}

void TcpServer::m_handleClient()
{
  #ifdef LINUX
  int new_socket = m_new_socket;
  #elif defined(WIN)
  SOCKET new_socket = m_clientSocket;
  #endif
  char buffer[BUFFER_SIZE] = {0};
  int64_t bytesReceived = 0;
  
  bytesReceived = recv(new_socket, buffer, BUFFER_SIZE, 0);
  std::string buff(buffer);  
  //end of testing
  if (bytesReceived < 0)
  {
    m_exitWithError("Failed to read bytes from client socket connection");
  }
  //handle POST request (dir download)
  if (buff.find("POST") != std::string::npos)
  {
    m_handleDirDownload(buff);
    m_sendResponse(new_socket);
    #ifdef LINUX
    close(new_socket);
    #elif defined(WIN)
    closesocket(new_socket);
    #endif
    return;
  }
  std::string url;
  std::smatch matches;
  std::regex regex = std::regex("GET /([^ ]*) HTTP/1");
  if (std::regex_search(buff, matches, regex))
  {
    url = matches[1].str();
    std::cout << "Requested MIME type is: " << url << std::endl;
    m_getMimeType(url);
  }
  //m_buildResponse(url);
  m_buildResponse(url);
  m_sendResponse(new_socket);
  #ifdef LINUX
  int closing_status = close(new_socket);
  #elif defined(WIN)
  int closing_status = closesocket(new_socket);
  #endif
}


void TcpServer::m_handleDirDownload(const std::string& buff)
{
  //find requested zip file in incoming json
  std::regex regex("\"directory\"\\s*:\\s*\"(.*?)\"");
  std::smatch matches;
  std::string url;
  std::cout << "BUFF\n" << buff << "\nENDOFBUFF\n";
  //match pattern to get url
  if (std::regex_search(buff, matches, regex))
    url = matches[1].str();
  std::string response;
  //created zip name
  std::string zip_archive = url + ".zip";
  //check if zip does not exist and create it if not 
  if (!fs::exists(zip_archive)) 
  {
    #ifdef LINUX
    std::string command = "zip -r " + url + " " + url;
    #elif defined(WIN)
    std::cout << "[LOG]Requested zip path: " << url << std::endl;
    std::string command = "powershell.exe -Command \"Compress-Archive -Path " + url + " -DestinationPath " + url + ".zip\"";
    #endif
    if (system(command.c_str()))
    {
      m_log("[ERROR]Failed to create zip from " + url);
      return;
    }
    //add zipped directory to a member vector
    zipped_dirs.push_back(zip_archive);
  }
  //open zip archive
  std::fstream file(zip_archive, std::ios::in);
  if (file.fail())
  {
    m_log("Failed to open zip for read, zip name: " + zip_archive);
  }
  //copy zip content to string variable
  std::string file_content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
  std::ostringstream ss;
  ss << "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/zip\r\n"
    << "Content-Length: " << file_content.size()
    << "\r\n\r\n";
  ss << file_content;
  m_serverMessage = ss.str();
}

void TcpServer::m_buildResponse()
{
  m_serverMessage = HTML::webpage;
}

void TcpServer::m_buildResponse(const std::string &file_name)
{
  //reading mime type of rrequest based on requested file_name
  std::string mime = m_getMimeType(file_name);
  std::string file_content;
  //handling requested file
  std::fstream file;
  if (file_name == "" && main_file == "")
  {
    file_content = HTML:: webpage;
  }
  else
  {
    if (file_name != "")
      file.open(proj_dir + file_name, std::ios::in); 
    else 
      file.open(main_file, std::ios::in);
  //building response
    if (file.fail())
    {
      std::string msg = "Could not find requested file: " + proj_dir + file_name;
      m_log(msg);
      m_serverMessage = "HTTP/1.1 404 Not Found\r\n"
                        "Content-Type: text/plain\r\n"
                        "\r\n"
                        "404 Not Found";
      return;
    }
    
    file_content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
  }
  std::stringstream ss;
  ss << "HTTP/1.1 200 OK\r\n"
    "Content-Type: " << mime << "\r\n"
    << "Content-Length: " << file_content.size()
    << "\r\n\r\n";
  ss << file_content;
  m_serverMessage = ss.str();
  //std::cout << m_serverMessage << std::endl;
}

#ifdef LINUX
void TcpServer::m_acceptConnection(int &new_socket)
{
    std::cout << "[INFO]Accepting incoming socket connection.\n";
    new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, (socklen_t*)&m_socketAddress_len);
    if (new_socket < 0) {
        std::string addr = inet_ntoa(m_socketAddress.sin_addr);
        std::string info = "Server failed to accept incoming connection from ADDRESS: " + addr + "; PORT: " + 
            std::to_string(ntohs(m_socketAddress.sin_port));
        m_exitWithError(info.c_str());
    }
    std::cout << "[INFO]Properly interpreted incoming connection on socket number: " << new_socket << "\n";
}

void TcpServer::m_sendResponse(int &new_socket)
{
  int64_t bytesSent;
  bytesSent = write(new_socket, m_serverMessage.c_str(), m_serverMessage.size());
  if (bytesSent == m_serverMessage.size())
    m_log("--- Server response sent to client ---\n*********************\n");
  else 
    m_log("Error sending response to client, some bytes has been lost...");
}
#elif defined(WIN)
void TcpServer::m_sendResponse(SOCKET &new_socket)
{
  int64_t bytesSent;
  bytesSent = send(new_socket, m_serverMessage.c_str(), m_serverMessage.size(), 0);
  if (bytesSent == m_serverMessage.size())
    m_log("--- Server response sent to client ---\n*********************\n");
  else 
    m_log("Error sending response to client, some bytes has been lost...");
}
#endif

//it is actually not used at all for now (used socket options for using unused)
void TcpServer::m_signalHandler(int signum)
{
  std::string prompt = "***** Process termination, SIG : " + std::to_string(signum) + " sent, closing server.";
  //remove json file as it is no longer needed after closing server
  #ifdef LINUX
  system("rm -f ./.download.json");
  #elif defined(WIN)
  system("del .download.json");
  #endif
  s_instance->m_cleanZippedDirs();
  s_instance->m_exitWithError(prompt.c_str());
}

//remove zipped dirs created for client to download
void TcpServer::m_cleanZippedDirs()
{
  for (int i = zipped_dirs.size()-1; i >= 0; i--)
  {
    #ifdef LINUX
    std::string dir_remove = "rm -r " + zipped_dirs[i];
    system(dir_remove.c_str());
    #elif defined(WIN)
    m_transformPathForWindows(zipped_dirs[i]);
    std::string dir_remove = "del \"" + zipped_dirs[i] + "\"";
    std::cout << "erasing content: \"" << dir_remove << "\"" << std::endl;
    system(dir_remove.c_str());
    
    #endif
  }
}

#ifdef WIN
void TcpServer::m_transformPathForWindows(std::string& path)
{
  //need to transform path to "windows" style to properly remove it
  for (int i = 0; i < path.size(); i++)
  {
    if (path[i]=='/')
      path[i] = '\\';
  }
}
#endif

}//namespace http

#include "../inc/http_tcpServer_linux.h"

namespace http
{
//NOT TOUCH
void log(const std::string& msg)
{
  std::cout << "[LOG] " << msg << "\n";
}
//NOT TOUCH
void exitWithError(const std::string& msg)
{
  std::cout << "[ERROR] " << msg << "\n";
  exit(1);
}
//NOT TOUCH
std::string getMimeType(const std::string &filename)
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
  m_incomingMessage(), main_file(file_name), m_socketAddress_len(sizeof(m_socketAddress)),
  m_serverMessage()
{
  //handling CTRL-C and CTRL-Z signals
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTSTP, signalHandler);

  #ifdef WIN
  #endif
  //init windows members
  m_fillSocketAddr();
  m_startServer();
  //setting socket options (reusing not used open socket)
  #ifdef LINUX
  int opt = 1;
  if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    exitWithError("Failed to set SO_REUSEADDR.");
  }
  #endif

  m_bind();

  
  startListen();
}

http::TcpServer::~TcpServer()
{
  m_closeServer(0);
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
    exitWithError("Opening a server's socket failed... shutting down");
  }
  std::cout << "Properly opened server's socket, the socket file descriptor: " << m_socket << "\n";
  return 0;
  //windows implementation
  #elif WIN
  m_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (m_listenSocket == INVALID_SOCKET)
  {
    freeaddrinfo(result);
    WSACleanup();
    exitWithError("Error at socket(): %s", WSAGetLastError());
  }
  return 0;
  #endif
}

void TcpServer::m_closeServer(int error_code)
{
  #ifdef LINUX
  close(m_socket);
  close(m_new_socket);
  exit(error_code);
  #elif WIN
  closesocket(m_listenSocket);
  WSACleanup();
  #endif
}

void TcpServer::m_fillSocketAddr()
{
  #ifdef LINUX
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address);
  #elif WIN
  ZeroMemory(&m_hints, sizeof(m_hints));
  m_hints.ai_family = AF_INET;
  m_hints.ai_socktype = SOCK_STREAM;
  m_hints.ai_protocol = IPPROTO_TCP;
  m_hints.ai_flags = AI_PASSIVE;
  int iResult = getaddrinfo(NULL, DEFAULT_PORT, &m_hints, &m_result);
  if (iResult != 0) {
    WSACleanup();
    exitWithError("getaddrinfo failed: %d\n", iResult);
  }
  #endif
}

void TcpServer::m_bind()
{
  #ifdef LINUX
  if (bind(m_socket, (sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0)
  {
    close(m_socket);
    exitWithError("Failed to assign socket address to created socket.");
  }
  #elif WIN
  int iResult = bind(m_listenSocket, m_result->ai.addr, (int)result->ai_addrlen);
  if (iResult == SOCKET_ERROR) {
    freeaddrinfo(result);
    closesocket(m_listenSocket);
    WSACleanup();
    exitWithError("bind failed with error: %d\n", WSAGetLastError());
  }
  freeaddrinfo(result);
  #endif
}

void TcpServer::startListen()
{
  #ifdef LINUX
  if (listen(m_socket, SOMAXCONN) < 0)
  {
    exitWithError("Socket listen failed");
  }
  #elif WIN
  if ( listen( m_listenSocket, SOMAXCONN ) == SOCKET_ERROR ) {
    printf( "Listen failed with error: %ld\n", WSAGetLastError() );
    closesocket(m_listenSocket);
    WSACleanup();
    return 1;
  }
  #endif
  std::string ss;
  ss = "\n*** Listening on ADDRESS: " + std::string(m_ip_address) + " PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)) + " ***\n"; 
  log(ss.c_str());

  while (true)
  {
    log("Waiting for a new connection...\n");
    #ifdef LINUX
    acceptConnection(m_new_socket);

    std::thread clientThread(&TcpServer::handleClient, this);
    #elif WIN
    m_clientSocket = accept(m_listenSocket, NULL, NULL);
    //actually here I am not sure i want to quit
    if (m_clientSocket == INVALID_SOCKET) {
      closesocket(m_listenSocket);
      WSACleanup();
      std::cout << "accept failed: " << WSAGetLastError() << "\n";
    }
    //need to change that
    std::thread clientThread(&TcpServer::handleClient, this, m_new_socket);
    #endif
    clientThread.detach();
  } 
}

void TcpServer::acceptConnection(int &new_socket)
{
  new_socket = accept(m_socket, (sockaddr *)&m_socketAddress, (socklen_t*)&m_socketAddress_len);
  if (new_socket < 0)
  {
    std::string addr = inet_ntoa(m_socketAddress.sin_addr);
    std::string info = "Server failed to accept incoming connection from ADDRESS: " + addr + "; PORT: " + 
        std::to_string(ntohs(m_socketAddress.sin_port));
    exitWithError(info.c_str());
  }
}

//have to check it more a little
void TcpServer::handleClient()
{
  int new_socket = m_new_socket;
  char buffer[BUFFER_SIZE] = {0};
  int64_t bytesReceived = 0;
  #ifdef LINUX
  bytesReceived = read(new_socket, buffer, BUFFER_SIZE);
  #elif WIN
  bytesReceived = recv(m_clientSocket, buffer, BUFFER_SIZE, 0);
  #endif
  log(std::string(buffer));
  if (bytesReceived < 0)
  {
    exitWithError("Failed to read bytes from client socket connection");
  }
  //creating regex for checking mime types requested
  std::regex regex("GET /([^ ]*) HTTP/1");
  std::smatch matches;
  std::string buff(buffer);
  std::string url;
  if (std::regex_search(buff, matches, regex))
  {
    url = matches[1].str();
    std::cout << "Found URL is: " << url << std::endl;
    getMimeType(url);
  }
  buildResponse(url);
  sendResponse(new_socket);
  close(new_socket);
}

//NOT TOUCH
void TcpServer::buildResponse(const std::string &file_name)
{
  //reading mime type of rrequest based on requested file_name
  std::string mime = getMimeType(file_name);
  //handling requested file
  std::fstream file;
  if (file_name == "")
    file.open(main_file, std::ios::in);
  else 
    file.open(proj_dir + file_name, std::ios::in); 

  //building response
  if (file.fail())
  {
    std::string msg = "Could not find requested file: " + proj_dir + file_name;
    log(msg);
    m_serverMessage = "HTTP/1.1 404 Not Found\r\n"
                      "Content-Type: text/plain\r\n"
                      "\r\n"
                      "404 Not Found";
    return;
  }
  std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  file.close();
  std::stringstream ss;
  ss << "HTTP/1.1 200 OK\r\n"
    "Content-Type: " << mime << "\r\n"
    << "Content-Length: " << file_content.size()
    << "\r\n\r\n";
  ss << file_content;
  m_serverMessage = ss.str();
}

void TcpServer::sendResponse(int new_socket)
{
  int64_t bytesSent;
  #ifdef LINUX
  bytesSent = write(new_socket, m_serverMessage.c_str(), m_serverMessage.size());
  #elif WIN
  bytesSent = send(m_clientSocket, m_serverMessage.c_str(), m_serverMessage.size(), 0);
  #endif
  if (bytesSent == m_serverMessage.size())
    log("--- Server response sent to client ---\n*********************\n");
  else 
    log("Error sending response to client, some bytes has been lost...");
}

//NOT TOUCH
//it is actually not used at all for now (used socket options for using unused)
void TcpServer::signalHandler(int signum)
{
  std::string prompt = "***** Process termination, SIG : " + std::to_string(signum) + " sent, closing server.";
  //remove json file as it is no longer needed after closing server
  #ifdef LINUX
  system("rm -f ./.download.json");
  #elif _WIN32 || _WIN64
  system("del ./.download.json");
  #endif
  exitWithError(prompt.c_str());
}

}//namespace http

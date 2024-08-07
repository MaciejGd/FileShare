#include "../inc/http_tcpServer_linux.h"

namespace http
{

void log(const std::string& msg)
{
  std::cout << "[LOG] " << msg << "\n";
}

void exitWithError(const std::string& msg)
{
  std::cout << "[ERROR] " << msg << "\n";
  exit(1);
}

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
  m_socket(), m_new_socket(), m_incomingMessage(), main_file(file_name), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)),
  m_serverMessage()
{
  std::signal(SIGINT, signalHandler);
  m_startServer();
  m_fillSocketAddr();

  //setting socket options (reusing not used open socket)
  int opt = 1;
  if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    exitWithError("Failed to set SO_REUSEADDR.");
  }

  if (bind(m_socket, (sockaddr*)&m_socketAddress, sizeof(m_socketAddress)) < 0)
  {
    exitWithError("Failed to assign socket address to created socket.");
  }
  startListen();
}

http::TcpServer::~TcpServer()
{
  m_closeServer();
}

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
  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socket < 0)
  {
    exitWithError("Opening a server's socket failed... shutting down");
    return 1;
  }
  std::cout << "Properly opened server's socket, the socket file descriptor: " << m_socket << "\n";
  return 0;
}

void TcpServer::m_closeServer()
{
  close(m_socket);
  close(m_new_socket);
  exit(0);
}

void TcpServer::m_fillSocketAddr()
{
  m_socketAddress.sin_family = AF_INET;
  m_socketAddress.sin_port = htons(m_port);
  m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address);
}

void TcpServer::startListen()
{
  if (listen(m_socket, 20) < 0)
  {
    exitWithError("Socket listen failed");
  }
  std::string ss;
  ss = "\n*** Listening on ADDRESS: " + std::string(m_ip_address) + " PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)) + " ***\n"; 
  log(ss.c_str());

  while (true)
  {
    log("Waiting for a new connection...\n");
    acceptConnection(m_new_socket);

    std::thread clientThread(&TcpServer::handleClient, this, m_new_socket);
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

void TcpServer::sendResponse(int new_socket)
{
  int64_t bytesSent;
  bytesSent = write(new_socket, m_serverMessage.c_str(), m_serverMessage.size());
  if (bytesSent == m_serverMessage.size())
    log("--- Server response sent to client ---\n*********************\n");
  else 
    log("Error sending response to client, some bytes has been lost...");
}

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

void TcpServer::handleClient(int new_socket)
{
  char buffer[BUFFER_SIZE] = {0};
  int64_t bytesReceived = read(new_socket, buffer, BUFFER_SIZE);
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

void TcpServer::signalHandler(int signum)
{
  std::string prompt = "***** Process termination, SIG : " + std::to_string(signum) + " sent, closing server.";
  exitWithError(prompt.c_str());
}

}//namespace http

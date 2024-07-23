#include "../inc/http_tcpServer_linux.h"
#include "http_tcpServer_linux.h"

namespace http
{

  void log(const char *msg)
  {
    std::cout << "[LOG] " << msg << "\n";
  }

  void exitWithError(const char *msg)
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
    return std::string();
  }

  http::TcpServer::TcpServer(const char* ip, uint32_t port):m_ip_address(ip), m_port(port), 
    m_socket(), m_new_socket(), m_incomingMessage(), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)),
    m_serverMessage(buildResponse())
  {
    std::signal(SIGINT, signalHandler);
    m_startServer();
    m_fillSocketAddr();

    //setting socket options
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
    std::string addr = inet_ntoa(m_socketAddress.sin_addr);
    ss = "\n*** Listening on ADDRESS: " + addr + " PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)) + " ***\n"; 
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

  std::string TcpServer::buildResponse(const std::string &file_name, std::string &response, size_t response_len)
  {
    //reading mime type of rrequest based on requested file_name
    std::string mime = getMimeType(file_name);
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n)"
      "\r\n" << mime;
    int file_fd = open(file_name.c_str(), O_RDONLY);
  }

  // std::string TcpServer::buildResponse()
  // {
  //   std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p><p><img src=\"images/fox.jpg\"></p></body></html>";
  //   std::ostringstream ss;
  //   ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
  //       << htmlFile;
  //   return ss.str();
  // }

  void TcpServer::handleClient(int new_socket)
  {
    char buffer[BUFFER_SIZE] = {0};
    int64_t bytesReceived = read(new_socket, buffer, BUFFER_SIZE);
    if (bytesReceived < 0)
    {
      exitWithError("Failed to read bytes from client socket connection");
    }
    //creating regex for checking mime types requested
    std::regex regex("GET /([^ ]*) HTTP/1");
    std::smatch matches;
    std::string buff(buffer);
    if (std::regex_search(buff, matches, regex))
    {
      std::string url = matches[1].str();
      std::cout << "Found URL is: " << url << std::endl;
      getMimeType(url);
    }

    log(buffer);
    //log("***Received Request from client***");
    sendResponse(new_socket);
    close(new_socket);
  }

  void TcpServer::signalHandler(int signum)
  {
    std::string prompt = "***** Process termination, SIG : " + std::to_string(signum) + " sent, closing server.";
    exitWithError(prompt.c_str());
  }
}

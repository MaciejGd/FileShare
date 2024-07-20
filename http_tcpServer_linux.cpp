#include "http_tcpServer_linux.h"

void log(const char *msg)
{
  std::cout << "[LOG] " << msg << "\n";
}

void exitWithError(const char *msg)
{
  std::cout << "[ERROR] " << msg << "\n";
  exit(1);
}


namespace http
{
  http::TcpServer::TcpServer(const char* ip, uint32_t port):m_ip_address(ip), m_port(port), 
    m_socket(), m_new_socket(), m_incomingMessage(), m_socketAddress(), m_socketAddress_len(sizeof(m_socketAddress)),
    m_serverMessage(buildResponse())
  {
    std::signal(SIGINT, signalHandler);
    startServer();
    fillSocketAddr();

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
    closeServer();
  }

  void TcpServer::startListen()
  {
    if (listen(m_socket, 20) < 0)
    {
      exitWithError("Socket listen failed");
    }
    std::string ss;
    std::string addr = inet_ntoa(m_socketAddress.sin_addr);
    ss = "\n*** Listening on ADDRESS: " + addr + " PORT: " + std::to_string(ntohs(m_socketAddress.sin_port)) + " ***\n\n"; 
    log(ss.c_str());

    int64_t bytesReceived;

    while (true)
    {
      log("Waiting for a new connection...\n\n");
      acceptConnection(m_new_socket);

      char buffer[BUFFER_SIZE] = {0};
      bytesReceived = read(m_new_socket, buffer, BUFFER_SIZE);
      if (bytesReceived < 0)
      {
        exitWithError("Failed to read bytes from client socket connection");
      }
      log("***Received Request from client***");
      sendResponse();
      close(m_new_socket);
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

  void TcpServer::sendResponse()
  {
    int64_t bytesSent;
    bytesSent = write(m_new_socket, m_serverMessage, sizeof(m_serverMessage));
    if (bytesSent == sizeof(m_serverMessage))
      log("--- Server response sent to client ---\n");
    else 
      log("Error sending response to client, some bytes has been lost...");
  }

  const char* TcpServer::buildResponse() {
    std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << htmlFile.size() << "\r\n\r\n"
        << htmlFile;
    return ss.str().c_str();
  }

  void TcpServer::signalHandler(int signum)
  {
    std::string prompt = "***** Process termination, SIG : " + std::to_string(signum) + " sent, closing server.";
    exitWithError(prompt.c_str());
  }

  uint8_t TcpServer::startServer()
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

  void TcpServer::closeServer()
  {
    close(m_socket);
    close(m_new_socket);
    exit(0);
  }
  void TcpServer::fillSocketAddr()
  {
    m_socketAddress.sin_family = AF_INET;
    m_socketAddress.sin_port = htons(m_port);
    m_socketAddress.sin_addr.s_addr = inet_addr(m_ip_address);
    m_socketAddress_len = sizeof(m_socketAddress);
  }
}

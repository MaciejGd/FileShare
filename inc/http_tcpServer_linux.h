#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <stdlib.h>
#include <stdint.h>
#include <thread>
#include <csignal>
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <unistd.h>
#include <filesystem>
#include <stdio.h>
//testing
#include "mime_types.h"
#include <fstream>



#ifdef __linux__
#define LINUX
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#elif _WIN32
#define WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#define BUFFER_SIZE 120000
#define DEFAULT_PORT "8080"

namespace fs = std::filesystem;

namespace http {

class TcpServer
{
  const char* m_ip_address;
  uint32_t m_port;
  int64_t m_incomingMessage;
  std::string m_serverMessage;
  std::string main_file;
  std::string proj_dir;
  #ifdef LINUX
  int32_t m_socket;
  int32_t m_new_socket;
  struct sockaddr_in m_socketAddress;
  uint64_t m_socketAddress_len;
  #elif defined(WIN)
  WSADATA wsaData;
  SOCKET m_listenSocket = INVALID_SOCKET;
  SOCKET m_clientSocket = INVALID_SOCKET;
  struct addrinfo *m_result = nullptr;
  struct addrinfo m_hints;
  #endif
  //static instance of a class needed to handle signalling
  static TcpServer* instance;

  std::vector<std::string> zipped_dirs;

  //helper functions
  void m_log(const std::string& msg);
  void m_exitWithError(const std::string& msg);
  std::string m_getFileExtension(const std::string& filename);
  std::string m_getMimeType(const std::string& extension);
  void m_evaluateProjDir();
  //
  uint8_t m_startServer();
  void m_closeServer();
  void m_fillSocketAddr();
  void m_bind();
  void m_startListen();
  void m_acceptConnection(int &new_socket);
  void m_sendResponse(int new_socket); 
  void m_buildResponse();
  void m_buildResponse(const std::string& file_name);
  //handlers
  void m_handleClient();
  void m_handleDirDownload(const std::string& url);
  static void m_signalHandler(int signum);
public:
  TcpServer(const char* ip, uint32_t port, std::string main_file = "");
  ~TcpServer();

};
} //namespace http
#endif // INCLUDED_HTTP_TCPSERVER_LINUX


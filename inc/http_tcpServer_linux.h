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

namespace http {
  void log(const std::string& msg);
  void exitWithError(const std::string& msg);
  std::string getFileExtension(const std::string& filename);
  std::string getMimeType(const std::string& extension);

  class TcpServer
  {
    const char* m_ip_address;
    uint32_t m_port;
    int64_t m_incomingMessage;
    #ifdef LINUX
    int32_t m_socket;
    int32_t m_new_socket;
    struct sockaddr_in m_socketAddress;
    #elif WIN
    SOCKET m_listenSocket = INVALID_SOCKET;
    SOCKET m_clientSocket = INVALID_SOCKET;
    struct addrinfo *m_result = nullptr;
    struct addrinfo m_hints;
    #endif
    uint64_t m_socketAddress_len;
    std::string m_serverMessage;
    std::string main_file;
    std::string proj_dir;

    void m_evaluateProjDir();
    uint8_t m_startServer();
    void m_closeServer(int error_code);
    void m_fillSocketAddr();
    void m_bind();
  public:
    TcpServer(const char* ip, uint32_t port, std::string main_file = "./test_folder/main.html");
    ~TcpServer();
    void startListen();
    void acceptConnection(int &new_socket);
    void sendResponse(int new_socket); 
    void buildResponse(const std::string& file_name);
    //handlers
    void handleClient();
    static void signalHandler(int signum);
  };
} //namespace http
#endif // INCLUDED_HTTP_TCPSERVER_LINUX


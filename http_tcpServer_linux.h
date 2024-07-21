#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <thread>
#include <csignal>
#include <iostream>
#include <string>
#include <sstream>

#define BUFFER_SIZE 120000

void log(const char* msg);
void exitWithError(const char* msg);


namespace http {
  class TcpServer
  {
    const char* m_ip_address;
    uint32_t m_port;
    int32_t m_socket;
    int32_t m_new_socket;
    int64_t m_incomingMessage;
    struct sockaddr_in m_socketAddress;
    uint64_t m_socketAddress_len;
    std::string m_serverMessage;

    uint8_t startServer();
    void closeServer();
    void fillSocketAddr();
  public:
    TcpServer(const char* ip, uint32_t port);
    ~TcpServer();
    void startListen();
    void acceptConnection(int &new_socket);
    //response
    void sendResponse(int new_socket); 
    std::string buildResponse();
    //handlers
    void handleClient(int new_socket);
    static void signalHandler(int signum);
  };
} //namespace http
#endif


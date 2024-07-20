//main code for server
#include "http_tcpServer_linux.h"

//socket system api
// int socket(int domain, int type, int protocol);
//domain AF_INET domain stands for TCP/IP protocol
//type specifies the type of communication structure the socket will allow SOCK_STREAM for reliable full-duplex stream
//protocol which will be responsible for supporting domain in case of AF_INET equal to 0

int main(void)
{
  //using namespace http;

  http::TcpServer server = http::TcpServer("0.0.0.0", 8080);
  return 0;
}
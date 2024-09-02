// First I need system calls socket, bind, listen, accept, send and recive
// Libraries from man page
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for read, write, close
#include <sys/types.h>  // for socket, bind, listen
#include <sys/socket.h> // for socket, bind, listen
#include <netinet/in.h> // Defines constants and structures for internet domain addresses, such as sockaddr_in.
#include <iostream>
#include <arpa/inet.h>
#define RETURN_VALUE_ERROR_1 (-1)
#define RETURN_VALUE_ERROR_2 (-2)
#define RETURN_VALUE_ERROR_3 (-3)
#define RETURN_VALUE_ERROR_4 (-4)
#define RETURN_VALUE_ERROR_5 (-5)
#define RETURN_VALUE_ERROR_6 (-6)

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;
constexpr unsigned int QUEUE_LENGTH = 10;

constexpr unsigned int MSG_REPLY_LENGTH = 18;
int main(int argc, char *argv[])
{
    // first we need to open a socket
    // int socket(int domain, int type, int protocol);
    // AF_INET -> for IPV4 , SOCK_STREAM -> Connection-Oreinted (TCP)
    // The 'protocol' parameter specifies the protocol to use with the socket.
    // Set to 0 for the default protocol (e.g., TCP for SOCK_STREAM, UDP for SOCK_DGRAM).
    // If multiple protocols exist, specify a specific protocol number.

    int socketFileDis = socket(AF_INET, SOCK_STREAM, 0);
    // we must check for the system call success
    // RETURN VALUE
    //  On success, a file descriptor for the new socket is returned.  On error, -1 is returned, and
    // errno is set appropriately.
    if (socketFileDis < 0)
    {
        std::cerr << "open socket error" << std::endl;
        return RETURN_VALUE_ERROR_1;
    }
    // int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
    // manipulate options for the socket referred to by the file descriptor sockfd.
    int optVal = 1;
    int sockOpt = setsockopt(socketFileDis, SOL_SOCKET, SO_REUSEADDR, (const void *)&optVal, sizeof(int));
    /*level: The level at which the option is defined. SOL_SOCKET indicates that the option is at the socket level.
     optname: The name of the option you want to set. In this case, SO_REUSEADDR is used.
     SO_REUSEADDR is a socket option that allows a socket to forcibly bind
     to a port in use by another socket.
    This is particularly useful in development and server environments where
    you might need to restart a server quickly and want to avoid the "Address already in use" error.
    By setting SO_REUSEADDR to 1 (optval = 1), the socket is allowed to reuse a local address (port) that is in a TIME_WAIT state.
    This does not mean it can bind to a port that's fully active and in use by another socket.
*/
    struct sockaddr_in server_addr, client_addr;
    // making struct of sockaddr_in
    /* struct sockaddr_in {
    sa_family_t    sin_family;   // Address family (AF_INET for IPv4)
    in_port_t      sin_port;     // Port number (16 bits), in network byte order
    struct in_addr sin_addr;     // Internet address (32 bits)
    char           sin_zero[8];  // Padding to match `struct sockaddr` size
};
 */

    bzero((char *)&server_addr, sizeof(server_addr));
    // bzero makes all bytes from starting address to size zeros to avoid any garbage values
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);
    // here we initialized server sock_addr
    /*int bind(int sockfd, const struct sockaddr *addr,
                socklen_t addrlen); */
    if (bind(socketFileDis, (const sockaddr *)&server_addr,
     sizeof(server_addr))< 0)
    {
        std::cerr << "bind failed" << std::endl;
        return RETURN_VALUE_ERROR_2;
    }
    // then after binding the socket to specific address
    // we start to listen to that port
    // int listen(int sockfd, int backlog);
    if (listen(socketFileDis, QUEUE_LENGTH) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return RETURN_VALUE_ERROR_3;
    }
    // then we have a blocking call called accept it will extract the first connection
    // from the connection queue.
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int client_length = sizeof(client_addr);
    int acceptedSocket;
    acceptedSocket = accept(socketFileDis, (struct sockaddr *)&client_addr, (socklen_t *)&client_length);
    if (acceptedSocket < 0)
    {
        std::cerr << "bind failed" <<strerror(errno)<< std::endl;
        return RETURN_VALUE_ERROR_4;
    }
    /*On success, these system calls return a file descriptor 
    for the accepted socket (a  nonnegative  integer).
       On  error, -1 is returned*/
       // printing success message for the user with client specifications
    std::cout << "server: got connection from : "
              << inet_ntoa(client_addr.sin_addr)
              << " and port : " << ntohs(client_addr.sin_port)
              << std::endl;
    
    // after that i can write to the client using write system call
    // ssize_t write(int fd, const void *buf, size_t count);
    if (write (acceptedSocket,"You are connected",MSG_REPLY_LENGTH)<0)
    {
        std::cout<<"Failed to write to the client."<<std::endl;
        return RETURN_VALUE_ERROR_5;
    }
    while (true)
    {
    // then we start to receive from user
    // ssize_t read(int fd, void *buf, size_t count);
    std :: string messageFromClient (MAX_BUFFER,0);
    if (read(acceptedSocket, &messageFromClient[0],MAX_BUFFER-1)<0 )
    {
        std:: cout << "Failed to read from the client."<< std::endl;
        return RETURN_VALUE_ERROR_6;
    }
    std:: cout << "Got the message" <<std::endl<< messageFromClient<<std::endl;
    std::cout<<"Reply back to the client "<< std::endl;
    std:: string msgBackToTheClient (MAX_BUFFER,0);
    getline(std::cin, msgBackToTheClient);
    if (write (acceptedSocket,msgBackToTheClient.c_str(),strlen(msgBackToTheClient.c_str()))<0)
    {
        std::cout<<"Failed to write to the client."<<std::endl;
        return RETURN_VALUE_ERROR_5;
    }

    }
    close(acceptedSocket);
    close(socketFileDis);
    return 0;
}

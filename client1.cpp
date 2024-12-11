// socket() -> connect() -> send() -> receive()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#define RETURN_VALUE_ERROR_1 (-1)
#define RETURN_VALUE_ERROR_2 (-2)
#define RETURN_VALUE_ERROR_3 (-3)
#define RETURN_VALUE_ERROR_4 (-4)
#define RETURN_VALUE_ERROR_5 (-5)

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;
int main(int argc, char *argv[])
{
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
    /*
    struct hostent {
    char  *h_name;        // Official name of the host
    char **h_aliases;     // Null-terminated array of alternate names
    int    h_addrtype;    // Address type (e.g., AF_INET for IPv4)
    int    h_length;      // Length of each address in bytes
    char **h_addr_list;   // Null-terminated array of network addresses (in network byte order)
};
*/
    /*
    The gethostbyname() function returns a structure of type hostent for the  given  host  name.
           Here  name  is  either  a  hostname  or  an  IPv4  address  in standard dot notation (as for
           inet_addr(3)).  If name is an IPv4 address, no lookup is performed and gethostbyname()  sim‐
           ply  copies  name  into  the  h_name  field  and  its  struct  in_addr  equivalent  into the
           h_addr_list[0] field of the returned hostent structure.*/
    struct hostent *server = gethostbyname(argv[1]);
    /*The gethostbyname() and gethostbyaddr() functions return the hostent  structure  or  a  null
       pointer  if  an  error  occurs.*/
    if (server == nullptr)
    {
        std::cerr << "gethostbyname, no such host" << std::endl;
        return RETURN_VALUE_ERROR_2;
    }
    struct sockaddr_in server_addr;
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
    bcopy((char *)server->h_addr,
          (char *)&server_addr.sin_addr.s_addr,
          server->h_length);

    server_addr.sin_port = htons(SERVER_PORT);
    // int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
    /*he  connect()  system call connects the socket referred to by the file descriptor sockfd to
       the address specified by addr.  The addrlen argument specifies the size of addr.
        If the connection or binding succeeds, zero is returned.  On error, -1 is returned, and  er‐
       rno is set appropriately. */
    if (connect(socketFileDis, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "connect error" << std::endl;
        return RETURN_VALUE_ERROR_3;
    }
    // after that we read the ACK messeage from the server
    std::string readBuffer(MAX_BUFFER, 0);
    if (read(socketFileDis, &readBuffer[0], MAX_BUFFER - 1) < 0)
    {
        std::cerr << "read from socket failed" << std::endl;
        return RETURN_VALUE_ERROR_4;
    }
    std::cout << readBuffer << std::endl;
    while (true)
    {
    // after connection is initiated we start to write to the server
    std::string writeBuffer(MAX_BUFFER, 0);
    std::string receivedBuffer(MAX_BUFFER, 0);
    std::cout << "What message for the server? : ";
    getline(std::cin, writeBuffer);
    if (write(socketFileDis, writeBuffer.c_str(), strlen(writeBuffer.c_str())) < 0)
    {
        std::cerr << "write to socket" << std::endl;
        return RETURN_VALUE_ERROR_5;
    }
     if (read(socketFileDis, &receivedBuffer[0], MAX_BUFFER - 1) < 0)
    {
        std::cerr << "read from socket failed" << std::endl;
        return RETURN_VALUE_ERROR_4;
    }
    std::cout << receivedBuffer << std::endl;
    }
    close(socketFileDis);
    return 0;
}
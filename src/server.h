#ifndef _SERVER_H
#define _SERVER_H

// socket and server initialization
void* init_server(void* );
int create_socket(const char* );

// server closure
void close_server(bool );

// server connections
void handle_clients(int* , int* , int* , struct pollfd** );
void add_pollfd(struct pollfd** , int , int* , int* );
void remv_pollfd(struct pollfd* , int , int* );

// same cast function in client 
namespace server
{
    // cast to right sockaddr_in address (IPV4 or IPV6)
    void* cast_in_addr(struct sockaddr* );
}

#endif // _SERVER_H
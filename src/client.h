#ifndef _CLIENT_H
#define _CLIENT_H

// client and socket initialization
void init_client(const char* , const char* );
int connect_to_socket(const char* , const char* );

// server comms handling - send and receive functions
// receive runs on another thread
void handle_server(int* );
void* recv_data(void* );

// same cast function in server
namespace client 
{
    // cast to right sockaddr_in address (IPV4 or IPV6)
    void* cast_in_addr(struct sockaddr* );
}

#endif // _CLIENT_H
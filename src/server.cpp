#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>

#include "server.h"

#define BUFFLENGTH 128

using namespace server;

static bool s_ServerStatus;

void* init_server(void* port)
{
    int sockfd, clients = 0, serversize = 5;
    struct pollfd* pollfds = (pollfd*)(malloc(sizeof(*pollfds) * serversize));
    
    if((sockfd = create_socket((const char*)port)) == -1){
        std::cout << "\x1b[31mserver : server was not created\x1b[0m" << std::endl;
        std::cout << "\x1b[31mserver : use '!close' to try again\x1b[0m" << std::endl;
        close_server(false);
        return NULL;
    }

    pollfds[0].fd = sockfd; 
    pollfds[0].events = POLLIN;
    clients = 1;

    s_ServerStatus = true;

    std::cout << "\x1b[32mserver : awaiting connections...\x1b[0m" << std::endl;
    
    handle_clients(&sockfd, &clients, &serversize, &pollfds);

    return NULL;
}

int create_socket(const char* port)
{    
    int sockfd, addrstatus, reuseport = 1;
    struct addrinfo ref, *res, *faddr;

    memset(&ref, 0, sizeof(ref));
    ref.ai_family = AF_UNSPEC;
    ref.ai_socktype = SOCK_STREAM;
    ref.ai_flags = AI_PASSIVE;

    if((addrstatus = getaddrinfo(NULL, port, &ref, &res)) != 0){
        fprintf(stderr, "\x1b[31mgetaddrinfo : %s\x1b[0m\n", gai_strerror(addrstatus));
        return -1; 
    }    

    for(faddr = res; faddr != NULL; faddr = faddr->ai_next){

        if((sockfd = socket(faddr->ai_family, faddr->ai_socktype, faddr->ai_protocol)) == -1){
            perror("\x1b[31mserver socket \x1b[0m");
            continue;
        }

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseport, sizeof(int)) == -1){
            perror("\x1b[31mserver setsockopt \x1b[0m");
            return -1;
        }

        if(bind(sockfd, faddr->ai_addr, faddr->ai_addrlen) == -1){
            close(sockfd);
            perror("\x1b[31mserver bind \x1b[0m");
            continue;
        }

        break;
    }

    freeaddrinfo(res);
    
    if(faddr == NULL){
        fprintf(stderr, "\x1b[31mserver : failed to bind\x1b[0m\n");
        return -1;
    }

    if((listen(sockfd, 5)) == -1){
        perror("\x1b[31mserver listen \x1b[0m");
        return -1;
    }   

    std::cout << "\x1b[32mserver : started listening on port " << port << "\x1b[0m" << std::endl;

    return sockfd;
}


void close_server(bool status)
{
    s_ServerStatus = status;
}

void handle_clients(int* sockfd, int* clients, int* serversize, struct pollfd** pollfds)
{
    struct sockaddr_storage cliaddr;
    socklen_t socksize;
    int clifd, recvdbytes;
    char recvbuf[BUFFLENGTH] = {0}, ipstr[INET6_ADDRSTRLEN] = {0};

    while(s_ServerStatus){

        if(poll(*pollfds, *clients, 500) == -1){
            perror("\x1b[31mserver poll\x1b[30m");
            s_ServerStatus = false;
            break;
        }  

        for(int i = 0; i < *clients; i++){
        
            if((*pollfds)[i].revents & POLLIN){
                
                if((*pollfds)[i].fd == *sockfd){
                    socksize = sizeof(cliaddr);
                    
                    if((clifd = accept(*sockfd, (struct sockaddr*)&cliaddr, &socksize)) == -1){
                        perror("\x1b[31msocket accept \x1b[0m");
                        continue;
                    } else {
                        add_pollfd(pollfds, clifd, clients, serversize);
                        inet_ntop(cliaddr.ss_family, server::cast_in_addr((struct sockaddr*)&cliaddr),
                        ipstr, INET6_ADDRSTRLEN);
                        std::cout << "\x1b[32mserver : incoming connection from " << ipstr << std::endl;
                        std::cout << "server : " << (*clients)-1 << " client connection(s)\x1b[0m" << std::endl;
                    }

                } else {
                    recvdbytes = recv((*pollfds)[i].fd, recvbuf, BUFFLENGTH - 1, 0);
                    
                    if(recvdbytes <= 0){
                        
                        if (recvdbytes == 0) {
                            printf("\x1b[32mserver : client %d disconnected\x1b[0m\n", clifd);
                        } else {
                            perror("\x1b[31mserver recv \x1b[0m");
                        }

                        close((*pollfds)[i].fd);
                        remv_pollfd(*pollfds, i, clients);
                        std::cout << "\x1b[31mserver : " << (*clients)-1 << " client connection(s)\x1b[0m" << std::endl;

                    } else {
                        recvbuf[BUFFLENGTH] = '\0';

                        for(int j = 0; j < *clients; j++){
                            if((*pollfds)[j].fd != *sockfd && (*pollfds)[j].fd != (*pollfds)[i].fd){
                                if(send((*pollfds)[j].fd, recvbuf, recvdbytes, 0) == -1){
                                    perror("\x1b[31msend\x1b[0m");
                                }
                            }
                        }
                    }

                }
            }
       
        }
    }

    std::string connclosed = "\n\x1b[31mserver : connection to server has been closed\x1b[0m\n";
    int n = connclosed.length() + 1;
    char closebuf[n] = {0};
    strcpy(closebuf, connclosed.c_str());

    for(int i = 0; i < *clients; i++){

        if((*pollfds)[i].fd != *sockfd){
            if(send((*pollfds)[i].fd, closebuf, n, 0) == -1){
                perror("\x1b[31msend\x1b[0m");
            }

            close((*pollfds)[i].fd);
            remv_pollfd((*pollfds), i, clients);
        }

    }

    close(*sockfd);
}

void add_pollfd(struct pollfd** pollfds, int clifd, int* clients, int* servsize)
{
    if(*clients == *servsize){
        *servsize *= 2;
        *pollfds = (pollfd*)realloc(*pollfds, sizeof(**pollfds) * (*servsize));
    }

    (*pollfds)[*clients].fd = clifd;
    (*pollfds)[*clients].events = POLLIN;
    (*clients)++;
}

void remv_pollfd(struct pollfd* pollfds, int i, int* clients)
{
    pollfds[i] = pollfds[*clients - 1];
    (*clients)--;
}

void* server::cast_in_addr(struct sockaddr* addr)
{
    if(addr->sa_family == AF_INET){
        return &(((struct sockaddr_in*)addr)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*)addr)->sin6_addr);
    }
}
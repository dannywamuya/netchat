#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <string>
#include <pthread.h>

#include "client.h"
#include "menu.h"

#define BUFFLENGTH 128
#define USERNAMELEN 8

static bool s_ClientStatus;

void init_client(const char *ip, const char *port)
{
    int sockfd;

    if ((sockfd = connect_to_socket(ip, port)) == -1)
    {
        std::cout << "\x1b[31mclient : did not connect to server\x1b[0m" << std::endl;
        leave_server();
        return;
    }

    s_ClientStatus = true;

    std::cout << "\x1b[32mclient : ready to communicate with server\x1b[0m\n"
              << std::endl;

    handle_server(&sockfd);
}

int connect_to_socket(const char *ip, const char *port)
{
    int sockfd;
    struct addrinfo ref, *res, *faddr;
    char ipstr[INET6_ADDRSTRLEN];
    int addrstatus;

    memset(&ref, 0, sizeof(ref));
    ref.ai_family = AF_UNSPEC;
    ref.ai_socktype = SOCK_STREAM;

    if ((addrstatus = getaddrinfo(ip, port, &ref, &res)) != 0)
    {
        fprintf(stderr, "\x1b[31mgetaddrinfo : %s\x1b[0m\n", gai_strerror(addrstatus));
        return -1;
    }

    for (faddr = res; faddr != NULL; faddr = faddr->ai_next)
    {

        if ((sockfd = socket(faddr->ai_family, faddr->ai_socktype, faddr->ai_protocol)) == -1)
        {
            perror("\x1b[31mclient socket \x1b[0m");
            continue;
        }

        if (connect(sockfd, faddr->ai_addr, faddr->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("\x1b[31mclient connect \x1b[0m");
            continue;
        }

        inet_ntop(faddr->ai_family, client::cast_in_addr((struct sockaddr *)faddr->ai_addr), ipstr, INET6_ADDRSTRLEN);
        printf("\x1b[32mconnected to %s\x1b[0m\n", ipstr);

        break;
    }

    if (faddr == NULL)
    {
        return -1;
    }

    freeaddrinfo(res);

    return sockfd;
}

void handle_server(int *sockfd)
{
    int sentbytes;
    char buf[BUFFLENGTH] = {0};
    char leave[] = "!leave";

    pthread_t th;
    pthread_create(&th, NULL, &recv_data, (void *)sockfd);

    std::cout << "\x1b[33m";
    while (s_ClientStatus)
    {
        memset(&buf, 0, BUFFLENGTH);
        fgets(buf, BUFFLENGTH, stdin);

        if (strncmp(leave, buf, 6) == 0)
        {
            s_ClientStatus = false;
            close(*sockfd);
            pthread_cancel(th);
        }
        else
        {
            if ((sentbytes = send(*sockfd, buf, BUFFLENGTH, 0)) == -1)
            {
                perror("\x1b[31mclient send \x1b[0m");
            }
        }
    }

    pthread_join(th, NULL);
}

void *recv_data(void *sockfd)
{
    int recvdbytes;
    char recvbuf[BUFFLENGTH];

    while (s_ClientStatus)
    {
        if ((recvdbytes = recv((*(int *)sockfd), recvbuf, BUFFLENGTH - 1, 0)) <= 0)
        {
            if (recvdbytes == 0)
            {
                printf("\n\x1b[31mclient : server %d disconnected\x1b[0m\n", (*(int *)sockfd));
                std::cout << "\x1b[34m\npress any key to leave...\x1b[0m" << std::endl;
                ;
                s_ClientStatus = false;
                return NULL;
            }
            else if (recvdbytes == -1)
            {
                perror("\x1b[31mclient recv \x1b[0m");
            }
        }
        else
        {
            recvbuf[recvdbytes] = '\0';
            printf("%s", recvbuf);
        }
    }
    return NULL;
}

void *client::cast_in_addr(struct sockaddr *addr)
{
    if (addr->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)addr)->sin_addr);
    }
    else
    {
        return &(((struct sockaddr_in6 *)addr)->sin6_addr);
    }
}
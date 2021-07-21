#include <iostream>
#include <map>
#include <string>
#include <pthread.h>

#include "menu.h"
#include "server.h"
#include "client.h"

void create_server(){  
    std::string port, close;

    std::cout << "\x1b[34mplease enter port number to use (eg. 1111)\x1b[0m" << std::endl;
    std::cout << ">> ";
    std::cin >> port;

    std::cout << "\x1b[32m\nattempting to initialize server on port " << port << "\x1b[0m" << std::endl;

    pthread_t th;
    pthread_create(&th, NULL, &init_server, (void*)port.c_str());
    
    std::cin >> close;

    if(close == "!close"){
        close_server(false);
        pthread_join(th, NULL);
    }

    std::cout << "\x1b[31mserver : server connections closed\x1b[0m\n" << std::endl;
    menu(); 
}

void close_server(){
    std::cout << "\x1b[31mno server instance running on this process\x1b[0m" << std::endl;
    menu();
}

void join_server(){
    std::string ip, port, leave;

    std::cout << "\x1b[34mplease enter ip address to use (eg. 127.0.0.1)\x1b[0m" << std::endl;
    std::cout << ">> ";
    std::cin >> ip;
    std::cout << "\x1b[34mplease enter port number to use (eg. 1111)\x1b[0m" << std::endl;
    std::cout << ">> ";
    std::cin >> port;

    std::cout << "\n\x1b[32mattempting to join server " << ip << ":" << port << "\x1b[0m" << std::endl;

    init_client(ip.c_str(), port.c_str());

    std::cout << "\x1b[31mclient : connection to server closed\x1b[0m\n" << std::endl;
    menu();
}

void leave_server(){
    std::cout << "\x1b[31myour not connected to any server on this process\x1b[0m" << std::endl;
    menu();
}

void unrecognised_input(){
    std::cout << "\x1b[31munrecognised input, please try again or enter '!help' to print help\x1b[0m" << std::endl;
    menu(); 
}

void print_help()
{
    using std::cout;
    using std::cin;
    using std::endl;
    
    cout << "" << endl;
    cout << "  \x1b[34mhelp\n"         << endl;
    cout << "  !start           - to start server      " << endl;
    cout << "  !close           - to close server      " << endl;
    cout << "  !join            - to join server       " << endl;
    cout << "  !leave           - to leave server      " << endl;
    cout << "  !help            - to print help \033[0m" << endl;

    cout << "\n" << endl;
    menu();
}

void menu()
{
    std::string opt;
    std::map<std::string, int> options;
    
    options.insert(std::pair<std::string, int>("!start", 1));
    options.insert(std::pair<std::string, int>("!close" , 2));
    options.insert(std::pair<std::string, int>("!join"  , 3));
    options.insert(std::pair<std::string, int>("!leave" , 4));
    options.insert(std::pair<std::string, int>("!help"  , 5));

    std::cout << ">> ";
    std::cin >> opt;

    switch(options[opt]){
        case 1:
            create_server();
            break;
        case 2:
            close_server();
            break;
        case 3:
            join_server();
            break;
        case 4:
            leave_server();
            break;
        case 5:
            print_help();
            break;
        default:
            unrecognised_input();
            break;
    }
}
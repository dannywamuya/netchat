/*
    +-------------------------------------------------+
    |                                                 |
    |                  netchat v1.0                   | 
    |    Based on Beej's Guide to Network Programming |
    |                                                 |
    +-------------------------------------------------+
    
    author  ~           Danny Wamuya
    date    ~           21-07-2021
    license ~           MIT License
*/

#include <iostream>

#include "menu.h"

int main()
{
    system ("clear");
    std::cout << "\t\t\t\x1b[35mnetchat v1.0\n\033[0m" << std::endl;
    print_help();
    menu();
    return 0;
}
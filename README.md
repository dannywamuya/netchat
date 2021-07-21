# *netchat v1.0*

v1.0 of netchat, a client/server chat application using the tcp protocol to make sure that your messages do actually get there and in the actual way that you sent them (*lol, i just learnt about this, can you already tell?*).

<img src="res/netchat-v10.gif" height="512">

&ensp;  
### ***Server***

Supports real-time, multiple client-server communication. Only thing is you have to do is host the server yourself (*well, only if you are using unix, sorry windows users*). 

The server runs on a thread of it's own but you can't run a client and server at the same time yet. The most feasible way would be to run two instances of netchat. The server thread uses the poll system call to listen for data allowing it to listen for new connections and to receive data at the same time.

It's probalbly the right time to also reference **Beej's Guide to Network Programming** where almost all the fundamental concepts of this application are rooted.  

To run the server, use the **!start** command in the application and follow the steps.

```
...
>> !start
please enter port number to use (eg. 1111)
>> 1111

attempting to initialize server on port 1111 
...
```  
To close the server, use the **!close** command while the server is running.
```
...
server : 1 client connection(s)
!close
server : server connections closed 
...
```  
&ensp;  
### ***Client***  

Simple but effecient, the client runs on two threads (*main and a receiver thread*) basically allowing the receive and send functions to run concurrently.  

In addition to the socket's api, this application uses the pthread api for all the multithreading functions (*yet another **L** for windows users*).

To join a server, use the **!join** command in the application and follow the steps.

```
>> !join
please enter ip address to use (eg. 127.0.0.1)
>> localhost
please enter port number to use (eg. 1111)
>> 1111

attempting to join server localhost:1111
```

To leave the server, use the **!leave** command in the application while the client is running.
```
...
!leave
client : connection to server closed
...
```
&ensp;  
### ***Compilation***

I've only used the gnu compiler so that's what I would recommend. Feel free to use other compilers if you wish, I am unaware of what might go wrong (*apart from the fact that it won't run on windows at all*). What do they call it? Unconscious Incompetence... Yeah. 😂  

To compile :
```
g++ -pthread netchat.cpp menu.cpp server.cpp client.cpp -o netchat
```
To run :
```
./netchat
```

For windows users... try using <a href="https://docs.microsoft.com/en-us/windows/wsl/install-win10">WSL</a>. It's easy to set up and run. More if you have the time, install linux and if you have the money just buy a macbook.

Windows uses the Winsock api for sockets and if interested you could implement this on windows, I might later on. 

&ensp;
### ***Bugs***

If you find a bug, It's probably a feature, don't worry. If you would like to contribute, feel free to create and issue or even better make a PR.  

&ensp;
### ***Future***
  
- Authentication, I think  
- Data Encapsulation (Basically allowing the client to send more information about the request in a header, eg. username, authentication, bufferlength)   
- Discord like features, lol
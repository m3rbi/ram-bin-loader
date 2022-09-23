# Ram Binary Loader
_**DISCLAIMER**  
This Project is for reasearch purposes only!_

___
## Intro  
This tool is a server that is able to receive a binary file over TCP, and run it in a detached process, from memory.  
The process name will be as configured in `src\consts.h`.

___
## Compile  
Run inside the repository:
```
make
```

___
## Run
In order to run the server locally,
to listen on specified ip and port, run:
```
./proc_server {ip} {port}
```

___
## Client
The client requires the `--ip` and `--port` arguments in order to connect to the remote server.  

In order to run the client, run:
```
./client.py --ip {ip} --port {port} {action} [..]
```
The available actions are:  
* exec
    * Requires another param to specify path to local binary to send.
* close 

___
## Protocol
The protocol is implemented in the `message_t` struct, declared inside `src/message.h`.  
The protocol is using little-endian encoding, and the following is the structure of the protocol:  

* message_type - int
* content_length - size_t
* buffer - bytes in the size specified earlier

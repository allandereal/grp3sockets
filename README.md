## Distributed Systems Development Group 3
Assignment to Implement Client and Server Socket RPC using UDP protocol in C

### How to run the program
#### Compile.
_There are 2 programs:- Client program and Server program._

To compile the client program run the following linux command
`gcc server.c -o server`

To compile the client program run the following linux command
`gcc client.c -o client`

#### Run.
`./server` to start the server,

_To enable the client to connect to the server, you need the server's hostname. run `hostname` or `hostnamectl` on your server to obtain this.
`./client hostname` to start the client,

#### Testing Timeout.
- Uncomment the `sleep()` function in the `server.c` file, recompile and run the code. or
- Run the client program when the server is not running.
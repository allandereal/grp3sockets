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
`./client hostname` to start the client,

#### Testing Timeout.
- Uncomment the `sleep()` function in the `server.c` file, recompile and run the code.
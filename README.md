# TCPFileTransfer
Implement .txt File Transfer Over TCP

Running instructions

Using cygwin console for compile and execute

Compile: 

	make all

Execute:

	Server:
	
		Useage:		./TCPFileServer.exe <port num> 
		
		Example:	./TCPFileServer.exe 82
		
	Client:
	
		Useage:		./TCPFileClient.exe <server ip address> <server port num> 
	
		Example:	./TCPFileClient.exe 127.0.0.1 82 82

1. The server starts and waits for incoming connection request.
2. The client establishes a TCP connection with the server.
3. The client sends a filename to the server over the TCP connection.
4. The server receives the filename.
5. If the file is not present, the server informs the client by sending back empty file header.
6. If the file is present, the server continuously reads the file into a buffer and sends the buffer content to the
	 client until file-end is reached.
7. Once the transmission finishes, the client verifies that the received file is correct. File verficiation includeds the number of lines
	 and number of charaters in .txt file
8. The client chooses to download a different file or close the TCP connection and exit.





#include <stdio.h> /*for printf() and fprintf()*/
#include <sys/socket.h> /*for socket(), connect(), send(), and recv()*/
#include <arpa/inet.h> /*for sockaddr_in and inet_addr()*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 1024 /*Size of receive buffer*/

void DieWithError(char *errorMessage);

int main(int argc, char *argv[]){
	int sock; /*Socket descriptor*/
	struct sockaddr_in servAddr;
	unsigned short servPort;
	char *servIP;
	char filename[100];
	int packet_count;
	int totalChar;
	char buffer[RCVBUFSIZE];
	FILE * fp;

	struct FileHeader {
		int lines;
		int total_char;
	};
	struct PacketHeader {
		int char_count;
		int seq_num;
	};

	if (argc != 3){
		fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n"), argv[0];
		exit(1);
	}

	servIP = argv[1];
	servPort = atoi(argv[2]);
	/*Create a socket using TCP*/
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))<0)
		DieWithError("socket() failed");

	/*Construct the server address structure*/
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(servIP);
	servAddr.sin_port = htons(servPort);

	/*Establish connection to echo server*/
	if (connect(sock, (struct sockaddr*) &servAddr, sizeof (servAddr))<0)
		DieWithError("connect( ) failed");

	printf("Connect Succeed\n");

	for (;;){

		cleanBuffer(filename, 100);
		cleanBuffer(buffer, RCVBUFSIZE);

		// File Detail Info
		struct FileHeader header;
		header.lines = 0;
		header.total_char = 0;

		printf("Please Enter the Filename \n");
		scanf("%s", filename);

		if (send(sock, filename, sizeof(filename), 0) != sizeof(filename)){
			DieWithError("Receive Filename Fails");
		}

		if (strcmp (filename, "EXIT") == 0){
			break;
		}

		// Receive the Header From Server
		if (recv(sock, &header, sizeof(header), 0) != sizeof(header)){
			DieWithError("Receive File Header Fails");
		}

		if (header.lines == 0 || header.total_char == 0){
			printf("File Not Exist\n");
			continue;
		}

		printf("File has %d lines \n", header.lines);
		printf("File has %d characters \n", header.total_char);

		struct PacketHeader packet;
		packet.char_count = 0;
		packet.seq_num = 0;

		packet_count = 0;
		totalChar = 0;

		fp = fopen(strcat(filename, "_recv.txt") , "w");
		while (packet_count < header.lines){
			if (recv(sock, &packet, sizeof(packet), 0) != sizeof(packet)){
				DieWithError("Header Transmission Fails");
			}

			packet_count++;
			totalChar += packet.char_count;

			if (packet.seq_num != packet_count){
				DieWithError("Header Transmission Fails");
			}
			if (recv(sock, buffer, packet.char_count + 1, 0) != packet.char_count + 1){
				DieWithError("Packet Transmission Fails");
			}
			fputs (buffer, fp);
			cleanBuffer(buffer, packet.char_count + 1);
		}

		if (totalChar != header.total_char){
			DieWithError("Packet Transmission Fails");
		}

		fclose(fp);
	}

	close(sock);
	exit(0);
}

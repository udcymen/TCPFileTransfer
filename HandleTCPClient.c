#include <stdio.h> /* for printf() and fprintf() */
#include <stdio.h> /* for printf() and fprintf() */
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 1024 /* Size of receive buffer */

void DieWithError(char *errorMessage);

void cleanBuffer(char *buffer, int size);

void HandleTCPClient(int clntSocket)
{
	char buffer[RCVBUFSIZE];     /* Buffer for echo string */
	char filename[100];
	FILE * fp;
	char *line = NULL;
	size_t len = 0;

	struct FileHeader {
		int lines;
		int total_char;
	};

	struct PacketHeader {
		int char_count;
		int seq_num;
	};

	printf("Enter HandleTCPClient, clntSocket: %d\n", clntSocket);

	for (;;){

		cleanBuffer(filename, 100);
		cleanBuffer(buffer, RCVBUFSIZE);
		free(line);

		// File Detail Info
		struct FileHeader header;
		header.lines = 0;
		header.total_char = 0;

		// Receive Filename
		if (recv(clntSocket, filename, sizeof(filename), 0) != sizeof(filename)){
			DieWithError("Receive Filename Fails");
		}

		if (strcmp (filename, "EXIT") == 0){
			printf("Exit HandleTCPClient, clntSocket: %d\n", clntSocket);
			break;
		}

		printf("Receive File Request for %s \n", filename);

		fp = fopen(filename, "r");

		// If File Does Not Exist, Close Socket.
		if (fp == NULL){
			// Send the Empty Header To Client. Tell Cilent File Does Not Exist
			if (send(clntSocket, &header, sizeof(header), 0) != sizeof(header)){
				DieWithError("Send File Header Fails");
			}
			continue;
		}

		// Get Info From File For Later Validation
		while (getline(&line, &len, fp) > 0) {
			header.lines++;
			header.total_char += strlen(line);
		}

		fclose(fp);
		len = 0;
		line = NULL;

		// Send the Header From Server
		if (send(clntSocket, &header, sizeof(header), 0) != sizeof(header)){
			DieWithError("Send File Header Fails");
		}

		printf("%d \n", header.lines);
		printf("%d \n", header.total_char);

		struct PacketHeader packet;
		packet.char_count = 0;
		packet.seq_num = 0;

		fp = fopen(filename, "r");


		// Send File Content
		while (getline(&line, &len, fp) > 0) {
			int line_lenght;
			packet.char_count = strlen(line);
			packet.seq_num++;
			if (send(clntSocket, &packet, sizeof(packet), 0) != sizeof(packet)){
				DieWithError("Header Transmission Fails");
			}

			line_lenght = strlen(line);
			for (int i=0; i<line_lenght; i++){
				buffer[i] = line[i];
			}
			buffer[line_lenght] = '\0';
			if (send(clntSocket, buffer, line_lenght + 1, 0) != line_lenght + 1){
				DieWithError("Packet Transmission Fails");
			}
			cleanBuffer(buffer, line_lenght + 1);
		}

		fclose(fp);
		len = 0;
		line = NULL;
	}

	close(clntSocket); /* Close client socket */
}



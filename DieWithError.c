

#include <stdio.h> /* for perror() */
#include <stdlib.h> /* for exit() */

void DieWithError(char *errorMessage)
{
	
	perror(errorMessage);
	exit(1);
}

void cleanBuffer(char *buffer, int size){
	for (int i = 0; i < size; i++){
		buffer[i] = '\0';
	}
}

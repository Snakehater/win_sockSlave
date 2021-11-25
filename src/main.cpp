/*
	Simple udp client
*/
#include <stdio.h>
#include <winsock2.h>

#include "host_finder.hpp"

//#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512	//Max length of buffer
#define PORT 6969	//The port on which to listen for incoming data

std::string hostIP = "";

HostFinder hostFinder;

int main(void)
{
no_host:
	hostIP = "";
	while (hostIP.length() < 11) { // minimum is xxx.xxx.x.x which is 11 chars
		hostIP = hostFinder.find(PORT, 10, 0, false);
	}
	
	struct sockaddr_in si_other;
	int s, slen=sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");
	
	//create socket
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d" , WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	
	int ReceiveTimeout = 50; 
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&ReceiveTimeout, sizeof(int));
	
	//setup address structure
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(hostIP.c_str());
	
	//start communication
	while(1)
	{
		printf("Enter message : ");
		gets(message);
		
		//send the message
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d" , WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		
		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d" , WSAGetLastError());
			
			closesocket(s);
			WSACleanup();
			
			goto no_host;

			//exit(EXIT_FAILURE);
		}
		
		puts(buf);
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

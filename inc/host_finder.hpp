/*
	Simple udp client
*/
#include <stdio.h>
//#include <iostream>
#include <winsock2.h>
#include <string>

//#pragma comment(lib,"ws2_32.lib") //Winsock Library

std::string SERVER_PREFIX = "192.168";	// start of ip address of udp server
#define PING_CMD "PING"

class HostFinder {
	public:
		HostFinder(){}; // null constructor
		// port - timeout of each connection - start prefix of xxx.xxx.this.xxx - start prefix of xxx.xxx.xxx.this - should we cycle first prefix
		std::string find(int port, int timeout, int startPref1, int startPref2, bool cycleFirstPref) {
			std::string hit_addr = "";
			for (uint16_t i = startPref1; i <= (cycleFirstPref ? 254 : startPref1); i++) {
				for (uint16_t j = startPref2; j <= 254; j++) {
					std::string addr = SERVER_PREFIX + '.' + std::to_string(i) + '.' + std::to_string(j);
//					std::cout << "testing " << addr << std::endl;
					if (this->ping(addr, port)) {
						hit_addr = addr;
						goto hit;
					}
				}
			}
		hit:
			return hit_addr;
		}
	private:
		bool ping(std::string server_addr, int port, int receiveTimeout = 50) {
			struct sockaddr_in si_other;
			int s, slen=sizeof(si_other);
			char buf[BUFLEN];
			char message[] = PING_CMD;
			WSADATA wsa;

			//Initialise winsock
			//printf("\nInitialising Winsock...");
			if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
			{
				printf("Failed. Error Code : %d",WSAGetLastError());
//				exit(EXIT_FAILURE);
			}
			//printf("Initialised.\n");
			
			//create socket
			if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
			{
				printf("socket() failed with error code : %d" , WSAGetLastError());
				return false;
				//exit(EXIT_FAILURE);
			}
			
			setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&receiveTimeout, sizeof(int));
			
			//setup address structure
			memset((char *) &si_other, 0, sizeof(si_other));
			si_other.sin_family = AF_INET;
			si_other.sin_port = htons(port);
			si_other.sin_addr.S_un.S_addr = inet_addr(server_addr.c_str());
			
			//start communication
			
			//send the message
			if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
			{
				//printf("sendto() failed with error code : %d" , WSAGetLastError());
				return false;
				//exit(EXIT_FAILURE);
			}
			
			//receive a reply and print it
			//clear the buffer by filling null, it might have previously received data
			memset(buf,'\0', BUFLEN);
			//try to receive some data, this is a blocking call
			if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen) == SOCKET_ERROR)
			{
				//printf("recvfrom() failed with error code : %d" , WSAGetLastError());
				return false;
				//exit(EXIT_FAILURE);
			}
			
			closesocket(s);
			WSACleanup();
			if (strcmp(buf, "ACK") == 0)
				return true;
			else
				return false;
		}
};

/*int main() {
	HostFinder hostFinder;
	std::cout << hostFinder.find(6969, 10, 0, false);
	return 0;
}*/


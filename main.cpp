#include <iostream>

#include "CNetworkLib.c"
#include "PortAndDBLib.cpp"

void listenerLoop(int sockfd, sqlite3* db, QextSerialPort* port){
	char buf[256];
	bool receiveMessage(true);
	int numBytes;
	while(receiveMessage){
		if ((numBytes=recv(sockfd, buf, 256, 0)) != -1) {
			if(numBytes == 0){
				std::cout << "    recv failed" << std::endl;
				receiveMessage = false;
			}else{
		  		buf[numBytes] = '\0';
		  		searchAndExec(db, port, std::string(buf));
		  	}
		} else {
			std::cout << "    recv failed" << std::endl;
			receiveMessage = false;
		}
  	}
}

int main(){
	std::cout << "==================== DemotiK Server ====================" << std::endl;
	std::cout << std::endl;
	std::cout << "Initialisation of all the parameters..." << std::endl;
	bool mainLoop(true);

	std::cout << "    Initialisation of the socket listener..."  << std::endl;
	int socketListener = initConnection(), connfd;
	std::cout << "    Done !" << std::endl;
	std::cout << "    Opening of the USB port to Arduino..." << std::endl;
	QextSerialPort* port = createPort();
	std::cout << "    Done !" << std::endl;
	std::cout << "    Opening of the sqlite3 database..." << std::endl;
	sqlite3* db = openDatabase();
	std::cout << "    Done !"  << std::endl;
	std::cout << "Initialisation Done !" << std::endl;

	std::cout << "Begining to listen on port..." << std::endl;
	while (mainLoop){
		std::cout << "    New lease turn" << std::endl;
		int pid = -1, connfd = -1;

		if((connfd = accept(socketListener, (struct sockaddr*)NULL, NULL)) !=-1){; 
			if((pid = fork()) == 0){
				//Si processus fils
				std::cout << "    New device connected" << std::endl;
				listenerLoop(connfd, db, port);
				std::cout << "    Stopping to listen to device" << std::endl;
				exit(0);
			} else if (pid < 0) {
				std::cout << "    [Error - main] Fork failed" << std::endl;
				exit(1);
			}
		}
	}

	std::cout << "Program stopping, closing all instances..." << std::endl;
	close(socketListener);
	close(connfd);
	sqlite3_close(db) ;
	port->close();
	delete port;
	std::cout << "All instances closed, goodbye !" << std::endl;

	return 0;
}
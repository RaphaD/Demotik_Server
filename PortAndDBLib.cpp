#ifndef __PORT_AND_DB_LIB
#define __PORT_AND_DB_LIB

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sqlite3.h>

#include <iostream>
#include <string>

#include "qextserialport.h"

#define PORT_NAME "/dev/ttyACM0"
#define DATABASE "/usr/local/include/Demotic.db"
#define MAXLEN  256
#define MSGKEY  5L
#define COMMAND_SIZE 4

static int sendOrderFromDB(void *data, int argc, char **argv, char **azColName);
void searchAndExec(sqlite3* db, QextSerialPort* port, std::string message);
sqlite3* openDatabase() ;
void sendData(QextSerialPort* port, char toSend[MAXLEN]);
QextSerialPort* createPort();

QextSerialPort* createPort(){
	//on commence par créer l'objet port série
	QextSerialPort* port = new QextSerialPort(QString(PORT_NAME));

	//on règle la vitesse utilisée
	port->setBaudRate(BAUD9600);

	//quelques règlages pour que tout marche bien
	port->setParity(PAR_NONE);//parité
	port->setStopBits(STOP_1);//nombre de bits de stop
	port->setDataBits(DATA_8);//nombre de bits de données
	port->setFlowControl(FLOW_OFF);//pas de contrôle de flux

	try{
		//on démarre !
		port->open(QextSerialPort::ReadWrite);
	}catch(std::exception const& e){
		std::cout << "        [Error - createPort] Opening of port failed" << std::endl;
	}
	return port;
}

void sendData(QextSerialPort* port, char toSend[MAXLEN]){
	const char* data = (std::string(toSend) + ";").c_str() ;
	std::cout << "        Sending data " << data << " to arduino" << std::endl;
	
	if(port != 0)
		port->write(data);
}

sqlite3* openDatabase() {
	sqlite3* db;

	int rc=sqlite3_open(DATABASE, &db) ;
	if (rc) {
		std::cout << "        [Error - openDatabase] Opening " << DATABASE << " failed !" << std::endl ;
		exit(0) ;
	}

	return db ;
}

void searchAndExec(sqlite3* db, QextSerialPort* port, std::string message){
	char* zErrMsg = 0 ;

	std::string stringSql = "SELECT id FROM CommandId WHERE Command = \""+ message +"\";" ;
	std::cout << "        SQL Request : " << std::endl;
	std::cout << "        " << stringSql << std::endl;

	int rc = sqlite3_exec(db, stringSql.c_str(), sendOrderFromDB, (void*) port, &zErrMsg) ;

	if (rc != SQLITE_OK) {
		std::cout << "        [Error - searchAndExec] SQL request failed " << zErrMsg << std::endl ;
		sqlite3_free(zErrMsg) ;
	} else {
		std::cout << "        SQL request successfull" << std::endl ;
	}
}


static int sendOrderFromDB(void *data, int argc, char **argv, char **azColName){
	sendData((QextSerialPort*) data, argv[0]) ;
	return 0;
}

#endif

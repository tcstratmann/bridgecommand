/*   Bridge Command 5.0 - MergeToolSocket Integration
     Copyright (C) 2016 Dierk Brauer
     dierk.brauer@uni-oldenburg.de
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/time.h>
#include "MergeToolSocket.hpp"

#ifdef __WIN32__
# include <winsock2.h>
# include <windows.h>
# include <unistd.h>
# include <WS2tcpip.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif

//#include "sio_client.h"

#include "SimulationModel.hpp"
#include "Utilities.hpp"
#include "Constants.hpp"

const int DEFAULT_PORT = 5006;

using namespace std;

bool connected = false;
MergeToolSocket::MergeToolSocket() //Constructor
{
    model=0; //Not linked at the moment
    std::cout << "Init MergeToolSocket";

    //startServer(DEFAULT_PORT);
}

MergeToolSocket::~MergeToolSocket() //Destructor
{
    //shut down socket.io connection
	// TODO implement

    std::cout << "Shut down MergeToolSocket connection\n";
    close(newsockfd);
    close(sockfd);
}

bool MergeToolSocket::startServer(int portno){
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));

     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
     if (newsockfd < 0)
          error("ERROR on accept");
     connected = true;
     /*
     bzero(buffer,256);
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) error("ERROR writing to socket");
     */
}

void MergeToolSocket::error(const char *msg)
{
    perror(msg);
    exit(1);
}

void MergeToolSocket::setModel(SimulationModel* model)
{
	this->model = model;
}

void MergeToolSocket::update()
{
    if(connected){
        receive();
        send();
    }
}

void MergeToolSocket::send()
{
	/* type: irr::f32
	model->getLat()
	model->getLong()
	model->getCOG() //FIXME: currently the same as getHeading(), because current is not implemented, yet
	model->getSOG() //FIXME: currently the same as getSpeed(), because current is not implemented, yet
	model->getSpeed() // speed through water
	model->getHeading() // compass course
	model->getRudder()
	model->getRateOfTurn()
	model->getPortEngineRPM()
	model->getStbdEngineRPM()
	model->getPortEngine() //FIXME: currently commented out in SimulationModel
	model->getStbdEngine() //FIXME: currently commented out in SimulationModel
	*/





    /*struct timeval tv;
    gettimeofday(&tv, 0);
    long int ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    */

    std::string msg = ""; // + CMD_UPDATE_ENV_ELEM + to_string(ms) + ";";


    //ee_WaterSupply_long,8.187772,DOUBLE,;

    msg+= "ie_own_ship_course_og," + to_string(model->getCOG()) + ",STRING,;";
    msg+= "ie_own_ship_speed_og," + to_string(model->getSOG()) + ",DOUBLE,;";
    //msg+= "ie_own_ship_name," + to_string(model->getLat()) + ",STRING,;";
    //msg+= "ie_own_ship_type," + to_string(model->getLat()) + ";";

    msg+= "ie_own_ship_turn_rate," + to_string(model->getRateOfTurn()) + ";";
    msg+= "ie_own_ship_rudder_angle," + to_string(model->getRudder()) + ";";
    msg+= "ie_own_ship_orientation," + to_string(model->getHeading()) + ";";


/*for number to getNumberOfOtherShips():
	getOtherShipName(int number)
	getOtherShipPosX(int number) // convert to lat
	getOtherShipPosZ(int number) // convert to long
	getOtherShipHeading(int number)
	getOtherShipSpeed(int number)
	getOtherShipName
end for*/
    int numOtherShips = model->getNumberOfOtherShips();

/*		<infElem name="ie_ship1_course_og" dataType="STRING" />
		<infElem name="ie_ship1_speed_og" dataType="DOUBLE" />
		<infElem name="ie_ship1_name" dataType="STRING" />
		<infElem name="ie_ship1_tcpa" dataType="DOUBLE" />  <!-- time to closest point of Approach -->
*/

    for(int i=0; i<numOtherShips; i++){
	string prefix = "ie_ship" + to_string(i);
    	msg+= prefix + "_name," + model->getOtherShipName(i) + ",STRING,;";
    	msg+= prefix + "_course_og," + to_string(model->getOtherShipSpeed(i)) + ",STRING,;";
    	msg+= prefix + "_speed_og," + to_string(model->getOtherShipSpeed(i))  + ",DOUBLE,;";
    	//msg+= prefix + "_tcpa;" + to_string(model->TODO(i) + ";"); // TODO
    }

    int n = write(newsockfd, msg.c_str() , msg.size());

    if (n < 0) error("ERROR writing to socket");
}

void MergeToolSocket::receive()
{
	/* type: irr::f32
	model->setRudder(irr::f32 rudder)
	model->setPortEngine(irr::f32 port)
	model->setStbdEngine(irr::f32 stbd)
	*/
}


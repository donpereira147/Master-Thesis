#include "veins/modules/application/traci/SocketRSUSimulatorVehicle.h"
#include "veins/modules/application/traci/RouteModifier.h"
//#include "veins/modules/messages/BSM.h"
#include "veins/modules/application/traci/CommonOps.h"

#include <sstream>
#include <math.h>
#include <stdlib.h>
#include "pugixml.hpp"
#include <time.h>
#include <algorithm>
#include <chrono>

#include <iostream>
#include <winsock2.h>
#include <winsock.h>

#define _USE_MATH_DEFINES
#define NODES_NUMBER 15
#define INF 99999
#define SECURITY_BUFFER_AMBULANCE_CROSSING  20

Define_Module(SocketRSUSimulatorVehicle);


void socketThreadRSUV(ThreadData* evD)
{
    //ciclo while que estÃ¡ sempre Ã  espera de receber mensagens
    while(evD != NULL && !evD->isOver)
    {

        WSADATA WSAData;
        SOCKET server, client;
        SOCKADDR_IN serverAddr, clientAddr;

        WSAStartup(MAKEWORD(2,0), &WSAData);
        server = socket(AF_INET, SOCK_STREAM, 0);

        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(6666);

        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

        listen(server, 10000);


        char buffer[200];
                            //std::vector<char> buf(2000);
        int clientAddrSize = sizeof(clientAddr);

        if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
        {

                     //   std::cout << "1->RSU====>quantas vezes aqui\n";
            if(/*readyToReceive(client) == true*/ 1)
            {
                       //     std::cout << "2->RSU====>quantas vezes aqui\n";
                            //ead data
                recv(client, buffer, sizeof(buffer), 0);
                            //std::cout << "RECEBI ISTO: " << buffer << "\n";
                std::string csv(buffer);
                std::cout << "RSU-->buffer: " << buffer << "\n";
                BSM* newBSM ;
                std::vector<std::string> campos = CommonOps::split(csv, ",");


                if(campos.size() == 32)
                {
                    std::string xml =  newBSM->createXML(stoi(campos[30]), stoi(campos[31]), stoi(campos[29]), stoi(campos[3]), stoi(campos[4]),stoi( campos[6]), stoi(campos[7]), 0, stoi(campos[9]), campos[1]);

                    std::cout << "RSU-->transformação de volta para  xml: " << xml << "\n";
                                //recebe uma string correspondente Ã  mensagem BSM
                                //SE CALHAR Ã‰ PRECISO AUMENTAR ESTE VALOR DO BUFFER
                                //guarda nas variÃ¡veis globais a string da BSM

                    evD->bsmString = xml;

                                //coloca a variÃ¡vel de mensagem recebida a true para a outra thread tratar disso
                    evD->receivedMessage = true;

                }
                memset(buffer, 0, sizeof(buffer));

            }
            else
            {
             std::cout << "nought to read\n";
         }
     }
     closesocket(client);
     closesocket(server);
 }
}


void SocketRSUSimulatorVehicle::initSocket()
{
    WSAStartup(MAKEWORD(2,0), &sd.WSAData);
    sd.server = socket(AF_INET, SOCK_STREAM, 0);

    sd.addr.sin_addr.s_addr = inet_addr("192.168.0.9");
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_port = htons(7070);

    connect(sd.server, (SOCKADDR *)&sd.addr, sizeof(sd.addr));

}



/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void SocketRSUSimulatorVehicle::initialize(int stage)
{

    BaseWaveApplLayer::initialize(stage);

    if (stage == 0)
    {
        //para tornar esta secção genérica, ir buscar os semáforos e depois usar o método que dá as estradas que levam aos semáforos
        //buscar ids dos semáforos

        //para cada id preencher a lista com as ruas dos semáforos



        //std::cout << "-->" << findHost()->getFullName() << " com posX:" << mobility->getCurrentPosition().x << ", posY:" << mobility->getCurrentPosition().y << "\n";

        //colocar o carro na faixa da esquerda
       // traciVehicle->changeLane(1, 10);

        data.isOver = false;

        initThread = false;
        data.receivedMessage = false;

        lastString = "";

    }
}

void SocketRSUSimulatorVehicle::finish()
{
    BaseWaveApplLayer::finish();
/*
    closesocket(sd.server);
    WSACleanup();

    data.isOver = true;
    thr.join();*/
}



/*
 *
 *
 * --------COMMUNICATIONS------------
 *
 *
 */

void SocketRSUSimulatorVehicle::handleReceivedMessage(std::string bsmString)
{
    //enviar a mensagem por bsm (simulador) para a RSU
    if(!bsmString.empty())
    {
        BasicSafetyMessage* bsm = new BasicSafetyMessage();
        bsm->setXmlBSM(bsmString);
        populateWSM(bsm);
        populateWSM(bsm);
        sendDelayedDown(bsm, 0);

    }
}

void createAndSendBSMThroughSocketRSUV(SocketData sd, std::string xml)
{

    if(!xml.empty())
    {
        std::string csv = XMLParser::transformXMLIntoCSV(xml);

        //std::cout << "nv::enviei isto: \n" <<csv << "\n\n";
        char buffer[csv.length()+1];
        strcpy(buffer, csv.c_str());
        send(sd.server, buffer, sizeof(buffer), 0);
    }
}

void SocketRSUSimulatorVehicle::onBSM(BasicSafetyMessage *bsm)
{
    std::cout << "------>vou enviar de novo para a placa:"<< simTime << "\n";
    //enviar para a placa
    createAndSendBSMThroughSocketRSUV(sd, bsm->getXmlBSM());
}

/*
 *
 *
 * --------HANDLE CAR BEHAVIOUR------------
 *
 *
 */

void SocketRSUSimulatorVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;



    if(data.receivedMessage)
    {
        data.receivedMessage = false;
        //se for igual � anterior, n�o faz isto
        if(data.bsmString.compare(lastString) != 0)
        {
            lastString = data.bsmString;
            handleReceivedMessage(data.bsmString);
        }
    }
    if(!initThread)
    {
        thr = std::thread(socketThreadRSUV, &data);
        initThread = true;
        initSocket();
    }

}



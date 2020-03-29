#include "veins/modules/application/traci/SocketNormalVehicle.h"
#include "veins/modules/application/traci/RouteModifier.h"
//#include "veins/modules/messages/BSM.h"
#include "XMLParser.h"
#include "veins/modules/application/traci/GeometricOperations.h"
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

#define _USE_MATH_DEFINES
#define NODES_NUMBER 15
#define INF 99999
#define SECURITY_BUFFER_AMBULANCE_CROSSING  20

Define_Module(SocketNormalVehicle);


bool readyToReceiveNV(int sock)
{
    int interval = 1;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(sock, &fds);
    timeval tv;
    tv.tv_sec = interval;
    tv.tv_usec = 0;
    return(select(sock+1, &fds, 0, 0, &tv) == 1);
}


void socketThreadNV(ThreadData* evD)
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
                          serverAddr.sin_port = htons(4444);

                          bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

                           listen(server, 10000);


                          char buffer[200];
                          //std::vector<char> buf(2000);
                          int clientAddrSize = sizeof(clientAddr);

                  if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
                  {

                      //std::cout << "1->NV====>quantas vezes aqui\n";
                      if(/*readyToReceive(client) == true*/ 1)
                      {
                        //  std::cout << "2->NV====>quantas vezes aqui\n";
                          //ead data
                          recv(client, buffer, sizeof(buffer), 0);
                          //std::cout << "RECEBI ISTO: " << buffer << "\n";
                          std::string csv(buffer);
                          //std::cout << "NV-->buffer: " << buffer << "\n";
                          BSM* newBSM ;
                          std::vector<std::string> campos = CommonOps::split(csv, ",");


                          if(campos.size() == 32)
                          {
                              std::string xml =  newBSM->createXML(stoi(campos[30]), stoi(campos[31]), stoi(campos[29]), stoi(campos[3]), stoi(campos[4]),stoi( campos[6]), stoi(campos[7]), 0, stoi(campos[9]), campos[1]);

                          //    std::cout << "NV-->transformação de volta para  xml: " << xml << "\n";
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


void SocketNormalVehicle::initSocket()
{
    WSAStartup(MAKEWORD(2,0), &sd.WSAData);
    sd.server = socket(AF_INET, SOCK_STREAM, 0);
 
    sd.addr.sin_addr.s_addr = inet_addr("192.168.0.3");
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_port = htons(9090);
 
    connect(sd.server, (SOCKADDR *)&sd.addr, sizeof(sd.addr));
    
}


/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void SocketNormalVehicle::initialize(int stage)
{
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0)
    {
        sentMessage = false;
        stopped = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        //para tornar esta secção genérica, ir buscar os semáforos e depois usar o método que dá as estradas que levam aos semáforos
        //buscar ids dos semáforos
        std::list<std::string> idsTL = traci->getTrafficlightIds();

        //para cada id preencher a lista com as ruas dos semáforos

        //inicialização de variáveis
        acknTLChange = true;
        acknTLChange = true;
        firstTime = true;
        roadTLChange = "";
        currentRoad = "";
        previousSpeed = -1;

        previousCoords.x = mobility->getCurrentPosition().x;
        previousCoords.y =  mobility->getCurrentPosition().y;

        /*
        std::string nodeName = findHost()->getFullName();
        nodeName = nodeName + ".veinsmobility";
        meuId = getModuleByPath(nodeName.c_str())->getId();
        */

        //std::cout << "-->" << findHost()->getFullName() << " com posX:" << mobility->getCurrentPosition().x << ", posY:" << mobility->getCurrentPosition().y << "\n";

        //colocar o carro na faixa da esquerda
       // traciVehicle->changeLane(1, 10);
        waitingForEVMessage = false;
        initLane = true;
        lastRoad = false;


        initRouting = false;

        properId = CommonOps::createShortIdForSelf("n", findHost()->getFullName());

        previousTime = std::chrono::system_clock::now();

        speedReduction = false;

        data.isOver = false;

        initThread = false;
        data.receivedMessage = false;
        oneTime = true;
        

        
    }
}

void SocketNormalVehicle::finish()
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

void SocketNormalVehicle::handleReceivedMessage(std::string bsmString)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsmString);
    //std::cout << "mensagem recebida, boss, do " << redux->getTemporaryId() <<"\n";
    //não é veículo de emergência
    if(oneTime == true)
        {
        if(redux->getRole() == 6 && redux->getResponseType() == 1 && redux->getVehicleType() == 4 && !checkIfSingleLane())
        {

                //verifica se está no caminho da ambulância
                //->orientação de ambos tem de ser a mesma
                //->o veículo tem de estar à frente da ambulância, ou seja,
                    //dependendo da orientação verificar se as coordenadas são maiores ou menores
                //distância entre eles terá de ser menor que o especificado

                float currentOrientation = mobility->getAngleRad();
                float ambulanceOrientation = redux->getHeading();
                Coord posAmbulance;
                posAmbulance.x = redux->getLongitude() ;
                posAmbulance.y = redux->getLatitude() ;

                if(!waitingForEVMessage)
                {
                    //veículo tem de se desviar para o veículo de emergência
                    if( GeometricOperations::isNearlyEqual(currentOrientation,ambulanceOrientation) == 0
                            && GeometricOperations::vehicleInFrontOfEV(posAmbulance, mobility->getCurrentPosition(), currentOrientation) == 0)
                    {
                        //if para impedir que veículo encoste demasiado cedo
                        if(traci->getDistance(posAmbulance, mobility->getCurrentPosition(), false) < 25 && traciVehicle->getRoadId().find(':') == std::string::npos)
                        {
                            std::cout << "::::TENHO DE ME DESVIAR::::\n";
                            //muda de faixa
                            previousLane = traciVehicle->getLaneIndex();
                            traciVehicle->changeLane(0, 1000000);


                            if(mobility->getSpeed() != 0)
                                previousSpeedStopped = mobility->getSpeed();
                            traciVehicle->setSpeed(0.0);
                            //para e muda o bool para que não faça nada até receber confirmação qu epode continuar
                            waitingForEVMessage = true;


                        }
                    }
                }
                else if((GeometricOperations::isNearlyEqual(currentOrientation, ambulanceOrientation) == 0 && GeometricOperations::eVPassedVehicle(mobility->getCurrentPosition(), posAmbulance, currentOrientation))
                        || (GeometricOperations::isNearlyEqual(currentOrientation, ambulanceOrientation) != 0))
                {
                    if(waitingForEVMessage)
                    {
                        //FAZER AQUI UM IFZÃO QUE, CASO ESTEJA NAQUELAS TRANSIÇÕES MARADAS DE ESTRADA, ANDA UM BOCADO E DEPOIS É QUE MUDA
                        std::cout << "::::VOU VOLTAR A FAIXA NORMAL::::\n";

                        traciVehicle->changeLane(previousLane, 10000);
                        traciVehicle->setSpeed(previousSpeedStopped);
                        waitingForEVMessage = false;
                        wasStoppedCurb = true;

                        oneTime = false;
                    }
                }
            }

        if(!waitingForEVMessage)
        {
            //veículos normais, não de emergência e não acidentados && hostName.compare("node[1]") !=

                //voltar à velocidade anterior, a ambulância não conta
            //std::cout << "previousSpeed: " << previousSpeed << ", timeAmbulance: " << redux->getTimeEmergencyVehicle() << "\n";
                if(redux->getTimeEmergencyVehicle() < 0 && previousSpeed != -1)
                {
                    //std::cout<<"VOU MUDAR A VELOCIDADE que antes era" << previousSpeed << "\n";
                    traciVehicle->setSpeed(previousSpeed);
                    previousSpeed = -1;
                    speedReduction = false;
                    //std::cout << "voltando a vida que tinha com previousSpeed: "<< previousSpeed << " e time: " << redux->getTimeEmergencyVehicle() << " e quem me mandou foi a "<< redux->getTemporaryId() <<"\n";
                    //std::cout<<"MUDEI A VELOCIDADE QUE AGORA E " << mobility->getSpeed() << "\n";
                }
                //recebe mensagem da RSU para alterar a velocidade
                if(redux->getRole() == 8 && redux->getVehicleType() == 2 && redux->getResponseType() == 1 && redux->getTimeEmergencyVehicle() > 0 && !speedReduction)
                {


                    //std::cout << "temporaryId" << redux->getTemporaryId() << "\n";
                    Coord posRSU, coordVehicle;
                    posRSU.x = redux->getLongitude();
                    posRSU.y = redux->getLatitude();

                    coordVehicle.x = mobility->getCurrentPosition().x;
                    coordVehicle.y = mobility->getCurrentPosition().y;

                    float speed = mobility->getSpeed();
                    float time2arriveAmbulance = redux->getTimeEmergencyVehicle();
                    float orientation = mobility->getAngleRad();
                    float orientationAmbulance = redux->getHeading();

                    float distance = traci->getDistance(posRSU, coordVehicle, false);
                    int dir = GeometricOperations::directionIntersection(coordVehicle, posRSU, orientation);

                    if( distance < 250 && dir == 1 &&  GeometricOperations::isNearlyEqual(orientation, orientationAmbulance) != 0)
                    {
                       // std::cout << "entrei no distance < 250 e dir == 1\n";
                        if(time2arriveAmbulance != -1)
                        {
                            //CASO ESTEJA A DAR ERRO NOS STOPS, VERIFICAR ISTO

                            //calcular o tempo que demora a chegar à RSU
                            float time2arriveVehicle = distance / speed;
                            //std::cout << "vehicle:" << time2arriveVehicle << ", ambulance: " << time2arriveAmbulance << "\n";
                            //std::cout<< "id:"<< idVeiculo << " time do vehicle: " << time2arriveVehicle << ", time da ambulanceee: " << time2arriveAmbulance << "\n";
                            //verificar se algum veículo de emergência se aproxima
                         //   std::cout << "timeAmbulance: " << time2arriveAmbulance << ", timeVehicle: " << time2arriveVehicle << "\n";
                           // std::cout << "posRSU.x: " << posRSU.x << ", posRSU.y: " << posRSU.y << "\n";
                            if(time2arriveVehicle < time2arriveAmbulance + SECURITY_BUFFER_AMBULANCE_CROSSING)
                            {
                                    speedReduction = true;

                                    //std::cout << properId << ": tempo menor que a ambulanceee >> "<< redux->getTemporaryId() <<"\n";
                                    //calcular velocidade de modo a que a ambulância chegue primeir
                                    double desiredSpeed = distance / (time2arriveAmbulance + SECURITY_BUFFER_AMBULANCE_CROSSING*2); //menos 5 para ter aquele buffer de segurança

                                    //enviar mensagem com a mudança de velocidade caso o veículo chegue antes do veículo de emergência
                                    //std::cout << findHost()->getFullName() << "vel antes: " << mobility->getSpeed() << " e vel desejada: " << desiredSpeed << "\n";
                                    if(previousSpeed == -1)
                                        previousSpeed = mobility->getSpeed();
                                 //   std::cout << "spid: " << speed << ", desiredSpid:" << desiredSpeed << "\n";
                                    traciVehicle->setSpeed(desiredSpeed);
                                    //std::cout << "vel apos: " << mobility->getSpeed() << " e vel desejada: " << desiredSpeed << "\n";

                           }
                        }
                    }
                }
        }
     }
}



void SocketNormalVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //é BSM
        case SEND_BEACON_EVT:
        {

        }

    }
}

void createAndSendBSMThroughSocketNV(SocketData sd, std::string xml)
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

/*
 *
 *
 * --------HANDLE CAR BEHAVIOUR------------
 *
 *
 */

void SocketNormalVehicle::handleLaneChanging()
{

    //evitar erros na saída da interseção
        /*if(!waitingForEVMessage)
            traciVehicle->changeLane(1, 5);*/
    /*if(GeometricOperations::isNearlyEqual(0.0, mobility->getSpeed()) == 0 && traciVehicle->getLaneIndex() == 1 && !waitingForEVMessage && initLane)
        traciVehicle->changeLane(0, 5);
    else*/
    /*if(initLane && traciVehicle->getLaneIndex() == 0)
        traciVehicle->changeLane(1, 5);
    else if(initLane && traciVehicle->getLaneIndex() != 0)
        {
            // std::cout << "2\n";
            initLane = false;
        }
        else if((traciVehicle->getPlannedRoadIds().back().compare(traciVehicle->getRoadId()) == 0) && !lastRoad)
        {
            // std::cout << "3\n";
            traciVehicle->changeLane(1, 1000000000);
            lastRoad = true;
        }



        if(wasStoppedCurb && traciVehicle->getLaneIndex() == 0)
        {
            //std::cout << "4\n";
            traciVehicle->changeLane(1, 5);
            //se for a última estrada, pode ir sempre pela direita
            if(traciVehicle->getPlannedRoadIds().back().compare(traciVehicle->getRoadId()) == 0)
            {
                // std::cout << "5\n";
                traciVehicle->changeLane(1, 1000000000);
            }
        }
        else
        {
            // std::cout << "6\n";
            wasStoppedCurb = false;
        }*/

}

bool SocketNormalVehicle::checkIfSingleLane()
{
    std::string road = traciVehicle->getRoadId();
    bool res = false;
    if(road.find("_") != std::string::npos)
    {
        std::vector<std::string> fields = CommonOps::split(road, "_");
        if(fields[0].compare(":n12") == 0 || fields[0].compare(":n13") == 0 || fields[0].compare(":n14") == 0 || fields[0].compare(":n4") == 0)
        {
            res = true;
        }
    }
    else if(road.compare("12to13") == 0 || road.compare("13to14") == 0
            || road.compare("14to15") == 0 || road.compare("4to5") == 0
            || road.compare("14to13") == 0 || road.compare("5to4") == 0
            || road.compare("12to13") == 0)
        res = true;

    return res;
}

void SocketNormalVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;



    if(data.receivedMessage)
    {
        data.receivedMessage = false;
        handleReceivedMessage(data.bsmString);
    }
    if(!initThread)
    {
        thr = std::thread(socketThreadNV, &data);
        initThread = true;
        initSocket();
    }

    if(!checkIfSingleLane())
        handleLaneChanging();


    //node[0] será o id do veículo de emergência

    Coord current;
    current.x = mobility->getCurrentPosition().x;
    current.y = mobility->getCurrentPosition().y;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    //std::chrono::duration<float> elapsed = now - previousTime;
    std::chrono::duration<float> mili =   std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
    float elapsedTime = mili.count();

    //std::cout << "mili: " << elapsedTime << "\n";
    if(elapsedTime > (1 / 500.0))
    {
        //createAndSendBSM(newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
        previousTime = now;
   //    createAndSendBSMThroughSocketNV(sd, newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
    //}

    //|| mobility->getSpeed() == 0.0
    //if(traci->getDistance(previousCoords, current, true) > 15 )
    //{

      //  previousCoords.x = mobility->getCurrentPosition().x;
       // previousCoords.y = mobility->getCurrentPosition().y;
            //envia mensagem a comunicar a sua posição, velocidade, orientação
        createAndSendBSMThroughSocketNV(sd, newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));

    }

}

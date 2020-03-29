//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

//#define M_PI           3.14159265358979323846  /* pi */
#define SECURITY_BUFFER_AMBULANCE_CROSSING  20
#define DISTANCE_RSU_AREA  250
#define RSU_AREA_RADIUS_UPPER  200 //original era 50
#define RSU_AREA_RADIUS_LOWER  10

#include "veins/modules/application/traci/SocketRSU.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/application/traci/CommonOps.h"
//#include "veins/modules/messages/BSM.h"

Define_Module(SocketRSU);



void socketThread(ThreadData* evD)
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
//thread

void SocketRSU::initSocket()
{
    WSAStartup(MAKEWORD(2,0), &sd.WSAData);
    sd.server = socket(AF_INET, SOCK_STREAM, 0);

    sd.addr.sin_addr.s_addr = inet_addr("192.168.0.9");
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_port = htons(7070);

    connect(sd.server, (SOCKADDR *)&sd.addr, sizeof(sd.addr));

}


void createAndSendBSMThroughSocketRSU(SocketData sd, std::string xml)
{
    if(!xml.empty())
    {
        char buffer[xml.length()+1];
        strcpy(buffer, xml.c_str());
        send(sd.server, buffer, sizeof(buffer), 0);
    }
}

/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void SocketRSU::initializeRSU(double rsuX, double rsuY, double cenX, double cenY, std::string type, bool tlI, std::string road, int id)
{
    posRSU.x = rsuX;
    posRSU.y = rsuY;
    rsuCenter.x = cenX;
    rsuCenter.y = cenY;
    rsuType = type;
    initTL = tlI;
    rsuRoad = road;
    meuId = id;
}

void SocketRSU::initialize(int stage)
{
    BaseWaveApplLayer::initialize(stage);

    socketMessageTrigger = new cMessage("socketMessageTrigger");
    std::string nodeName = findHost()->getFullName();
            nodeName = nodeName + ".mobility";
    if(stage == 0)
    {
        initTL = false;
        initProgram = false;

        //meuId = getModuleByPath(nodeName.c_str())->getId();
        vehicleEnteringIntersectionId = -1;
        initTraci = false;
        time2arriveAmbulance = -1;
        time2arriveAmbulanceTL = -1;
        sentCancel = true;

        if(nodeName.compare("rsu[0].mobility") == 0)
            initializeRSU(270.05, 288.45, 250.05, 258.45, "tl", false, "16to7", getModuleByPath("rsu[0].mobility")->getId());
        else if((nodeName.compare("rsu[2].mobility") == 0) || (nodeName.compare("rsu[1].mobility") == 0))
            initializeRSU(520.05, 288.45, 500.05, 258.45, "tl", false,  "17to8", getModuleByPath("rsu[1].mobility")->getId());
        //else if((nodeName.compare("rsu[2].mobility") == 0) || (nodeName.compare("rsu[1].mobility") == 0))
          //  initializeRSU(770.05, 288.45, 750.05, 258.45, "tl", false, "18to9", getModuleByPath("rsu[2].mobility")->getId());
        else if(nodeName.compare("rsu[3].mobility") == 0)
            initializeRSU(520.05, 514.85, 500.05, 504.85, "ss", true, "", getModuleByPath("rsu[3].mobility")->getId());
        else if(nodeName.compare("rsu[4].mobility") == 0)
            initializeRSU(779.95, 514.85, 559.95, 504.85, "ss", true, "", getModuleByPath("rsu[4].mobility")->getId());
        else if(nodeName.compare("rsu[5].mobility") == 0)
            initializeRSU(779.95 + 250, 288.45, 1000.05, 258.45, "ss", true, "", getModuleByPath("rsu[5].mobility")->getId());



        properId = CommonOps::createShortIdForSelf("r", findHost()->getFullName());

        initThread = false;
        data.receivedMessage = false;

        data.isOver = false;

    }
    else if(stage == 1)
    {
        if((nodeName.compare("rsu[2].mobility") == 0))
            scheduleAt(simTime() + 80, socketMessageTrigger);
        else if((nodeName.compare("rsu[1].mobility") == 0))
            scheduleAt(simTime() + 110, socketMessageTrigger);
    }

}

void SocketRSU::finish()
{
    BaseWaveApplLayer::finish();

   /* closesocket(sd.server);
    WSACleanup();

    data.isOver = true;
    thr.join();*/
}


/*
 *
 *
 * --------GEOMETRIC OPERATIONS----------
 *
 *
 */

bool SocketRSU::vehicleIsInRSUArea(Coord posVeh, int radius)
{
    if (GeometricOperations::distance(posVeh, posRSU) <= radius)
        return 0;
    return 1;
}

/*
 *
 *
 * --------TRAFFIC LIGHTS------------
 *
 *
 */

//side é o lado da interseção
//0-> cima para baixo
//1-> esquerda para direita
//2-> baixo para cima
//3-> direita para esquerda
std::string SocketRSU::changeTrafficLightColours(int side, std::string newProgram)
{
    for(int i = 0; i < newProgram.size(); i++)
        newProgram[i] = 'r';

    switch(side)
    {
    case 0:
        newProgram[0] = 'G';
        newProgram[1] = 'G';
        newProgram[2] = 'G';
        break;
    case 1:
        newProgram[3] = 'G';
        newProgram[4] = 'G';
        newProgram[5] = 'G';
        break;
    case 2:
        newProgram[6] = 'G';
        newProgram[7] = 'G';
        newProgram[8] = 'G';
        break;
    case 3:
        newProgram[9] = 'G';
        newProgram[10] = 'G';
        newProgram[11] = 'G';
        break;
    }
    return newProgram;
}

void SocketRSU::changeTrafficLightLights(float orientation)
{
    if(!initProgram)
    {
        initialPhaseIndex = tl.getPhaseIndex();
        initProgram = true;
    }
    //std::string newProgram = initialProgram;
    /*
    if(orientation.compare("0.000000") == 0)
        newProgram = changeTrafficLightColours(3, newProgram);
    else if(orientation.compare(std::to_string(-M_PI)) == 0)
        newProgram = changeTrafficLightColours(1, newProgram);
    else if(orientation.compare(std::to_string(M_PI/2)) == 0)
        newProgram = changeTrafficLightColours(2, newProgram);
    else if(orientation.compare(std::to_string(-M_PI/2)) == 0)
        newProgram = changeTrafficLightColours(0, newProgram);
    */
    //std::cout << "newProgram post ifs: " << newProgram << "\n";
    std::cout << "orientation: " <<orientation << "\n";
    if(GeometricOperations::isNearlyEqual(orientation, 0) == 0 || GeometricOperations::isNearlyEqual(orientation, -M_PI) == 0)
    {
        std::cout << "priemiro if\n";
        tl.setPhaseIndex(4);
    }

    else if(GeometricOperations::isNearlyEqual(orientation, M_PI/2) == 0 || GeometricOperations::isNearlyEqual(orientation, -M_PI/2) == 0)
    {
        std::cout << "segundo if\n";
        tl.setPhaseIndex(0);
    }

    //tl.setPhaseIndex(0);
}

void SocketRSU::trafficLightLogic(std::string bsmString)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsmString);
    std::cout << "<<<<<<<1>>>>>>>\n";
    if(redux->getResponseType() == 1 && redux->getRole() == 6 && redux->getVehicleType() == 4)
    {
        std::cout << "<<<<<<<2>>>>>>>\n";
        //guardar que existe um veículo de emergência a aproximar-se da área da RSU
        idAmbulance = 1;
        posAmbulance.x = redux->getLongitude();
        posAmbulance.y = redux->getLatitude();
        speedAmbulance = redux->getSpeed();
        orientationAmbulance = redux->getHeading();

        //verificar se é a rsu correta que irá proceder ao processamento da mensagem
        float distance = traci->getDistance(posRSU, posAmbulance, false);

        int dir = GeometricOperations::directionIntersection(posAmbulance, posRSU, orientationAmbulance);

        //verificar se o veículo está na estrada que leva à RSU ou contra
        //caso se verifique, é a RSU correta
        //VALOR INICIAL É DE 250

        std::cout << "distance: " << distance << "\n";
        std::cout << "dir: " << dir << "\n";
        if(distance < 250 && dir == 1)
        {
            std::cout << "<<<<<<<3>>>>>>>\n";
            time2arriveAmbulanceTL = distance / speedAmbulance;
            //std::cout << "valor do time: " << time2arriveAmbulanceTL << "\n";
            sentCancel = false;
            //verificar se está perto o suficiente para alterar as luzes
            //se houver carros presos no semáforo, mudar a cor do semáforo antes, para que este não fique preso atrás desses

            //aproximadamente o tamanho da estrada que tem 3 faixas
            if(distance < 150)
            {
                std::cout << "entrei \n";
                changeTrafficLightLights(orientationAmbulance);
            }

            //enviar mensagem a dizer que há uma ambulância a aproximar-se
            BSM* newBSM;
            //std::cout << "esses gajos deviam estar a desacelerar: " << findHost()->getFullName() <<"\n";
            std::string xml = newBSM->createXML(8, 2, 1, posRSU.y* 100000, posRSU.x* 100000, 0* 100000, redux->getHeading()* 100000, 0, time2arriveAmbulanceTL* 100000, properId);
            createAndSendBSMThroughSocketRSU(sd, xml);


        }
        else if(distance < 50 && distance > 20 && dir == 2 && !sentCancel)
        {

            //colocar time2arrive a -1 para simbolizar que já não há nenuma ambulancia a entrar na rsu
            time2arriveAmbulanceTL = -1;
            sentCancel = true;

            BSM* newBSM;
            createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 1, posRSU.y* 100000, posRSU.x* 100000, 0, 0, 0, -1, properId));

            //voltar ao programa anterior caso seja possível e seguro
            //COLOCAR AQUI UM IF!!!!!!!!!!!!
            //std::cout << "size:-> " << vehiclesInRSUArea.size() << "\n";
            if(vehiclesInRSUArea.size() == 0)
                tl.setPhaseIndex(initialPhaseIndex);
        }

    }
    //veículo normal
    else if(redux->getResponseType() == 2 && redux->getRole() == 0 && redux->getVehicleType() == 4)
    {
        Coord posVeh;
        posVeh.x = redux->getLongitude();
        posVeh.y = redux->getLatitude();
        float orientation = redux->getHeading();

        float distance = traci->getDistance(posRSU, posAmbulance, false);

        int dir = GeometricOperations::directionIntersection(posVeh, posRSU, orientation);

        std::string idVehicle = createIdForPossibleStoppedCar(redux->getLongitude(), redux->getLatitude());
        if(distance < 250 && distance > 30 && dir == 1)
        {
            if(time2arriveAmbulanceTL != -1)
            {
                //se estiver à frente da ambulância, não faz nada, porque vai ter de se desviar
                if(GeometricOperations::isNearlyEqual(orientation, orientationAmbulance) != 0 && GeometricOperations::vehicleInFrontOfEV(posAmbulance, posVeh, orientation) == false )
                {
                    //não está dentro do círculo da interseção
                    if(vehicleIsInRSUArea(posVeh, RSU_AREA_RADIUS_LOWER) == 0)
                        //se não tiver sido inserido na lista dos veiculos dentro da area da RSU, inserir
                        if(std::find(vehiclesInRSUArea.begin(), vehiclesInRSUArea.end(), idVehicle) == vehiclesInRSUArea.end())
                            vehiclesInRSUArea.push_back(idVehicle);
                }
            }
        }
        else if(distance < 250 && dir == 2)
            vehiclesInRSUArea.remove(idVehicle);
    }
}

/*--------STOP SIGNS------------*/
void SocketRSU::stopSignLogic(std::string bsmString)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsmString);
    if(redux->getResponseType() == 1 && redux->getRole() == 6 && redux->getVehicleType() == 4)
    {
        //guardar que existe um veículo de emergência a aproximar-se da área da RSU

        idAmbulance = 1;
        posAmbulance.x = redux->getLongitude();
        posAmbulance.y = redux->getLatitude();
        speedAmbulance = redux->getSpeed();
        orientationAmbulance = redux->getHeading();

        float distance = traci->getDistance(posRSU, posAmbulance, false);

        int dir = GeometricOperations::directionIntersection(posAmbulance, posRSU, orientationAmbulance);
        //std::cout  << ":::::::::::::::::::\name:"<<findHost()->getFullName() <<  "\n";
        if(distance < 250 &&  dir == 1)
        {
            //std::cout << "distancia menor que 250 e dir == 1 com ambulanceTime:"<< time2arriveAmbulance <<"\n";
            time2arriveAmbulance = (traci->getDistance(posAmbulance, posRSU, false)) / speedAmbulance;
            sentCancel = false;
            //enviar mensagem a dizer que uma ambulância se aproxima de uma zona de interseção e vai chegar em x tempo,
            //devendo também ser enviadas as coordenadas da rsu para o veículo conseguir calcular quanto tempo o tempo tem

            //std::cout << "RSU:posRSU.x: " << posRSU.x << ", posRSU.y: " << posRSU.y << ", timeAmbulance:" << time2arriveAmbulance << "\n";

            BSM* newBSM;
            createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 1, posRSU.y* 100000, posRSU.x* 100000, 0, redux->getHeading()* 100000, 0, time2arriveAmbulance* 100000, properId));
        }
        //deixá-lo sair da área da RSU antes de enviar a mensagem para ele voltar à velocidade anterior
        else if(distance < 70 && distance > 30 && dir == 2 && !sentCancel)
        {

            //std::cout << "ja saiu da intersecao\n";
            //VERIFICAR SE JÁ SAIU DA INTERSEÇÃO & QUANTAS MSGS SE DEVEM MANDAR
            //colocar time2arrive a -1 para simbolizar que já não há nenhuma ambulancia a entrar na rsu
            time2arriveAmbulance = -1;
            sentCancel = true;
            //enviar mensagem aos veículos para voltarem à velocidade anterior?
            //sim, mas como?
            //opção 1--> enviar a todos que alteraram a velocidade
            BSM* newBSM;
            createAndSendBSMThroughSocketRSU(sd,newBSM->createXML(8, 2, 1, posRSU.y* 100000, posRSU.x* 100000, 0, 0, 0, -1, properId));
        }
    }
}

/*
 *
 *
 * --------STOPPED CARS------------
 *
 *
 */

std::string SocketRSU::createIdForPossibleStoppedCar(float lon, float lat)
{
    std::string idVeiculo = std::to_string(lon) + "-";
    idVeiculo += std::to_string(lat);
    return idVeiculo;
}

void SocketRSU::handlePossibleStoppedCar(BSMRedux* bsm)
{
    std::string idVeiculo = createIdForPossibleStoppedCar(bsm->getLongitude(), bsm->getLatitude());

    //adicionar os dados do veículo ao map
    if(vehiclesAndStoppedFrequency.count(idVeiculo) == 0)
    {
        vehiclesAndStoppedFrequency[idVeiculo] = 1;
    }
    else
    {
        int freq = vehiclesAndStoppedFrequency[idVeiculo];
        //Recebi 5 mensagens de um veículo com v=0, logo deve estar parado
        if(freq + 1 > 3)
        {
            //std::cout << findHost()->getFullName() << ": ha alguem que esta parado->"<< bsm->getTemporaryId()<< "\n";

            //std::cout << "há um carro parado\n";
            vehiclesAndStoppedFrequency.erase(idVeiculo);

            //cria e envia mensagem de veículo parado

            BSM* newBSM ;
            createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 4, bsm->getLatitude() * 100000,bsm->getLongitude()* 100000, bsm->getSpeed()* 100000, bsm->getHeading()* 100000, 0, 0, properId ));
        }
        else
            vehiclesAndStoppedFrequency[idVeiculo] = freq + 1;
    }
}
/*
 *
 *
 * --------TRAFFIC------------
 *
 *
 */


void SocketRSU::handlePossibleTrafficSituation(BSMRedux* redux)
{
    bool exists = false;
    SlowVehicle* slow = new SlowVehicle(redux->getLongitude(), redux->getLatitude(), redux->getHeading(), redux->getTemporaryId());

    for(auto data : svd)
    {
        Coord pos;
        pos.x = data->getX();
        pos.y = data->getY();
        if(slow->vehiclesAreInTheSameRoad(pos, data->getHeading()).compare("") != 0)
        {
            exists = true;
            data->increment();
            if(data->getNumber() > 2)
            {
                //std::cout << findHost()->getFullName() <<  "ha transito em braga\n";
                BSM* newBSM ;
                createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 4, redux->getLatitude() * 100000, redux->getLongitude()* 100000, redux->getSpeed()* 100000, redux->getHeading()* 100000, 0, 0, properId ));
                createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 4, redux->getLatitude() * 100000, redux->getLongitude()* 100000, redux->getSpeed()* 100000, redux->getHeading()* 100000, 0, 0, properId ));
            }
        }
    }
    if(!exists)
        svd.push_back(new SlowVehicleData(redux->getLongitude(), redux->getLatitude(), redux->getHeading(), 1));
}


/*
 *
 *
 * --------COMMUNICATIONS------------
 *
 *
 */



void SocketRSU::handleReceivedMessage(std::string bsmString)
{

    if(!initTraci)
    {
        manager = Veins::TraCIScenarioManagerAccess().get();
        ASSERT(manager);
        traci = manager->getCommandInterface();
        if (!traci)
            error("Cannot create screenshot: TraCI is not connected yet");
        initTraci = true;
    }

    if(!initTL)
    {
        std::string idSemaforo;
        initTL = true;

        std::list<std::string> trafficLightIds =  traci->getTrafficlightIds();

        bool rsu = false;

        for(std::string id : trafficLightIds)
        {
            if(!rsu)
            {
                tl = traci->trafficlight(id);
                std::list<std::string>lanes =  tl.getControlledLanes();

                for(std::string lane : lanes)
                    if(rsuRoad.compare(lane.substr(0, lane.size()-2)) == 0)
                    {
                        idSemaforo = id;
                        rsu = true;
                    }
            }
        }
        initialPhaseIndex = tl.getPhaseIndex();
    }

    BSMRedux* redux = XMLParser::parseXMLBSM(bsmString);

    std::string hostName =  findHost()->getFullName();
    //if(hostName.compare("rsu[4]") == 0)
      //  std::cout << "response:" << redux->getResponseType() << ", role:" << redux->getRole() << ", vehicle:" << redux->getVehicleType() << "\n";

    //mensagem provém da rsu, ou seja, é mensagem replicada por outra rsu a avisar de um carro acidentado
    if(redux->getVehicleType() == 2 && redux->getRole() == 8 && redux->getResponseType() == 4  && redux->getTimeEmergencyVehicle() == 0)
    {
        //antes existia um mecanismo que impedia que enviasse a própria mensagem,
         //caso haja côdea, tentar implementar isso novamente

        std::string stopped = createIdForPossibleStoppedCar(redux->getLongitude(), redux->getLatitude());
        if(std::find(stoppedVehicles.begin(), stoppedVehicles.end(), stopped) == stoppedVehicles.end())
        {
            stoppedVehicles.push_back(stopped);
            BSM* newBSM ;
            createAndSendBSMThroughSocketRSU(sd, newBSM->createXML(8, 2, 4, redux->getLatitude() * 100000, redux->getLongitude()* 100000, redux->getSpeed()* 100000, redux->getHeading()* 100000, 0, -1, properId ));
        }
    }
    else
    {
        if(redux->getResponseType() == 2 && redux->getRole() == 0 && redux->getVehicleType() == 4)
        {
            double speed = redux->getSpeed();
            if(speed == 0.0)
                handlePossibleStoppedCar(redux);
            else if (speed < 1 && speed > 0)
                handlePossibleTrafficSituation(redux);
            else
                vehiclesAndStoppedFrequency.erase(createIdForPossibleStoppedCar(redux->getLongitude(), redux->getLatitude()));
        }
    }


    //lidar com as mensagens recebidas pelos veículos
    if(rsuType.compare("tl") == 0)//traffic light
        trafficLightLogic(bsmString);
    else if(rsuType.compare("ss") == 0)//stop sign
        stopSignLogic(bsmString);

}

void SocketRSU::onWSM(WaveShortMessage* wsm)
{
}

void SocketRSU::onWSA(WaveServiceAdvertisment* wsa)
{
    //if this RSU receives a WSA for service 42, it will tune to the chan
    if (wsa->getPsid() == 42) {
        mac->changeServiceChannel(wsa->getTargetChannel());
    }
}

void SocketRSU::handlePositionUpdate(cObject* obj)
{

    BaseWaveApplLayer::handlePositionUpdate(obj);

    if(!initThread)
    {
        thr = std::thread(socketThread, &data);
        initThread = true;
        initSocket();
    }
    if(data.receivedMessage)
    {
        data.receivedMessage = false;
        handleReceivedMessage(data.bsmString);
    }
    else
    {
        std::cout << "CARLAJO:"<< simTime() <<"\n";
    }



}

void SocketRSU::handleSelfMsg(cMessage* msg)
{
    std::cout << "aqui:"<<simTime() << ", rsu:" << findHost()->getFullName() <<"\n";
    if(msg->isSelfMessage())
    {
        std::cout << "1\n";
        if(msg == socketMessageTrigger)
        {
            std::cout << "2\n";
            if(data.receivedMessage)
            {
                std::cout << "3\n";
                data.receivedMessage = false;
                handleReceivedMessage(data.bsmString);

                scheduleAt(simTime() + 10, socketMessageTrigger);
            }
        }
    }

}

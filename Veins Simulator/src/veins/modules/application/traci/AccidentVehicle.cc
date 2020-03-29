//
// Copyright (C) 2006-2011 Christoph Sommer <christoph.sommer@uibk.ac.at>
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

#include "veins/modules/application/traci/AccidentVehicle.h"
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

#define _USE_MATH_DEFINES
#define NODES_NUMBER 15
#define INF 99999
#define SECURITY_BUFFER_AMBULANCE_CROSSING  10

Define_Module(AccidentVehicle);

/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void AccidentVehicle::initialize(int stage)
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
        //traciVehicle->changeLane(1, 10);
        waitingForEVMessage = false;
        initLane = true;
        lastRoad = false;

        //std::cout << findHost()->getFullName() << " com id " << myId << "\n";

        initRouting = false;

        properId = CommonOps::createShortIdForSelf("n", findHost()->getFullName());

        justOneTime = false;

        previousTime = std::chrono::system_clock::now();

        fewTests = true;
    }
}

void AccidentVehicle::finish()
{
    BaseWaveApplLayer::finish();
}

/*
 *
 *
 * --------COMMUNICATIONS------------
 *
 *
 */

void AccidentVehicle::onWSA(WaveServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if  (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void AccidentVehicle::onBSM(BasicSafetyMessage *bsm)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsm->getXmlBSM());

    if(!waitingForEVMessage)
    {
        //veículos normais, não de emergência e não acidentados && hostName.compare("node[1]") !=
            //voltar à velocidade anterior, a ambulância não conta
            if(redux->getTimeEmergencyVehicle() < 0 && previousSpeed != -1)
            {
                //std::cout<<"VOU MUDAR A VELOCIDADE que antes era" << previousSpeed << "\n";
                traciVehicle->setSpeed(previousSpeed);
                previousSpeed = -1;
                //std::cout<<"MUDEI A VELOCIDADE QUE AGORA E " << mobility->getSpeed() << "\n";
            }
            //recebe mensagem da RSU para alterar a velocidade
            if(redux->getRole() == 8 && redux->getVehicleType() == 2 && redux->getResponseType() == 1 && redux->getTimeEmergencyVehicle() > 0)
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

                if(distance < 250 && dir == 1 &&  GeometricOperations::isNearlyEqual(orientation, orientationAmbulance) != 0)
                {

                    if(time2arriveAmbulance != -1)
                    {
                        //CASO ESTEJA A DAR ERRO NOS STOPS, VERIFICAR ISTO

                        //calcular o tempo que demora a chegar à RSU
                        float time2arriveVehicle = distance / speed;
                        //std::cout << "vehicle:" << time2arriveVehicle << ", ambulance: " << time2arriveAmbulance << "\n";
                        //std::cout<< "id:"<< idVeiculo << " time do vehicle: " << time2arriveVehicle << ", time da ambulanceee: " << time2arriveAmbulance << "\n";
                        //verificar se algum veículo de emergência se aproxima

                        if(time2arriveVehicle < time2arriveAmbulance)
                        {
                            //std::cout << "tempo menor que a ambulanceee\n";
                            //calcular velocidade de modo a que a ambulância chegue primeir
                            double desiredSpeed = distance / (time2arriveAmbulance + SECURITY_BUFFER_AMBULANCE_CROSSING); //menos 5 para ter aquele buffer de segurança

                            //enviar mensagem com a mudança de velocidade caso o veículo chegue antes do veículo de emergência
                            //std::cout << findHost()->getFullName() << "vel antes: " << mobility->getSpeed() << " e vel desejada: " << desiredSpeed << "\n";
                            if(previousSpeed == -1)
                                previousSpeed = mobility->getSpeed();

                            traciVehicle->setSpeed(desiredSpeed);
                            //std::cout << "vel apos: " << mobility->getSpeed() << " e vel desejada: " << speed << "\n";
                       }
                    }
                }
        }
    }
}


void AccidentVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //é BSM
        case SEND_BEACON_EVT:
        {
            /*
            BasicSafetyMessage* bsm = new BasicSafetyMessage();
                //Put Veh Type on beacon before send it.
                //Put standard beacon parameters
            populateWSM(bsm);
                //send it to mac layer
            sendDown(bsm);
                //schedule time to next periodic beacon...
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            break;
            */}

        }

    /*
    if (WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg)) {
        //send this message on the service channel until the counter is 3 or higher.
        //this code only runs when channel switching is enabled
        sendDown(wsm->dup());
        wsm->setSerial(wsm->getSerial() +1);
        if (wsm->getSerial() >= 3) {
            //stop service advertisements
            stopService();
            delete(wsm);
        }
        else {
            scheduleAt(simTime()+1, wsm);
        }
    }
    else if (BasicSafetyMessage* bsm = dynamic_cast<BasicSafetyMessage*>(msg)) {
        //send this message on the service channel until the counter is 3 or higher.
        //this code only runs when channel switching is enabled
        sendDown(bsm->dup());

        if (wsm->getSerial() >= 3) {
            //stop service advertisements
            stopService();
            delete(wsm);
        }
        else {
            scheduleAt(simTime()+1, wsm);
        }
    }
    else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }*/
}

void AccidentVehicle::createAndSendBSM(std::string xml)
{
    if(!xml.empty())
    {
        BasicSafetyMessage* bsm = new BasicSafetyMessage();
        bsm->setXmlBSM(xml);
        populateWSM(bsm);
        sendDown(bsm);
        scheduleAt(simTime()+1, sendBeaconEvt);
    }
}


/*
 *
 *
 * --------HANDLE CAR BEHAVIOUR------------
 *
 *
 */

void AccidentVehicle::handleLaneChanging()
{
    /*
    traciVehicle->changeLane(1, 2100000);

    //evitar erros na saída da interseção
        if(!waitingForEVMessage)
            traciVehicle->changeLane(1, 5);
        if(initLane && traciVehicle->getLaneIndex() == 0)
        {
            // std::cout << "1\n";
            traciVehicle->changeLane(1, 5);
        }
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

bool AccidentVehicle::checkIfSingleLane()
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

void AccidentVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;


    if(!checkIfSingleLane())
        handleLaneChanging();


    Coord current;
    current.x = mobility->getCurrentPosition().x;
    current.y = mobility->getCurrentPosition().y;

    //envia mensagem a comunicar a sua posição, velocidade, orientação

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    //std::chrono::duration<float> elapsed = now - previousTime;
    std::chrono::duration<float> mili =   std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
    float elapsedTime = mili.count();

    //std::cout << "mili: " << elapsedTime << "\n";
    /*if(elapsedTime > 0.1)
    {
        //envia mensagem a comunicar a sua posiÃ§Ã£o, velocidade, orientaÃ§Ã£o
        createAndSendBSM(newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
      //  std::cout << "muitas vezes\n";
        previousTime = now;

    }*/

    
   /* if(!justOneTime)
    {
        justOneTime = true;
     */   
        createAndSendBSM(newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
    //}

}


void AccidentVehicle::onWSM(WaveShortMessage* wsm)
{}

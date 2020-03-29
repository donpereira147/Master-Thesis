#include "veins/modules/application/traci/NormalVehicle.h"
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
#define SECURITY_BUFFER_AMBULANCE_CROSSING  20

Define_Module(NormalVehicle);

/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void NormalVehicle::initialize(int stage)
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

        fewTests = true;
    }
}

void NormalVehicle::finish()
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

bool canISwitchLanesNV(bool fewTests, std::string road)
{

    std::cout << "\n\n\nROAD: " << road << "\n\n\n";
    if(!fewTests)
        return true;



    if(road.compare("6to7") == 0 || road.compare("7to8") == 0 || road.compare("8to7") == 0
        || road.compare("7to6") == 0  || road.compare("72to78") == 0  || road.compare("78to72") == 0
        || road.compare("23to78") == 0  || road.compare("23to90") == 0  || road.compare("90to24") == 0
        || road.compare("24to90") == 0 || road.compare("96to102") == 0 || road.compare("102to96") == 0
        || road.compare("110to111") == 0 || road.compare("23to17") == 0  || road.compare("44to45") == 0
        || road.compare("45to44") == 0 || road.compare("46to45") == 0 || road.compare("25to17") == 0)
            return false;

    std::cout << "vou enconstar\n";
    return true;
}


void NormalVehicle::onWSA(WaveServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if  (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}



void NormalVehicle::onBSM(BasicSafetyMessage *bsm)
{

    BSMRedux* redux = XMLParser::parseXMLBSM(bsm->getXmlBSM());
    //std:: cout << "time do ev: " << redux->getTimeEmergencyVehicle() << "\n";
    //std::cout << "mensagem recebida, boss, do " << redux->getTemporaryId() <<"\n";
    //não é veículo de emergência

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

                        if(canISwitchLanesNV(fewTests, traciVehicle->getRoadId()) == true)
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

void NormalVehicle::onWSM(WaveShortMessage* wsm)
{
}

void NormalVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //é BSM
        case SEND_BEACON_EVT:
        {

        }

    }
}

void NormalVehicle::createAndSendBSM(std::string xml)
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

void NormalVehicle::handleLaneChanging()
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

bool NormalVehicle::checkIfSingleLane()
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

void NormalVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;

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
    if(elapsedTime > (1 / 10.0))
    {
        //createAndSendBSM(newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
        previousTime = now;
   /* }



        //|| mobility->getSpeed() == 0.0
        if(traci->getDistance(previousCoords, current, true) > 15 )
        {
*/
            previousCoords.x = mobility->getCurrentPosition().x;
            previousCoords.y = mobility->getCurrentPosition().y;
            //envia mensagem a comunicar a sua posição, velocidade, orientação
            createAndSendBSM(newBSM->createXML(0, 4, 2, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));

        }

}

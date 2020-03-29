#include "veins/modules/application/traci/JustKerbsVehicle.h"
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
//mudar para caminho relativo, olhando para os includes de cima

Define_Module(JustKerbsVehicle);

/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void JustKerbsVehicle::initialize(int stage)
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


        initRouting = false;

        properId = CommonOps::createShortIdForSelf("n", findHost()->getFullName());

        previousTime = std::chrono::system_clock::now();

        speedReduction = false;

        fewTests = true;
    }
}

void JustKerbsVehicle::finish()
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

void JustKerbsVehicle::onWSA(WaveServiceAdvertisment* wsa) {
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if  (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

bool canISwitchLanesJKV(bool fewTests, std::string road)
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

void JustKerbsVehicle::onBSM(BasicSafetyMessage *bsm)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsm->getXmlBSM());
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
                    if(traci->getDistance(posAmbulance, mobility->getCurrentPosition(), false) < 15)
                    {
                        if(canISwitchLanesJKV(fewTests, traciVehicle->getRoadId()) == true)
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
                    std::cout << "::::VOU VOLTAR A FAIXA NORMAL::::\n";
                    traciVehicle->changeLane(previousLane, 10000);
                    traciVehicle->setSpeed(previousSpeedStopped);
                    waitingForEVMessage = false;
                    wasStoppedCurb = true;
                }
            }
        }
}

void JustKerbsVehicle::onWSM(WaveShortMessage* wsm)
{
}

void JustKerbsVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //é BSM
        case SEND_BEACON_EVT:
        {

        }

    }
}



/*
 *
 *
 * --------HANDLE CAR BEHAVIOUR------------
 *
 *
 */

void JustKerbsVehicle::handleLaneChanging()
{

    //evitar erros na saída da interseção
        /*if(!waitingForEVMessage)
            traciVehicle->changeLane(1, 5);*/
      /*  if(initLane && traciVehicle->getLaneIndex() == 0)
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

bool JustKerbsVehicle::checkIfSingleLane()
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

void JustKerbsVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    if(!checkIfSingleLane())
        handleLaneChanging();
}

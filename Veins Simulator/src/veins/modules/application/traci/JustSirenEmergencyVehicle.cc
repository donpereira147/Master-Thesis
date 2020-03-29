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

#include "veins/modules/application/traci/JustSirenEmergencyVehicle.h"
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
#define NODES_NUMBER 48
#define INF 99999
#define SECURITY_BUFFER_AMBULANCE_CROSSING  10
//mudar para caminho relativo, olhando para os includes de cima
#define PATH_ROUTES "C:\\Users\\PEB2BRG\\src\\veins-4.7.1\\src\\veins\\modules\\application\\traci\\routingBig.xml"

Define_Module(JustSirenEmergencyVehicle);

/*
 *
 *
 * --------INITIALIZE------------
 *
 *
 */

void JustSirenEmergencyVehicle::initialize(int stage)
{
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0)
    {
        sentMessage = false;
        stopped = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        //para tornar esta secÃ§Ã£o genÃ©rica, ir buscar os semÃ¡foros e depois usar o mÃ©to_do que dÃ¡ as estradas que levam aos semÃ¡foros
        //buscar ids dos semÃ¡foros
        std::list<std::string> idsTL = traci->getTrafficlightIds();

        //para cada id preencher a lista com as ruas dos semÃ¡foros

        //inicializaÃ§Ã£o de variÃ¡veis
        acknTLChange = true;
        acknTLChange = true;
        firstTime = true;
        roadTLChange = "";
        currentRoad = "";
        previousSpeed = -1;

        previousCoords.x = mobility->getCurrentPosition().x;
        previousCoords.y =  mobility->getCurrentPosition().y;

        

        traciVehicle->setColor(Veins::TraCIColor::fromTkColor("RoyalBlue"));

        //init variaveis de estatisticas
        stoppedAt = startedAt = -1;
        totalStoppedTime = totalMaxSpeedTime =  0;

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

        //prepareRoutingOptimization();
        initRouting = false;

        properId = CommonOps::createShortIdForSelf("n", findHost()->getFullName());

        routingNeeded = false;

        previousTime = std::chrono::system_clock::now();

        statistics.initialize();
       // statistics.watch(*this);

        lastUpdate = simTime();

        resultsArePrinted = false;

    }
}

void JustSirenEmergencyVehicle::finish()
{
    BaseWaveApplLayer::finish();

    std::cout << "IMPRIME_TE; BOYYYYYYYYYYYYYYYYYYYYY\n";

    if(!resultsArePrinted)
    {
        resultsArePrinted = true;

        std::ofstream file;
        if(startedAt !=-1)
            totalMaxSpeedTime += simTime().dbl() - startedAt.dbl();

        statistics.stopTime = simTime();
        statistics.recordScalars(*this);
        std::string v2x = "false";

        std::string nameWithWayTooMuchInformation = findHost()->getFullName();
        std::string hostName = CommonOps::split(nameWithWayTooMuchInformation, "[")[0];
        if(hostName.compare("emergency") == 0)
                v2x = "true";


        //o caminho m�nimo poss�vel � 1500, logo qualquer simula��o com um valor inferior apresenta erros
        if(statistics.totalDistance > 1499)
        {
            file.open("testResults.txt", std::ios_base::app);

            file << "v2x:" << v2x << "\n";
            file << "startTime:" << statistics.startTime << "\n";
            file << "stopTime:" << statistics.stopTime << "\n";
            file << "totalTime:" << statistics.stopTime - statistics.startTime << "\n";
            file  << "totalStoppedTime: " << totalStoppedTime << "\n";
            file << "totalDistance:" << statistics.totalDistance << "\n";
            file << "CO2 emission:" << statistics.totalCO2Emission << "\n";
            file << "% of time in full throtle: " << 100 * totalMaxSpeedTime / (statistics.stopTime - statistics.startTime) << "%\n";
            file << "###\n";
        }

        endSimulation();
    }
}

/*
 *
 *
 * ----------ROUTING----------
 *
 *
 */
void JustSirenEmergencyVehicle::prepareRoutingOptimization()
{

    //inicializar a matriz
    //routesCosts[sizeMatrix][sizeMatrix] = {INT_MAX}; //valor grande para nunca ser escolhido no algoritmo de otimizaÃ§Ã£o de rotas

    routesCosts = new int*[NODES_NUMBER];
    for(int i = 0; i < NODES_NUMBER; ++i)
        routesCosts[i] = new int[NODES_NUMBER];

    for(int i = 0; i < NODES_NUMBER; i++)
        for(int j = 0; j < NODES_NUMBER; j++)
            routesCosts[i][j] = INF;


    //ler o ficheiro e preencher o valor das rotas
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(PATH_ROUTES);



    RouteSpecs rs;
    if(result)
    {
        for(pugi::xml_node route: doc.child("routes").children("route"))
        {
            std::string idRoad = route.attribute("id").as_string();
            int time = route.attribute("time").as_int();
            double posxI = route.attribute("xI").as_double();
            double posyI = route.attribute("yI").as_double();
            double posxF = route.attribute("xF").as_double();
            double posyF = route.attribute("yF").as_double();

            rs.setRouteSpecs(posxI, posyI, posxF, posyF, idRoad);
            allRoutes.push_back(rs);

            idRoad += "to";
            std::vector<std::string> fieldsI = CommonOps::split(idRoad, "to");
            int origem = stoi(fieldsI[0]) - 1;
            int destino = stoi(fieldsI[1]) - 1;

            routesCosts[origem][destino] = time;
        }

    }
    else
        std::cout << "ERRO: XML\n";
}

//source do cÃ³digo de dijsktra https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
std::list<std::string> JustSirenEmergencyVehicle::getOptimalRouteThroughDijkstra(int origin, int end)
{
    std::list<std::string> finalPath;
    int dist[NODES_NUMBER];
    int parent[NODES_NUMBER];
    bool sptSet[NODES_NUMBER];

    for (int i = 0; i < NODES_NUMBER; i++)
    {
        parent[i] = -1;
        dist[i] = INT_MAX, sptSet[i] = false;
    }

    dist[origin] = 0;

    for (int count = 0; count < NODES_NUMBER; count++)
    {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        for (int v = 0; v < NODES_NUMBER; v++)
        {
            if (!sptSet[v] && routesCosts[u][v] && dist[u] != INT_MAX  && dist[u] + routesCosts[u][v] < dist[v])
            {
                dist[v] = dist[u] + routesCosts[u][v];
                parent[v] = u;

            }
        }
    }

    allPossibleOptimalPaths = "";
    for (int i = 0; i < NODES_NUMBER; i++)
    {
        allPossibleOptimalPaths += std::to_string(origin);
        allPossibleOptimalPaths += "-";
        prepareAllPaths(parent, i);
        allPossibleOptimalPaths += "||";
    }

    std::string optimalPath;
    std::vector<std::string> paths = CommonOps::split(allPossibleOptimalPaths, "||");
    std::vector<std::string> idsRoutes;

    for(std::string path : paths)
    {
        idsRoutes = CommonOps::split(path, "-");
        if(stoi(idsRoutes.back()) == end)
        {
            optimalPath = path;
            break;
        }
    }

    //criar um vetor com as ruas a partir dos nÃºmeros
    std::string aux;
    for(int j = 0; j < idsRoutes.size()-1; j++)
    {
        int ini = stoi(idsRoutes[j]) + 1;
        int fini = stoi(idsRoutes[j+1]) + 1;
        aux = std::to_string(ini) + "to";
        finalPath.push_back(aux + std::to_string(fini));
    }
    return finalPath;
}

void JustSirenEmergencyVehicle::performOptimalRouting()
{
    int actualLastNode;
    std::string lastRoad = traciVehicle->getPlannedRoadIds().back() + "to";
    //std::cout << "ultima estrada ser : " << lastRoad << "\n";

    int currentNode = stoi(CommonOps::split(traciVehicle->getLaneId(), "to")[0]);
    int lastNode = stoi(CommonOps::split(lastRoad,"to")[1]);
    int nextNode = getNextNodeCurrentRoad();


    if(currentNode == 7 && nextNode == 21)
        nextNode = 8;
    else if(currentNode == 8 && nextNode == 22)
        nextNode = 9;
    else if(currentNode == 9 && nextNode == 27)
        nextNode = 8;
    else if(currentNode == 8 && nextNode == 26)
        nextNode = 7;

    if(lastNode > 28)
        actualLastNode = stoi(CommonOps::split(lastRoad,"to")[0]);
    else
        actualLastNode = lastNode;

     //se calhar vai ser preciso retirar a lane ao getLaneId

     std::list<std::string> optimalPathRedux =
             getOptimalRouteThroughDijkstra(nextNode - 1, actualLastNode - 1);

     std::string inicio = std::to_string(currentNode) + "to";
     inicio += std::to_string(nextNode);


     optimalPathRedux.push_front(inicio);



     //alterar as estradas grandes para estradas piquenas
     optimalPath = RouteModifier::replaceReduxRoads(optimalPathRedux);


     if(lastNode == 29)
         optimalPath.push_back("1to29");
     else if(lastNode == 30)
         optimalPath.push_back("5to30");
     else if(lastNode == 31)
         optimalPath.push_back("11to31");
     else if(lastNode == 32)
         optimalPath.push_back("15to32");

     std::cout << "->CAMINHO OTIMO: " << "\n";
     for(std::string sa : optimalPath)
     {
         std::cout << sa << "__";
     }
     std::cout << "\n";
     //nada de routings nas ruas divididas, nem nas transições de ruas, senão dá erro
     if(currentNode  == 7 || currentNode == 8 || currentNode == 9 || traciVehicle->getLaneId().find(':') != std::string::npos)
     {
         oldRoad = traciVehicle->getLaneId();
         routingNeeded = true;
     }
     else
         traciVehicle->changeVehicleRoute(optimalPath);

     initRouting = true;
}



void JustSirenEmergencyVehicle::prepareAllPaths(int parent[], int j)
{
    // Base Case : If j is source
    if (parent[j] == - 1)
        return;
    prepareAllPaths(parent, parent[j]);
    allPossibleOptimalPaths += std::to_string(j);
    allPossibleOptimalPaths += "-";
}

int JustSirenEmergencyVehicle::getNextNodeCurrentRoad()
{
    bool after = false;
    bool next = false;
    int currentNode = stoi(CommonOps::split(traciVehicle->getLaneId(), "to")[0]);
    for(std::string road : traciVehicle->getPlannedRoadIds())
    {
        road += "to";
        std::vector<std::string> campos = CommonOps::split(road, "to");
        if(next)
        {
         //   std::cout << "resultado: " << stoi(campos[0]) << "\n";
            if(stoi(campos[0]) >= 16 && stoi(campos[0]) <= 28)
            {
                next = false;
                after = true;
            }
            else
                return stoi(campos[0]);
        }
        else
        {
            if(stoi(campos[0]) == currentNode)
                next = true;
            else if (after)
                return stoi(campos[0]);
        }
    }
    return -1;
}

/*
 *
 *
 * --------GEOMETRIC OPERATIONS----------
 *
 *
 */

int JustSirenEmergencyVehicle::minDistance(int dist[], bool sptSet[])
{
   // Initialize min value
   int min = INT_MAX, min_index;

   //mudar para a variÃ¡vel global
   for (int v = 0; v < 15; v++)
     if (sptSet[v] == false && dist[v] <= min)
         min = dist[v], min_index = v;

   return min_index;
}

/*
 *
 *
 * --------COMMUNICATIONS------------
 *
 *
 */

void JustSirenEmergencyVehicle::onBSM(BasicSafetyMessage *bsm)
{
    
}

void JustSirenEmergencyVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //Ã© BSM
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

}

void JustSirenEmergencyVehicle::createAndSendBSM(std::string xml)
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

void JustSirenEmergencyVehicle::handleLaneChanging()
{
/*
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
        }
    */
}

bool JustSirenEmergencyVehicle::checkIfSingleLane()
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
            || road.compare("14to13") == 0 || road.compare("5to4") == 0)
        res = true;

        res = true;

    return res;
}

void JustSirenEmergencyVehicle::recordStatistics()
{
    if(traciVehicle->getRoadId().compare("29to1") != 0 && traciVehicle->getRoadId().compare("30to5") != 0 && traciVehicle->getRoadId().compare("31to11") != 0 && traciVehicle->getRoadId().compare("32to15") != 0)
    {
        if(mobility->getSpeed() == 0.0 && stoppedAt == -1 && stoppedAt != 1)
                    stoppedAt = simTime();
                else if (mobility->getSpeed() != 0.0 && stoppedAt != -1)
                {
                    std::cout << "elapsed: " << simTime().dbl() << "-" <<  stoppedAt.dbl()<<"\n";
                    totalStoppedTime += (simTime() - stoppedAt).dbl();
                    stoppedAt = -1;
                }

        if(mobility->getSpeed() == 3.0 && startedAt == -1)
        {
            startedAt = simTime();
        }
        else if(mobility->getSpeed() != 3.0 && startedAt != -1)
        {
            totalMaxSpeedTime += simTime().dbl() - startedAt.dbl();
            startedAt = -1;
        }

    }

}

void JustSirenEmergencyVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;

    recordStatistics();
/*
    if(!initRouting)
    {
        performOptimalRouting();
        last_speed = mobility->getSpeed();
    }
    else
    {

        if(routingNeeded == true)
        {
            std::string roadX = traciVehicle->getLaneId();
            if( roadX.compare(oldRoad) != 0  && isdigit(roadX[0]))
            {
                optimalPath.pop_front();
                traciVehicle->changeVehicleRoute(optimalPath);
                routingNeeded = false;
            }
        }
    }
*/
    if(!checkIfSingleLane() && traciVehicle->getLaneId().find(':') != std::string::npos)
        handleLaneChanging();

    Coord current;
    current.x = mobility->getCurrentPosition().x;
    current.y = mobility->getCurrentPosition().y;

    //traciVehicle->setSpeedMode(7);

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    //std::chrono::duration<float> elapsed = now - previousTime;
    std::chrono::duration<float> mili =   std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
    float elapsedTime = mili.count();

    //std::cout << "mili: " << elapsedTime << "\n";
    if(elapsedTime > (1 / 100.0))
    {
        //envia mensagem a comunicar a sua posiÃ§Ã£o, velocidade, orientaÃ§Ã£o
        //createAndSendBSM(newBSM->createXML(6, 4, 1, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
      //  std::cout << "muitas vezes\n";
        previousTime = now;
    }


    if(traci->getDistance(previousCoords, current, true) > 10)
    {
        simtime_t updateInterval = simTime() - lastUpdate;
        double co2emission = mobility->calculateCO2emission(mobility->getSpeed(), (mobility->getSpeed() - last_speed) / updateInterval);

        statistics.totalCO2Emission += co2emission * updateInterval.dbl();


        statistics.totalDistance += GeometricOperations::distance(previousCoords, current);
        previousCoords.x = mobility->getCurrentPosition().x;
        previousCoords.y = mobility->getCurrentPosition().y;

        //envia mensagem a comunicar a sua posiÃ§Ã£o, velocidade, orientaÃ§Ã£o
        createAndSendBSM(newBSM->createXML(6, 4, 1, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));

    }
    lastUpdate = simTime();
    last_speed = mobility->getSpeed();
}

void JustSirenEmergencyVehicle::onWSM(WaveShortMessage* wsm){}

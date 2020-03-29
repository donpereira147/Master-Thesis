#include "veins/modules/application/traci/SocketEmergencyVehicle.h"
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
#include <winsock.h>

#define _USE_MATH_DEFINES
#define NODES_NUMBER 15
#define INF 99999
#define SECURITY_BUFFER_AMBULANCE_CROSSING  10
//mudar para caminho relativo, olhando para os includes de cima
#define PATH_ROUTES "C:\\Users\\PEB2BRG\\src\\veins-4.7.1\\src\\veins\\modules\\application\\traci\\routing.xml"



Define_Module(SocketEmergencyVehicle);

/*
 *
 *
 * ------------INITIALIZE------------
 *
 *
 */


bool readyToReceive(int sock)
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

//thread
void socketThread(EVData* evD/*, Veins::TraCICommandInterface::Vehicle* tv*/)
{
            //ciclo while que está sempre à espera de receber mensagens
            while(evD != NULL && !evD->isOver)
            {

                WSADATA WSAData;
                        SOCKET server, client;
                        SOCKADDR_IN serverAddr, clientAddr;

                        WSAStartup(MAKEWORD(2,0), &WSAData);
                        server = socket(AF_INET, SOCK_STREAM, 0);

                        serverAddr.sin_addr.s_addr = INADDR_ANY;
                                        serverAddr.sin_family = AF_INET;
                        serverAddr.sin_port = htons(5555);

                        bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

                         listen(server, 10000);


                        char buffer[200];
                        //std::vector<char> buf(2000);
                        int clientAddrSize = sizeof(clientAddr);

                if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
                {

                   // std::cout << "1->EV====>quantas vezes aqui\n";
                    if(/*readyToReceive(client) == true*/ 1)
                    {
                     //   std::cout << "2->EV====>quantas vezes aqui\n";
                        //ead data
                        recv(client, buffer, sizeof(buffer), 0);
                       // std::cout << "RECEBI ISTO: " << buffer << "\n";
                        std::string csv(buffer);
                        //std::cout << "EV-->buffer: " << buffer << "\n";
                        BSM* newBSM ;
                        std::vector<std::string> campos = CommonOps::split(csv, ",");


                        if(campos.size() == 32)
                        {
                            std::string xml =  newBSM->createXML(stoi(campos[30]), stoi(campos[31]), stoi(campos[29]), stoi(campos[3]), stoi(campos[4]),stoi( campos[6]), stoi(campos[7]), 0, stoi(campos[9]), campos[1]);

                       //     std::cout << "EV-->transforma��o de volta para  xml: " << xml << "\n";
                            //recebe uma string correspondente à mensagem BSM
                            //SE CALHAR É PRECISO AUMENTAR ESTE VALOR DO BUFFER
                            //guarda nas variáveis globais a string da BSM

                            evD->bsmString = xml;

                            //coloca a variável de mensagem recebida a true para a outra thread tratar disso
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



 /*   //código retirado de https://causeyourestuck.io/2016/01/17/socket-cc-windows/
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;

    WSAStartup(MAKEWORD(2,0), &WSAData);
    server = socket(AF_INET, SOCK_STREAM, 0);
 
    serverAddr.sin_addr.s_addr = INADDR_ANY;
                    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);

    bind(server, (SOCKADDR *)&serverAddr, sizeof(serverAddr));

     listen(server, 10000);

  
    char buffer[200];
    //std::vector<char> buf(2000);
    int clientAddrSize = sizeof(clientAddr);

    if((client = accept(server, (SOCKADDR *)&clientAddr, &clientAddrSize)) != INVALID_SOCKET)
    {
        //ciclo while que está sempre à espera de receber mensagens
        while(evD != NULL && !evD->isOver)
        {std::cout << "1->EV====>quantas vezes aqui\n";
            if(/*readyToReceive(client) == true 1)
            {
                std::cout << "2->EV====>quantas vezes aqui\n";
                //ead data
                recv(client, buffer, sizeof(buffer), 0);
                //std::cout << "RECEBI ISTO: " << buffer << "\n";
                std::string csv(buffer);
                std::cout << "EV-->buffer: " << buffer << "\n";
                BSM* newBSM ;
                std::vector<std::string> campos = CommonOps::split(csv, ",");


                if(campos.size() == 32)
                {
                    std::string xml =  newBSM->createXML(stoi(campos[30]), stoi(campos[31]), stoi(campos[29]), stoi(campos[3]), stoi(campos[4]),stoi( campos[6]), stoi(campos[7]), 0, stoi(campos[9]), campos[1]);

                    std::cout << "EV-->transforma��o de volta para  xml: " << xml << "\n";
                    //recebe uma string correspondente à mensagem BSM
                    //SE CALHAR É PRECISO AUMENTAR ESTE VALOR DO BUFFER
                    //guarda nas variáveis globais a string da BSM

                    evD->bsmString = xml;

                    //coloca a variável de mensagem recebida a true para a outra thread tratar disso
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
    }*/
}

void SocketEmergencyVehicle::initSocket()
{
    WSAStartup(MAKEWORD(2,0), &sd.WSAData);
    sd.server = socket(AF_INET, SOCK_STREAM, 0);
    //10.1.1.3 é a placa 
    sd.addr.sin_addr.s_addr = inet_addr("10.1.1.3");
    sd.addr.sin_family = AF_INET;
    sd.addr.sin_port = htons(8080);
 
    connect(sd.server, (SOCKADDR *)&sd.addr, sizeof(sd.addr));
    
}



void SocketEmergencyVehicle::initialize(int stage)
{
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0)
    {
        data.sentMessage = false;
        data.stopped = false;
        data.lastDroveAt = simTime();
        data.currentSubscribedServiceId = -1;

        //inicializaÃ§Ã£o de variÃ¡veis

        data.previousCoords.x = mobility->getCurrentPosition().x;
        data.previousCoords.y =  mobility->getCurrentPosition().y;

        

        traciVehicle->setColor(Veins::TraCIColor::fromTkColor("RoyalBlue"));

        //init variaveis de estatisticas
        data.stoppedAt = data.startedAt = -1;
        data.totalStoppedTime = data.totalMaxSpeedTime =  0;

        data.initLane = true;
        data.lastRoad = false;

        //prepareRoutingOptimization();
        //data.initRouting = false;

        data.properId = CommonOps::createShortIdForSelf("e", findHost()->getFullName());

        data.routingNeeded = false;

        previousTime = std::chrono::system_clock::now();

        data.statistics.initialize();

        data.lastUpdate = simTime();

        data.resultsArePrinted = false;

        data.isOver = false;

        //lançar a thread
        initThread = false;
        data.receivedMessage = false;

        
    }
}

void SocketEmergencyVehicle::finish()
{
    BaseWaveApplLayer::finish();

    /*    closesocket(sd.server);
        WSACleanup();

        data.isOver = true;
        thr.join();*/
}

/*
 *
 *
 * ----------ROUTING----------
 *
 *
 */
void SocketEmergencyVehicle::prepareRoutingOptimization()
{

    //inicializar a matriz
    //routesCosts[sizeMatrix][sizeMatrix] = {INT_MAX}; //valor grande para nunca ser escolhido no algoritmo de otimizaÃ§Ã£o de rotas

    data.routesCosts = new int*[NODES_NUMBER];
    for(int i = 0; i < NODES_NUMBER; ++i)
        data.routesCosts[i] = new int[NODES_NUMBER];

    for(int i = 0; i < NODES_NUMBER; i++)
        for(int j = 0; j < NODES_NUMBER; j++)
            data.routesCosts[i][j] = INF;


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
            data.allRoutes.push_back(rs);

            idRoad += "to";
            std::vector<std::string> fieldsI = CommonOps::split(idRoad, "to");
            int origem = stoi(fieldsI[0]) - 1;
            int destino = stoi(fieldsI[1]) - 1;

            data.routesCosts[origem][destino] = time;
            //routesCosts[origem][destino] = time;
        }

    }
    else
        std::cout << "ERRO: XML\n";
}

//source do cÃ³digo de dijsktra https://www.geeksforgeeks.org/printing-paths-dijkstras-shortest-path-algorithm/
std::list<std::string> SocketEmergencyVehicle::getOptimalRouteThroughDijkstra(int origin, int end)
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
            if (!sptSet[v] && data.routesCosts[u][v] && dist[u] != INT_MAX  && dist[u] + data.routesCosts[u][v] < dist[v])
            {
                dist[v] = dist[u] + data.routesCosts[u][v];
                parent[v] = u;

            }
        }
    }

    data.allPossibleOptimalPaths = "";
    for (int i = 0; i < NODES_NUMBER; i++)
    {
        data.allPossibleOptimalPaths += std::to_string(origin);
        data.allPossibleOptimalPaths += "-";
        prepareAllPaths(parent, i);
        data.allPossibleOptimalPaths += "||";
    }

    std::string optimalPath;
    std::vector<std::string> paths = CommonOps::split(data.allPossibleOptimalPaths, "||");
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

void SocketEmergencyVehicle::performOptimalRouting()
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
     data.optimalPath = RouteModifier::replaceReduxRoads(optimalPathRedux);


     if(lastNode == 29)
         data.optimalPath.push_back("1to29");
     else if(lastNode == 30)
         data.optimalPath.push_back("5to30");
     else if(lastNode == 31)
         data.optimalPath.push_back("11to31");
     else if(lastNode == 32)
         data.optimalPath.push_back("15to32");

     std::cout << "->CAMINHO OTIMO: " << "\n";
     for(std::string sa : data.optimalPath)
     {
         std::cout << sa << "__";
     }
     std::cout << "\n";
     //nada de routings nas ruas divididas, nem nas transições de ruas, senão dá erro
     if(currentNode  == 7 || currentNode == 8 || currentNode == 9 || traciVehicle->getLaneId().find(':') != std::string::npos)
     {
        data.oldRoad = traciVehicle->getLaneId();
        data.routingNeeded = true;
     }
     else
         traciVehicle->changeVehicleRoute(data.optimalPath);

    data.initRouting = true;
}

int SocketEmergencyVehicle::coordinateIsInStoppedVehicles(Coord parado)
{
    int result = -1;
    for(Coord pos : data.stoppedCoordinates)
    {
        if(pos.x == parado.x && pos.y == parado.y)
        {
            result = 0;
            break;
        }
    }
    return result;

}

std::string SocketEmergencyVehicle::getRoadIdThroughCoordinates(double x, double y, std::string orientation)
{
    //std::cout<< "ENTREI NESTE\n";
    std::string result;
    for(auto route : data.allRoutes)
        if((result = route.coordinateInFromTheRoute(x, y)).compare("") != 0)
            break;

    if(result.compare("") != 0 && (orientation.compare(std::to_string(M_PI/2)) == 0 || orientation.compare(std::to_string(-M_PI)) == 0 ))
    {
        result += "to";
        //reverter as estradas
        std::vector<std::string> campos =  CommonOps::split(result, "to");

        result = campos[1] + "to";
        result += campos[0];

    }
    return result;
}

void SocketEmergencyVehicle::prepareAllPaths(int parent[], int j)
{
    // Base Case : If j is source
    if (parent[j] == - 1)
        return;
    prepareAllPaths(parent, parent[j]);
    data.allPossibleOptimalPaths += std::to_string(j);
    data.allPossibleOptimalPaths += "-";
}

int SocketEmergencyVehicle::getNextNodeCurrentRoad()
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

int SocketEmergencyVehicle::minDistance(int dist[], bool sptSet[])
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

void SocketEmergencyVehicle::onWSA(WaveServiceAdvertisment* wsa)
{
    if (currentSubscribedServiceId == -1) {
        mac->changeServiceChannel(wsa->getTargetChannel());
        currentSubscribedServiceId = wsa->getPsid();
        if  (currentOfferedServiceId != wsa->getPsid()) {
            stopService();
            startService((Channels::ChannelNumber) wsa->getTargetChannel(), wsa->getPsid(), "Mirrored Traffic Service");
        }
    }
}

void SocketEmergencyVehicle::handleReceivedMessage(std::string bsmString)
{
    BSMRedux* redux = XMLParser::parseXMLBSM(bsmString);

    if(redux->getResponseType() == 4 && redux->getRole() == 8 && redux->getVehicleType() == 2 )
    {
            //std::cout << "AQUI ESTOU E ELE PARADO:" << traciVehicle->getRoadId() <<"\n";
        Coord parado;
        parado.x = redux->getLongitude();
        parado.y = redux->getLatitude();

        //std::cout << "empty :" << stoppedCoordinates.size() << "\n";
                //std::cout << "coordinateIsInStoppedCoordinates :" << coordinateIsInStoppedVehicles(parado) << "\n";
                //este if serve para impedir que se processe o mesmo acidente mais do que uma vez
       /* if(data.stoppedCoordinates.size() == 0 || coordinateIsInStoppedVehicles(parado) == -1)
        {
                    //std::cout << "ha obstaculos e eu estou na estrada " << traciVehicle->getRoadId() << "\n";
                    //atravÃ©s das coordenadas descobrir o id da estrada
            std::string forbiddenRoad = getRoadIdThroughCoordinates(parado.x, parado.y, std::to_string(redux->getHeading()));
                    //adicionar coordenadas ao vetor de gajos parados

            data.stoppedCoordinates.push_back(parado);

                    //std::cout << "existe um gajo parado nas coordenadas: (" << stod(fieldsI[1]) << ", " << stod(fieldsI[2]) << ")\n";

                    //std::cout << "FORBIDDEN ROAD: " << forbiddenRoad ;
                    //std::cout << ", estou na :" << traciVehicle->getRoadId() << "\n";

                    //std::cout << "SIZE :" << traciVehicle->getPlannedRoadIds().size() << "\n";
                    //ver se interfere na rota da ambulancia
            for(std::string route :  traciVehicle->getPlannedRoadIds())
            {
                if(forbiddenRoad.compare(route) == 0)
                {
                            //std::cout << "entrei no if fo forbidden road\n";
                    forbiddenRoad += "to";
                            //mudar o valor dessa estrada na matriz de custos para 0, ou seja, tornÃ¡-la instransponÃ­vel
                    std::vector<std::string> campos = CommonOps::split(forbiddenRoad, "to");
                    data.routesCosts[stoi(campos[0]) - 1][stoi(campos[1]) - 1] = INF;


                           /* for(int i = 0; i < 15 ; i++)
                            {
                                for(int j = 0; j < 15; j++)
                                    std::cout << routesCosts[i][j] << " ";

                                std::cout << "\n";
                            }
                            //correr novamente o algoritmo de routing
                    if(data.routingNeeded == false)
                    {
                        if(traciVehicle->getLaneId().find("_") == std::string::npos)
                        {
                            performOptimalRouting();
                        }
                        else
                        {
                            data.routingNeeded = true;
                        }
                    }
                    break;
                }
            }
        }*/
    }
}

void SocketEmergencyVehicle::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind())
    {
        //Ã© BSM
        case SEND_BEACON_EVT:
        {
            
        }

        }
}

void createAndSendBSMThroughSocket(SocketData sd, std::string xml)
{

    if(!xml.empty())
    {
        std::string csv = XMLParser::transformXMLIntoCSV(xml);

//        std::cout << "ev::csv: \n " << csv << "\n";

  //      std::cout << "ev::xml: \n" << xml << "\n\n";
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

void SocketEmergencyVehicle::handleLaneChanging()
{

    if(data.initLane && traciVehicle->getLaneIndex() == 0)
    {
            // std::cout << "1\n";
        traciVehicle->changeLane(1, 5);
    }
    else if(data.initLane && traciVehicle->getLaneIndex() != 0)
    {
            // std::cout << "2\n";
        data.initLane = false;
    }
    else if((traciVehicle->getPlannedRoadIds().back().compare(traciVehicle->getRoadId()) == 0) && !data.lastRoad)
    {
            // std::cout << "3\n";
        traciVehicle->changeLane(1, 1000000000);
        data.lastRoad = true;
    }

    if(data.wasStoppedCurb && traciVehicle->getLaneIndex() == 0)
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
        data.wasStoppedCurb = false;
    }   
}

bool SocketEmergencyVehicle::checkIfSingleLane()
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

        res = true;

    return res;
}

void SocketEmergencyVehicle::recordStatistics()
{
    if(traciVehicle->getRoadId().compare("29to1") != 0 && traciVehicle->getRoadId().compare("30to5") != 0 && traciVehicle->getRoadId().compare("31to11") != 0 && traciVehicle->getRoadId().compare("32to15") != 0)
    {
        if(mobility->getSpeed() == 0.0 && data.stoppedAt == -1 && data.stoppedAt != 1)
            data.stoppedAt = simTime();
        else if (mobility->getSpeed() != 0.0 && data.stoppedAt != -1)
        {
            std::cout << "elapsed: " << simTime().dbl() << "-" <<  data.stoppedAt.dbl()<<"\n";
            data.totalStoppedTime += (simTime() - data.stoppedAt).dbl();
            data.stoppedAt = -1;
        }

        if(mobility->getSpeed() == 3.0 && data.startedAt == -1)
        {
            data.startedAt = simTime();
        }
        else if(mobility->getSpeed() != 3.0 && data.startedAt != -1)
        {
            data.startedAt = -1;
            data.totalMaxSpeedTime += simTime().dbl() - data.startedAt.dbl();
        }

    }

}

void SocketEmergencyVehicle::handlePositionUpdate(cObject* obj)
{
    BaseWaveApplLayer::handlePositionUpdate(obj);
    BSM* newBSM ;


    if(data.receivedMessage)
    {

        data.receivedMessage = false;
        handleReceivedMessage(data.bsmString);
    }

    recordStatistics();

    if(!initThread)
    {
        thr = std::thread(/*&SocketEmergencyVehicle::*/socketThread, &data/*, std::ref(traciVehicle)*/);
        initThread = true;
        initSocket();
    }

/*    if(!data.initRouting)
    {
        performOptimalRouting();
        data.last_speed = mobility->getSpeed();
    }
    else
    {

        if(data.routingNeeded == true)
        {
            std::string roadX = traciVehicle->getLaneId();
            if( roadX.compare(data.oldRoad) != 0  && isdigit(roadX[0]))
            {
                data.optimalPath.pop_front();
                traciVehicle->changeVehicleRoute(data.optimalPath);
                data.routingNeeded = false;
            }
        }
    }
*/
    if(!checkIfSingleLane())
        handleLaneChanging();

    Coord current;
    current.x = mobility->getCurrentPosition().x;
    current.y = mobility->getCurrentPosition().y;

    traciVehicle->setSpeedMode(7);

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    //std::chrono::duration<float> elapsed = now - previousTime;
    std::chrono::duration<float> mili =   std::chrono::duration_cast<std::chrono::milliseconds>(now - previousTime);
    float elapsedTime = mili.count();

    //std::cout << "mili: " << elapsedTime << "\n";
    if(elapsedTime > (1 / 1000.0))
    {
        //envia mensagem a comunicar a sua posiÃ§Ã£o, velocidade, orientaÃ§Ã£o
        //createAndSendBSM(newBSM->createXML(6, 4, 1, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, properId));
      //  std::cout << "muitas vezes\n";
       previousTime = now;
//    }


    // if(traci->getDistance(data.previousCoords, current, true) > 10)
     //{

        //envia mensagem a comunicar a sua posiÃ§Ã£o, velocidade, orientaÃ§Ã£o
        createAndSendBSMThroughSocket(sd, newBSM->createXML(6, 4, 1, mobility->getCurrentPosition().y * 100000, mobility->getCurrentPosition().x * 100000, mobility->getSpeed() * 100000, mobility->getAngleRad() * 100000, 0, 0, data.properId));
    }
}

void SocketEmergencyVehicle::onWSM(WaveShortMessage* wsm){}

#ifndef SocketRSUSimulatorVehicle_H
#define SocketRSUSimulatorVehicle_H


#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "XMLParser.h"
#include "veins/modules/application/traci/GeometricOperations.h"
#include <thread>

class ThreadData
{
    public:
        bool isOver;
        bool receivedMessage;
        std::string bsmString;
};

class SocketData
{
    public:
        WSADATA WSAData;
        SOCKET server;
        SOCKADDR_IN addr;
};

class SlowVehicleData
{
    protected:
        Coord pos;
        double heading;
        int acc;
    public:
        SlowVehicleData(){}
        SlowVehicleData(double xx, double yy, double head, int ac)
        {
            pos.x = xx;
            pos.y = yy;
            heading = head;
            acc = ac;
        }

    public:
        double getX(){return pos.x;}
        double getY(){return pos.y;}
        double getHeading(){return heading;}
        int getNumber(){return acc;}
        void increment(){acc++;}

};

class SlowVehicle
{
    protected:
        Coord pos;
        double heading;
        std::string id;
    public:
        SlowVehicle(){}
        SlowVehicle(double xx, double yy, double head, std::string i)
        {
            pos.x = xx;
            pos.y = yy;
            heading = head;
            id = i;
        }

    public:
        std::string vehiclesAreInTheSameRoad(Coord veh, double head)
        {
            if(GeometricOperations::distance(pos, veh) < 250 && GeometricOperations::isNearlyEqual(heading, head) == 0)
                return id;
            return "";
        }

        double getX(){return pos.x;}
        double getY(){return pos.y;}
        double getHeading(){return heading;}
        std::string getId(){return id;}

};


class SocketRSUSimulatorVehicle : public BaseWaveApplLayer
{
    public:
        virtual void initialize(int stage);
    protected:
        void changeTrafficLightLights(float orientation);
        void trafficLightLogic(std::string bsmString);
        void stopSignLogic(std::string bsmString);
        void createAndSendWaveShortMessage(int idRSU, std::string wsmData);
        bool ambulanceIsInRSUArea(Coord posAmbulance);
        std::string changeTrafficLightColours(int side, std::string newProgram);
        bool vehicleIsInRSUArea(Coord posVeh, int radius);
        void initializeRSU(double rsuX, double rsuY, double cenX, double cenY, std::string type, bool tlI, std::string road, int id);
        void fillRSUsId();
        void handlePossibleStoppedCar(BSMRedux* bsm);
        void handleReceivedMessage(std::string bsmString);
        std::string createIdForPossibleStoppedCar(float lon, float lan);
        void handlePossibleTrafficSituation(BSMRedux* redux);
        void finish();
        void handlePositionUpdate(cObject* obj);
        void initSocket();
        void onBSM(BasicSafetyMessage *bsm);

    protected:
        TraCICommandInterface::Trafficlight tl;
        bool initTL;
        bool initProgram;
        bool initTraci;
        std::string rsuType;
        Coord posRSU;
        int meuId;
        int initialPhaseIndex;
        int vehicleEnteringIntersectionId;
        std::string rsuRoad;
        bool sentCancel;
        //ambulance data
        int idAmbulance;
        Coord posAmbulance;
        double speedAmbulance;
        float time2arriveAmbulance;
        float time2arriveAmbulanceTL;
        float orientationAmbulance;
        Coord rsuCenter;
        std::list<std::string> vehiclesInRSUArea;
        std::list<SlowVehicleData*> svd;
        //traci
        Veins::TraCIScenarioManager* manager;
        TraCICommandInterface* traci;
        std::map<std::string, int> vehiclesAndStoppedFrequency;
        std::string properId;
        std::list<std::string> stoppedVehicles;
        std::list<std::string> bannedIds;
        std::map<std::string, SlowVehicle*> slowVehicles;

        ThreadData data;
        bool initThread;
        std::thread thr;
        SocketData sd;

        std::string lastString;

        cMessage* socketMessageTrigger;
};



#endif

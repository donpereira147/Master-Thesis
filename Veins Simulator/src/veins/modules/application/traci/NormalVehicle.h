#ifndef NormalVehicle_H
#define NormalVehicle_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/application/traci/RouteSpecs.h"
#include <chrono>

class NormalVehicle : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
        virtual void finish();
    protected:
        simtime_t lastDroveAt;
        bool sentMessage;
        int currentSubscribedServiceId;
        bool stopped;
        bool acknTLChange;
        Coord coordIni;
        bool firstTime;
        bool acknLI;
        std::string roadTLChange;
        std::string currentRoad;
        double previousSpeed;
        double previousSpeedStopped;
        bool waitingForEVMessage;
        bool wasStoppedCurb;
        bool initLane;
        bool lastRoad;
        int** routesCosts;
        bool initRouting;
        std::string allPossibleOptimalPaths;
        std::vector<RouteSpecs> allRoutes;
        std::vector<Coord> stoppedCoordinates;
        std::string properId;
        std::chrono::time_point<std::chrono::system_clock> previousTime;
        bool speedReduction;
        int previousLane;
        bool fewTests;
    protected:
        Coord previousCoords;
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void onWSA(WaveServiceAdvertisment* wsa);
        virtual void handleSelfMsg(cMessage* msg);
        virtual void handlePositionUpdate(cObject* obj);
        void sendUpdateMessage(std::string flag);
        std::list<std::string> getOptimalRouteThroughDijkstra(int origin, int end);
        void prepareRoutingOptimization();
        void prepareAllPaths(int parent[], int j);
        void performOptimalRouting();
        void handleLaneChanging();
        std::string getRoadIdThroughCoordinates(double x, double y, std::string orientation);
        int getNextNodeCurrentRoad();
        int coordinateIsInStoppedVehicles(Coord parado);
        void createAndSendBSM(std::string xml);
        void onBSM(BasicSafetyMessage *bsm);
        int minDistance(int dist[], bool sptSet[]);
        bool checkIfSingleLane();
};

#endif

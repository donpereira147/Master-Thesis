#ifndef JustKerbsVehicle_H
#define JustKerbsVehicle_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/application/traci/RouteSpecs.h"
#include <chrono>

class JustKerbsVehicle : public BaseWaveApplLayer {
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
        void handleLaneChanging();
        void onBSM(BasicSafetyMessage *bsm);
        bool checkIfSingleLane();
};

#endif

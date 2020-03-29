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

#ifndef JustSirenEmergencyVehicle_H
#define JustSirenEmergencyVehicle_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "veins/modules/application/traci/RouteSpecs.h"
#include <chrono>

class JustSirenEmergencyVehicle : public BaseWaveApplLayer {
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
		std::list<std::string> optimalPath;
		bool routingNeeded;
		std::string oldRoad;
		std::chrono::time_point<std::chrono::system_clock> previousTime;
		TraCIMobility::Statistics statistics;
		simtime_t stoppedAt, startedAt, lastUpdate;
		int totalStoppedTime, totalMaxSpeedTime;
		double last_speed;
		bool resultsArePrinted;

	protected:
		Coord previousCoords;
        virtual void onWSM(WaveShortMessage* wsm);
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
		void recordStatistics();
};

#endif
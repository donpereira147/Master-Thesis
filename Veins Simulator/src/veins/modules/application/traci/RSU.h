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

#ifndef RSU_H
#define RSU_H

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "XMLParser.h"
#include "veins/modules/application/traci/GeometricOperations.h"

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


class RSU : public BaseWaveApplLayer 
{
	public:
		virtual void initialize(int stage);
	protected:
		virtual void onWSM(WaveShortMessage* wsm);
		virtual void onWSA(WaveServiceAdvertisment* wsa);
		void changeTrafficLightLights(float orientation);
		void trafficLightLogic(BasicSafetyMessage* wsm);
		void stopSignLogic(BasicSafetyMessage* wsm);
		void createAndSendWaveShortMessage(int idRSU, std::string wsmData);
		bool ambulanceIsInRSUArea(Coord posAmbulance);
		std::string changeTrafficLightColours(int side, std::string newProgram);
		bool vehicleIsInRSUArea(Coord posVeh, int radius);
		void initializeRSU(double rsuX, double rsuY, double cenX, double cenY, std::string type, bool tlI, std::string road, int id);
		void fillRSUsId();
		void handlePossibleStoppedCar(BSMRedux* bsm);
		void createAndSendBSM(std::string xml);
		virtual void onBSM(BasicSafetyMessage* bsm);
		std::string createIdForPossibleStoppedCar(float lon, float lan);
		void handlePossibleTrafficSituation(BSMRedux* redux);
		void initALotOfRSUs( std::string nodeName);
		void initFewRSUs( std::string nodeName);
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

};



#endif

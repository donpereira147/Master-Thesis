#include <stdlib.h>
#include <math.h>
#include <string>

class Accuracy
{	
	protected:
		int semiMajorAxisAccuracy;
		int semiMinorAxisAccuracy;
		int semiMajorAxisOrientation;

	public:
		Accuracy(){}
		Accuracy(int sMAA, int smAA, int sMao) 
		{
			semiMajorAxisAccuracy = sMAA;
			semiMinorAxisAccuracy = smAA;
			semiMajorAxisOrientation = sMao;	
		}

		int getSemiMajorAxisAccuracy(){return semiMajorAxisAccuracy;}
		int getSemiMinorAxisAccuracy(){return semiMinorAxisAccuracy;}
		int getSemiMajorAxisOrientation(){return semiMajorAxisOrientation;}

		void setSemiMajorAxisAccuracy(int x){semiMajorAxisOrientation = x;}
		void setSemiMinorAxisAccuracy(int x){semiMinorAxisAccuracy = x;}
		void setSemiMajorAxisOrientation(int x){semiMajorAxisOrientation = x;}
};

class Acceleration4Way
{
	protected:	
		int longi;
	    int lat;
	    int vert;
	    int yaw;

	public:
		Acceleration4Way(){}
		Acceleration4Way(int lon, int la, int ver, int ya)
		{
			longi = lon;
			lat = la;
			vert = ver;
			yaw = ya;
		}

		int getLongitude(){return longi;}
		int getLatitude(){return lat;}
		int getVerticalAcceleration(){return vert;}
		int getYawRate(){return yaw;}

		void setLongitude(int x){longi = x;}
		void setLatitude(int x){lat = x;}
		void setVerticalAcceleration(int x){vert = x;}
		void setYawRate(int x){yaw = x;}
};

class BrakeSystemStatus
{
	protected:
		std::string wheelBrake;
		int traction; //{0,1,2,3}
	    int abs; //{0,1,2,3}
	    int scs; //{0,1,2,3}
	    int brakeBoost; //{0,1,2}
	    int auxBrakes; //{0,1,2,3}

	public:
		BrakeSystemStatus(){}
		BrakeSystemStatus(std::string w, int t, int a, int s, int b, int aux)
		{
			wheelBrake = w;
			traction = t;
			abs = a;
			scs = s;
			brakeBoost = b;
			auxBrakes = aux;
		}
		std::string getWheelBrake(){return wheelBrake;}
		int getTraction(){return traction;}
		int getABS(){return abs;}
		int getStabilityControlStatus(){return scs;}
		int getBrakeBoost(){return brakeBoost;}
		int getAuxBrakes(){return auxBrakes;}

		void setWheelBrake(std::string x){wheelBrake = x;}
		void setTraction(int x){traction = x;}
		void setABS(int x){abs= x;}
		void setStabilityControlStatus(int x){ scs = x;}
		void setBrakeBoost(int x){brakeBoost = x;}
		void setAuxBrakes(int x){auxBrakes = x;}
};

class VehicleSize
{
	protected:
		int width;
		int length;

	public:
		VehicleSize(){}
		VehicleSize(int w, int l)
		{
			width = w;
			length = l;
		}

		int getWidth(){return width;}
		int getLength(){return length;}

		void setWidth(int x){width = x;}
		void setLength(int x){length = x;}
};
class VehicleSafetyExtensions
{
	protected:
		int speedAlter, emergencyVehicleTime;

	public:
		VehicleSafetyExtensions(){}
		VehicleSafetyExtensions(int s, int e)
		{
			speedAlter = s;
			emergencyVehicleTime = e;
		}
		int getSpeedAlter(){return speedAlter;}
		int getEmergencyVehicleTime(){return emergencyVehicleTime;}
		void setSpeedAlter(int s){speedAlter = s;}
		void setEmergencyVehicleTime(int e){emergencyVehicleTime = e;}
};

class SpecialVehicleExt
{
	protected:
		int sspRights;
		int sirenInUse;
		int lightsUse;
		int multi;
		int responseType;
	public:
		SpecialVehicleExt(){}
		int getSSPIndex(){return sspRights;}
		int getSirensInUse(){return sirenInUse;}
		int getLightsUse(){return lightsUse;}
		int getMultiVehicleResponse(){return multi;}
		int getResponseType(){return responseType;}

		void setSSPIndex(int x){sspRights = x;}
		void setSirensInUse(int x){sirenInUse = x;}
		void setLightsUse(int x){lightsUse = x;}
		void setMultiVehicleResponse(int x){multi = x;}
		void setResponseType(int x){responseType = x;}
};

class SupplementalVehicleExtensions
{
	protected:
		int classification;
		int role;
		int vehicleType;
	public:
		SupplementalVehicleExtensions(){}
		SupplementalVehicleExtensions(int c, int r, int v){classification = c; role = r; vehicleType = v;}
		int getClassification(){return classification;}
		int getRole(){return role;}
		int getVehicleType(){return vehicleType;}

		void setClassification(int x){classification = x;}
		void setRole(int x){role = x;}
		void setVehicleType(int x){vehicleType = x;}
};

class BSMRedux
{
	protected:
		int role, vehicleType, responseType, speedAlter;
		float latitude, longitude, speed, heading, timeEV;
		std::string temporaryId;

	public:
		BSMRedux();
		BSMRedux(float lat, float lon, float spe, float hea, int rol, int veh, int res, int sa, float ev, std::string ti)
		{
			latitude = lat;
			longitude = lon;
			speed = spe;
			heading = hea;
			role = rol;
			vehicleType = veh;
			responseType = res;
			speedAlter = sa;
			timeEV = ev;
			temporaryId = ti;
		}

		float getLatitude(){return latitude;}
		float getLongitude(){return longitude;}
		float getSpeed(){return speed;}
		float getHeading(){return heading;}
		float getTimeEmergencyVehicle(){return timeEV;}
		int getRole(){return role;}
		int getVehicleType(){return vehicleType;}
		int getResponseType(){return responseType;}
		int getSpeedAlter(){return speedAlter;}
		std::string getTemporaryId(){return temporaryId;}
};

class BSM
{
	protected:
		int msgCount;
		std::string temporaryId;
		int secMark;
		int latitude;
	    int longitude;
	    int elevation;
	    int transmissionState;// [0, 7]
	    int speed;
	    int heading;
	    int steeringWheelAngle;
	    //estruturas transformadas em tipos primitivos, para que se saiba facilmente o seu nome
	    //accuracy
	    Accuracy accuracy;
	    //accelerationSet4Way
	    Acceleration4Way acceleration;
	    //brake system status
	    BrakeSystemStatus brakeStatus;
	    VehicleSize vs;
	    bool specialVehicleExt;
	    bool supplementalVehicle;
	    bool vehicleSafetyExtension;
	    SpecialVehicleExt sve;
	    SupplementalVehicleExtensions sup;
	    VehicleSafetyExtensions vse;

	public:
		BSM();
		BSM(bool sve, bool sup, bool vse);
		int getMsgCount();
		std::string getTemporaryId();
		int getSecMark();
		int getLatitude();
		int getLongitude();
		int getElevation();
		int getTransmissionState();
		int getSpeed();
		int getHeading();
		int getSteeringWheelAngle();
		Accuracy getAccuracy();
		Acceleration4Way getAcceleration4Way();
		BrakeSystemStatus getBrakeSystemStatus();
		VehicleSize getVehicleSize();
		SpecialVehicleExt getSpecialVehicleExt();
		SupplementalVehicleExtensions getSupplementalVehicleExtensions();
		VehicleSafetyExtensions getVehicleSafetyExtensions();

		void setMsgCount(int msgCount);
		void setTemporaryId(std::string temporaryId);
		void setSecMark(int secMark);
		void setLatitude(int latitude);
		void setLongitude(int longitude);
		void setElevation(int elevation);
		void setTransmissionState(int transmissionState);
		void setSpeed(int speed);
		void setHeading(int heading);
		void setSteeringWheelAngle(int steeringWheelAngle);
		void setAccuracy(Accuracy accuracy);
		void setAcceleration4Way(Acceleration4Way acceleration);
		void setBrakeSystemStatus(BrakeSystemStatus bss);
		void setVehicleSize(VehicleSize vs);
		void setSpecialVehicleExt(SpecialVehicleExt sve);
		void setSupplementalVehicleExtensions(SupplementalVehicleExtensions sup);
		void setVehicleSafetyExtensions(VehicleSafetyExtensions vse);
		void setSpecialVehicleExtResponseType(int x);
		void setSupplementalVehicleExtensionsClassification(int x);
		void setSupplementalVehicleExtensionsRole(int x);
		void setSupplementalVehicleExtensionsVehicleType(int x);
		void setSpeedAlterVehicleSafetyExtension(int x);
		void setEmergencyVehicleTimeVehicleSafetyExtension(int x);
		int getEmergencyVehicleTimeVehicleSafetyExtension();
		int getSpeedAlterVehicleSafetyExtension();

		void partlyInitializeBSM();
		std::string xmlizeBSMs();
		std::string createXML(int role, int vehicleType, int responseType, int latitude, int longitude, int speed, int heading, int speedAlter, int timeEV, std::string temporaryId);
};

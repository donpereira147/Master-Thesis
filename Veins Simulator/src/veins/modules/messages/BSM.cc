#include "BSM.h"
#include <stdlib.h>
#include <string>

BSM::BSM(bool sv, bool su, bool vse)
{
	specialVehicleExt = sv;
	supplementalVehicle = su;
	vehicleSafetyExtension = vse; 
	msgCount = 127;
	temporaryId = "nada";
	secMark = 65535;
	elevation = 61439;

	accuracy.setSemiMajorAxisAccuracy(255);
	accuracy.setSemiMinorAxisAccuracy(255);
	accuracy.setSemiMajorAxisOrientation(65535);
	transmissionState = 7;
	heading = 127;

	acceleration.setLongitude(2001);
	acceleration.setLatitude(2001);
	acceleration.setVerticalAcceleration(127);
	acceleration.setYawRate(32767);


	brakeStatus.setWheelBrake("0");
	brakeStatus.setTraction(0);
	brakeStatus.setABS(0);
	brakeStatus.setStabilityControlStatus(0);
	brakeStatus.setBrakeBoost(0);
	brakeStatus.setAuxBrakes(0);


	vs.setWidth(1023);
	vs.setLength(4095);

	if(sv)
	{
		sve.setSSPIndex(31);
		sve.setSirensInUse(0);
		sve.setLightsUse(0);
		sve.setMultiVehicleResponse(0);
	}
}

int BSM::getMsgCount()
{
	return msgCount;
}

std::string BSM::getTemporaryId()
{
	return temporaryId;
}

int BSM::getSecMark()
{
	return secMark;
}

int BSM::getLatitude()
{
	return latitude;
}

int BSM::getLongitude()
{
	return longitude;
}

int BSM::getElevation()
{
	return elevation;
}

int BSM::getTransmissionState()
{
	return transmissionState;
}

int BSM::getSpeed()
{
	return speed;
}
int BSM::getHeading()
{
	return heading;
}

int BSM::getSteeringWheelAngle()
{
	return steeringWheelAngle;
}

Accuracy BSM::getAccuracy()
{
	return accuracy;
}

Acceleration4Way BSM::getAcceleration4Way()
{
	return acceleration;
}

BrakeSystemStatus BSM::getBrakeSystemStatus()
{
	return brakeStatus;
}

VehicleSize BSM::getVehicleSize()
{
	return vs;
}

void BSM::setMsgCount(int msgCount)
{
	this->msgCount = msgCount;
}

void BSM::setTemporaryId(std::string temporaryId)
{
	this->temporaryId = temporaryId;
}

void BSM::setSecMark(int secMark)
{
	this->secMark = secMark;
}

void BSM::setLatitude(int latitude)
{
	this->latitude = latitude;
}

void BSM::setLongitude(int longitude)
{
	this->longitude = longitude;
}
		
void BSM::setElevation(int elevation)
{
	this->elevation = elevation;
}

void BSM::setTransmissionState(int transmissionState)
{
	this->transmissionState = transmissionState;
}

void BSM::setSpeed(int speed)
{
	this->speed = speed;
}

void BSM::setHeading(int heading)
{
	this->heading = heading;
}

void BSM::setSteeringWheelAngle(int steeringWheelAngle)
{
	this->steeringWheelAngle = steeringWheelAngle;
}

void BSM::setAccuracy(Accuracy accuracy)
{
	this->accuracy = accuracy;
}

void BSM::setAcceleration4Way(Acceleration4Way acceleration)
{
	this->acceleration = acceleration;
}		

void BSM::setBrakeSystemStatus(BrakeSystemStatus bss)
{
	this->brakeStatus = bss;
}

void BSM::setVehicleSize(VehicleSize vs)
{
	this->vs = vs;
}

void BSM::setSpecialVehicleExt(SpecialVehicleExt vs)
{
	this->sve = vs;
}

void BSM::setSupplementalVehicleExtensions(SupplementalVehicleExtensions vs)
{
	this->sup = vs;
}

SpecialVehicleExt BSM::getSpecialVehicleExt()
{
	return sve;
}

SupplementalVehicleExtensions BSM::getSupplementalVehicleExtensions()
{
	return sup;
}

void BSM::setSpecialVehicleExtResponseType(int x)
{
    this->sve.setResponseType(x);
}

void BSM::setSupplementalVehicleExtensionsClassification(int x)
{
    this->sup.setClassification(x);
}

void BSM::setSupplementalVehicleExtensionsRole(int x)
{
    this->sup.setRole(x);
}

void BSM::setSupplementalVehicleExtensionsVehicleType(int x)
{
    this->sup.setVehicleType(x);
}

void BSM::setSpeedAlterVehicleSafetyExtension(int x)
{
	this->vse.setSpeedAlter(x);
}

void BSM::setEmergencyVehicleTimeVehicleSafetyExtension(int x)
{
	this->vse.setEmergencyVehicleTime(x);
}

int BSM::getSpeedAlterVehicleSafetyExtension()
{
	return this->vse.getSpeedAlter();
}

int BSM::getEmergencyVehicleTimeVehicleSafetyExtension()
{
	return this->vse.getEmergencyVehicleTime();
}

std::string BSM::xmlizeBSMs()
{
	std::string xml = "<bsm>\n\t<coreData>\n";
	
	xml += "\t\t<field name=\"msgCount\" value= \"";
	xml += std::to_string(msgCount);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"temporaryId\" value= \"";
	xml += temporaryId;
	xml += "\"/>\n";
	xml += "\t\t<field name=\"secMark\" value= \"";
	xml += std::to_string(secMark);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"latitude\" value= \"";
	xml += std::to_string(latitude);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"longitude\" value= \"";
	xml += std::to_string(longitude);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"transmissionState\" value= \"";
	xml += std::to_string(transmissionState);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"speed\" value= \"";
	xml += std::to_string(speed);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"heading\" value= \"";
	xml += std::to_string(heading);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"steeringWheelAngle\" value= \"";
	xml += std::to_string(steeringWheelAngle);
	xml += "\"/>\n";
	xml += "\t\t<field name=\"elevation\" value= \"";
	xml += std::to_string(elevation);
	xml += "\"/>\n";

	xml += "\t\t<sequence name= \"Accuracy\">\n" ;
	xml += "\t\t\t<field name=\"semiMajorAxisAccuracy\" value= \"";
	xml += std::to_string(accuracy.getSemiMajorAxisAccuracy());
	xml += "\"/>\n";
	xml += "\t\t\t<field name=\"semiMinorAxisAccuracy\" value= \"";
	xml += std::to_string(accuracy.getSemiMinorAxisAccuracy());
	xml += "\"/>\n";
	xml += "\t\t\t<field name=\"semiMajorAxisOrientation\" value= \"";
	xml += std::to_string(accuracy.getSemiMajorAxisOrientation());
	xml += "\"/>\n\t\t</sequence>\n";

	xml += "\t\t<sequence name= \"Acceleration4Way\">\n" ;
	xml += "\t\t\t<field name=\"longitude\" value= \"";
	xml += std::to_string(acceleration.getLongitude());
	xml += "\"/>\n\t\t\t<field name=\"latitude\" value= \"";
	xml += std::to_string(acceleration.getLatitude());
	xml += "\"/>\n\t\t\t<field name=\"yawRate\" value= \"";
	xml += std::to_string(acceleration.getYawRate());
	xml += "\"/>\n\t\t\t<field name=\"verticalAcceleration\" value= \"";
	xml += std::to_string(acceleration.getVerticalAcceleration());
	xml += "\"/>\n\t\t</sequence>\n";

	xml += "\t\t<sequence name= \"BrakeSystemStatus\">\n" ;
	xml += "\t\t\t<field name=\"wheelBrake\" value= \"";
	xml += brakeStatus.getWheelBrake() ;
	xml += "\"/>\n\t\t\t<field name=\"traction\" value= \"";
	xml += std::to_string(brakeStatus.getTraction());
	xml += "\"/>\n\t\t\t<field name=\"abs\" value= \"";
	xml += std::to_string(brakeStatus.getABS());
	xml += "\"/>\n\t\t\t<field name=\"scs\" value= \"";
	xml += std::to_string(brakeStatus.getStabilityControlStatus());
	xml += "\"/>\n\t\t\t<field name=\"brakeBoost\" value= \"";
	xml += std::to_string(brakeStatus.getBrakeBoost());
	xml += "\"/>\n\t\t\t<field name=\"auxBrakes\" value= \"";
	xml += std::to_string(brakeStatus.getAuxBrakes());
	xml += "\"/>\n\t\t</sequence>\n";

	xml += "\t\t<sequence name= \"VehicleSize\">\n" ;
	xml += "\t\t\t<field name=\"width\" value= \"";
	xml += std::to_string(vs.getWidth());
	xml += "\"/>\n\t\t\t<field name=\"length\" value= \"";
	xml += std::to_string(vs.getLength());
	xml += "\"/>\n\t\t</sequence>\n";

	xml += "\t</coreData>\n";
	if(specialVehicleExt || supplementalVehicle)
	{
	    xml += "\t<partII>\n";
	    if(specialVehicleExt)
        {
	        xml += "\t\t<SpecialVehicleExt>\n";
            xml += "\t\t\t<sequence name= \"emergencyDetails\">\n" ;
            xml += "\t\t\t\t<field name=\"sspRights\" value= \"";
            xml += std::to_string(sve.getSSPIndex());
            xml += "\"/>\n\t\t\t\t<field name=\"sirenInUse\" value= \"";
            xml += std::to_string(sve.getSirensInUse());
            xml += "\"/>\n\t\t\t\t<field name=\"lightsUse\" value= \"";
            xml += std::to_string(sve.getLightsUse());
            xml += "\"/>\n\t\t\t\t<field name=\"multiVehicleResponse\" value= \"";
            xml += std::to_string(sve.getMultiVehicleResponse());
            xml += "\"/>\n\t\t\t\t<field name=\"responseType\" value= \"";
            xml += std::to_string(sve.getResponseType());
            xml += "\"/>\n\t\t\t</sequence>\n";
            xml += "\t\t</SpecialVehicleExt>\n";
        }
	    if(supplementalVehicle)
	    {
	        xml += "\t\t<SupplementalVehicle>\n";
	        xml += "\t\t\t<sequence name= \"classDetails\">\n" ;
	        xml += "\t\t\t\t<field name=\"role\" value= \"";
	        xml += std::to_string(sup.getRole());
	        xml += "\"/>\n\t\t\t\t<field name=\"vehicleType\" value= \"";
	        xml += std::to_string(sup.getVehicleType());
	        xml += "\"/>\n\t\t\t</sequence>\n";
	        xml += "\t\t</SupplementalVehicle>\n";
	    }
	    /*if(vehicleSafetyExtension)
	    {
	        xml += "\t\t<vehicleSafetyExt>\n";
	        xml += "\t\t\t<sequence name= \"safetyDetails\">\n" ;
	        xml += "\t\t\t\t<field name=\"speedAlter\" value= \"";
	        xml += std::to_string(vse.getSpeedAlter());
	        xml += "\"/>\n\t\t\t\t<field name=\"timeEmergencyVehicle\" value= \"";
	        xml += std::to_string(vse.getEmergencyVehicleTime());
	        xml += "\"/>\n\t\t\t</sequence>\n";
	        xml += "\t\t</vehicleSafetyExt>\n";
	    }*/

	    xml += "\t</partII>\n";
	}
	xml +="</bsm>";

	return xml;
}


std::string BSM::createXML(int role, int vehicleType, int responseType, int latitude, int longitude, int speed, int heading, int speedAlter, int timeEV, std::string id)
{
	BSM* bsm = new BSM(true, true, true);

	bsm->setLongitude(longitude);
	bsm->setLatitude(latitude);
	bsm->setHeading(heading);
	bsm->setSpeed(speed);
	bsm->setSpecialVehicleExtResponseType(responseType);

	bsm->setSupplementalVehicleExtensionsClassification(2);
	bsm->setSupplementalVehicleExtensionsRole(role);
	bsm->setSupplementalVehicleExtensionsVehicleType(vehicleType);

	bsm->setSpeedAlterVehicleSafetyExtension(speedAlter);
	bsm->setElevation(timeEV);

	bsm->setTemporaryId(id);

	return bsm->xmlizeBSMs();
}

#include "veins/modules/messages/BSM.h"
#include <stdlib.h>
#include "veins/modules/application/traci/pugixml.hpp"

class XMLParser
{
	public:
		static BSMRedux* parseXMLBSM(std::string xml)
		{
			BSMRedux* redux;
			int role, vehicleType, responseType, latitude, longitude, speed, heading, speedAlter, timeEV;
			std::string temporaryId;
			role = vehicleType = responseType =-1;
			std::string name;
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load(xml.c_str());

			if(result)
			{
				pugi::xml_node bsmNode = doc.child("bsm");

				if(bsmNode.child("partII").child("SupplementalVehicle"))
				{
					pugi::xml_node classDetails = bsmNode.child("partII").child("SupplementalVehicle").child("sequence");

					for(pugi::xml_node field :  classDetails )
					{
						name = field.attribute("name").as_string();

						if(name.compare("role") == 0)
							role = field.attribute("value").as_int();
						else if(name.compare("vehicleType") == 0)
							vehicleType = field.attribute("value").as_int();
					}
				}
				if(bsmNode.child("partII").child("SpecialVehicleExt"))
				{
					pugi::xml_node emergencyDetails = bsmNode.child("partII").child("SpecialVehicleExt").child("sequence");

					for(pugi::xml_node field :  emergencyDetails )
					{
						name = field.attribute("name").as_string();

						if(name.compare("responseType") == 0)
						{
							responseType = field.attribute("value").as_int();
						}

					}
				}

				/*if(bsmNode.child("partII").child("vehicleSafetyExt"))
				{
					pugi::xml_node emergencyDetails = bsmNode.child("partII").child("vehicleSafetyExt").child("sequence");

					for(pugi::xml_node field :  emergencyDetails )
					{
						name = field.attribute("name").as_string();

						if(name.compare("speedAlter") == 0)
						{
							speedAlter = field.attribute("value").as_int();
						}
						else if(name.compare("timeEmergencyVehicle") == 0)
						{
							timeEV = field.attribute("value").as_int();
						}
					}
				}*/

				for(pugi::xml_node field: doc.child("bsm").child("coreData").children("field"))
				{
					name = field.attribute("name").as_string();

					if(name.compare("latitude") == 0)
						latitude =  field.attribute("value").as_int();
					else if(name.compare("longitude") == 0)
						longitude =  field.attribute("value").as_int();
					else if(name.compare("speed") == 0)
						speed =  field.attribute("value").as_int();
					else if(name.compare("heading") == 0)
						heading =  field.attribute("value").as_int();
					else if(name.compare("temporaryId") == 0)
						temporaryId = field.attribute("value").as_string();
					else if(name.compare("elevation") == 0)
						timeEV = field.attribute("value").as_int();
				}

				redux = new BSMRedux(latitude / 100000.0, longitude / 100000.0, speed / 100000.0, heading / 100000.0, role, vehicleType, responseType, speedAlter, timeEV /100000.0, temporaryId);
			}
			else
				redux = new BSMRedux(-1, -1, -1, -1, -1, -1, -1, -1, -1, "");


			return redux;

		}

		static std::string addCommaToTheEnd(std::string value)
		{
		    return value + ",";
		}


		static std::string transformXMLIntoCSV(std::string xml)
		{
		    std::string finalCSV = "";
		    std::string name;

		    pugi::xml_document doc;
		    pugi::xml_parse_result result = doc.load(xml.c_str());


		    if(result)
		    {
		        pugi::xml_node bsmNode = doc.child("bsm");


		        for(pugi::xml_node field: doc.child("bsm").child("coreData").children("field"))
		        {
		            name = field.attribute("name").as_string();


		            if(name.compare("msgCount") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("temporaryId") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("secMark") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("latitude") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("longitude") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("transmissionState") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("speed") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("heading") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("steeringWheelAngle") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            else if(name.compare("elevation") == 0)
		                finalCSV += addCommaToTheEnd(field.attribute("value").as_string());


		        }

		        finalCSV += addCommaToTheEnd("842608177");
		        finalCSV += addCommaToTheEnd("255");
		        finalCSV += addCommaToTheEnd("65535");


		        finalCSV += addCommaToTheEnd("2001");
		        finalCSV += addCommaToTheEnd("2001");
		        finalCSV += addCommaToTheEnd("32767");
		        finalCSV += addCommaToTheEnd("127");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");

		        finalCSV += addCommaToTheEnd("1023");
		        finalCSV += addCommaToTheEnd("4095");


		        finalCSV += addCommaToTheEnd("31");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");
		        finalCSV += addCommaToTheEnd("0");


		        if(bsmNode.child("partII").child("SpecialVehicleExt"))
		        {
		            pugi::xml_node emergencyDetails = bsmNode.child("partII").child("SpecialVehicleExt").child("sequence");

		            for(pugi::xml_node field :  emergencyDetails )
		            {
		                name = field.attribute("name").as_string();

		                if(name.compare("responseType") == 0)
		                {
		                    finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		                }

		            }
		        }

		        if(bsmNode.child("partII").child("SupplementalVehicle"))
		        {
		            pugi::xml_node classDetails = bsmNode.child("partII").child("SupplementalVehicle").child("sequence");

		            for(pugi::xml_node field :  classDetails )
		            {
		                name = field.attribute("name").as_string();

		                if(name.compare("role") == 0)
		                    finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		                else if(name.compare("vehicleType") == 0)
		                    finalCSV += addCommaToTheEnd(field.attribute("value").as_string());
		            }
		        }

		    }

		    return finalCSV;
		}


		static std::string transformFieldsIntoCSV(double latitude, double longitude, double speed, double heading, int responseType, int role, int vehicleType)
		{
		    std::string finalCSV = "";
		    std::string name;

		    finalCSV += addCommaToTheEnd("127");
		    finalCSV += addCommaToTheEnd("e1");
		    finalCSV += addCommaToTheEnd("65535");
		    finalCSV += addCommaToTheEnd(std::to_string(latitude));
		    finalCSV += addCommaToTheEnd(std::to_string(longitude));
		    finalCSV += addCommaToTheEnd("7");
		    finalCSV += addCommaToTheEnd(std::to_string(speed));
		    finalCSV += addCommaToTheEnd(std::to_string(heading));
		    finalCSV += addCommaToTheEnd("1030057313");
		    finalCSV += addCommaToTheEnd("100");


		    finalCSV += addCommaToTheEnd("842608177");
		    finalCSV += addCommaToTheEnd("255");
		    finalCSV += addCommaToTheEnd("65535");


		    finalCSV += addCommaToTheEnd("2001");
		    finalCSV += addCommaToTheEnd("2001");
		    finalCSV += addCommaToTheEnd("32767");
		    finalCSV += addCommaToTheEnd("127");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");

		    finalCSV += addCommaToTheEnd("1023");
		    finalCSV += addCommaToTheEnd("4095");


		    finalCSV += addCommaToTheEnd("31");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");
		    finalCSV += addCommaToTheEnd("0");

		    finalCSV += addCommaToTheEnd(std::to_string(responseType));

		    finalCSV += addCommaToTheEnd(std::to_string(role));
		    finalCSV += addCommaToTheEnd(std::to_string(vehicleType));

		    return finalCSV;
		}
};

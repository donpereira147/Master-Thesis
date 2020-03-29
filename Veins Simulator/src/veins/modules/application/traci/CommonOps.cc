#include "CommonOps.h"
#include <iostream>

std::string CommonOps::createShortIdForSelf(std::string flag, std::string hostName)
{
    std::string idFinal = flag;
    hostName += "[";
    std::vector<std::string> campos = split(hostName, "[");
    std::string number = split(campos[1], "]")[0];
    return idFinal + number;
}

std::vector<std::string> CommonOps::split(std::string original, std::string delimiter)
{
    std::vector<std::string> campos;
    size_t pos = 0;
    std::string campo;
    while((pos = original.find(delimiter)) != std::string::npos)
    {
        campo = original.substr(0, pos);
        campos.push_back(campo);
        original.erase(0, pos + delimiter.length());
    }
    return campos;
}

bool canISwitchLanes(bool fewTests, std::string road)
{
    if(!fewTests)
        return true;

    if(road.compare("6to7") == 0 || road.compare("7to8") == 0 || road.compare("8to7") == 0
        || road.compare("7to6") == 0  || road.compare("72to78") == 0  || road.compare("78to72") == 0
        || road.compare("23to78") == 0  || road.compare("23to90") == 0  || road.compare("90to24") == 0
        || road.compare("24to90") == 0 || road.compare("96to102") == 0 || road.compare("102to96") == 0
        || road.compare("110to111") == 0 || road.compare("23to17") == 0  || road.compare("44to45") == 0
        || road.compare("45to44") == 0 || road.compare("46to45") == 0 || road.compare("25to17") == 0)
            return false;

    return true;
}

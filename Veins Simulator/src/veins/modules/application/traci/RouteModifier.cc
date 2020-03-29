#include "RouteModifier.h"
#include <stdlib.h>
#include <string>
#include <list>

std::list<std::string> RouteModifier::replaceReduxRoads(std::list<std::string> redux)
{
    std::list<std::string> result;
    for(std::string road : redux)
    {
        if(road.compare("2to7") == 0)
        {
            result.push_back("2to16");
            result.push_back("16to7");
        }
        else if(road.compare("3to8") == 0)
        {
            result.push_back("3to17");
            result.push_back("17to8");
        }
        else if(road.compare("4to9") == 0)
        {
            result.push_back("4to18");
            result.push_back("18to9");
        }
        else if(road.compare("6to7") == 0)
        {
            result.push_back("6to20");
            result.push_back("20to7");
        }
        else if(road.compare("7to8") == 0)
        {
            result.push_back("7to26");
            result.push_back("26to21");
            result.push_back("21to8");
        }
        else if(road.compare("8to9") == 0)
        {
            result.push_back("8to27");
            result.push_back("27to22");
            result.push_back("22to9");
        }
        else if(road.compare("9to10") == 0)
        {
            result.push_back("9to28");
            result.push_back("28to10");
        }
        else if(road.compare("7to12") == 0)
        {
            result.push_back("7to23");
            result.push_back("23to12");
        }
        else if(road.compare("8to13") == 0)
        {
            result.push_back("8to24");
            result.push_back("24to13");
        }
        else if(road.compare("9to14") == 0)
        {
            result.push_back("9to25");
            result.push_back("25to14");
        }
        else if(road.compare("7to2") == 0)
        {
            result.push_back("7to16");
            result.push_back("16to2");
        }
        else if(road.compare("8to3") == 0)
        {
            result.push_back("8to17");
            result.push_back("17to3");
        }
        else if(road.compare("9to4") == 0)
        {
            result.push_back("9to18");
            result.push_back("18to4");
        }
        else if(road.compare("7to6") == 0)
        {
            result.push_back("7to20");
            result.push_back("20to6");
        }
        else if(road.compare("8to7") == 0)
        {
            result.push_back("8to21");
            result.push_back("21to26");
            result.push_back("26to7");
        }
        else if(road.compare("9to8") == 0)
        {
            result.push_back("9to22");
            result.push_back("22to27");
            result.push_back("27to8");
        }
        else if(road.compare("10to9") == 0)
        {
            result.push_back("10to28");
            result.push_back("28to9");
        }

        else if(road.compare("12to7") == 0)
        {
            result.push_back("12to23");
            result.push_back("23to7");
        }
        else if(road.compare("13to8") == 0)
        {
            result.push_back("13to24");
            result.push_back("24to8");
        }
        else if(road.compare("14to9") == 0)
        {
            result.push_back("14to25");
            result.push_back("25to9");
        }
        else if(road.compare("26to8") == 0)
        {
            result.push_back("26to21");
            result.push_back("21to8");
        }
        else if(road.compare("21to7") == 0)
        {
            result.push_back("21to26");
            result.push_back("26to7");
        }
        else if(road.compare("27to9") == 0)
        {
            result.push_back("27to22");
            result.push_back("22to9");
        }
        else if(road.compare("22to8") == 0)
        {
            result.push_back("22to27");
            result.push_back("27to8");
        }
        else
        {
            result.push_back(road);
        }
    }
    return result;
}

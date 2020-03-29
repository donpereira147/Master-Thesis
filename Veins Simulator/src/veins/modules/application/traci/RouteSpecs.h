#include <stdlib.h>
#include <math.h>
#include <string>
#include "/veins/base/utils/Coord.h"

class RouteSpecs
{
	private :
	    Coord p1;
	    Coord p2;
	    std::string idRoad;
	    
	public : 
	    void setRouteSpecs(double posx1, double posy1, double posx2, double posy2, std::string id);
	    double distance(Coord a, Coord b);
	    std::string coordinateInFromTheRoute(double posX, double posY);
};

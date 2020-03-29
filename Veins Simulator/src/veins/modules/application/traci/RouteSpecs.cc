#include "RouteSpecs.h"
#include <stdlib.h>
#include <string>
#include <math.h>

void RouteSpecs::setRouteSpecs(double posx1, double posy1, double posx2, double posy2, std::string id)
{
	p1.x = posx1;
	p1.y = posy1;
	p2.x = posx2;
	p2.y = posy2;
	idRoad = id;
}

double RouteSpecs::distance(Coord a, Coord b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}
/*
 *
 * A-------B
 * |       |
 * p1      p2  ---->rua horizontal
 * |       |
 * C-------D
 * */
std::string RouteSpecs::coordinateInFromTheRoute(double posX, double posY)
{

    //verificar se o ponto está dentro da área definida
	if(std::abs(p1.x - p2.x) >= std::abs(p1.y - p2.y))
	{
	    if(posX >= p1.x && posX < p2.x && posY < p1.y + 20 && posY >= p2.y - 20)
	    {
	        return idRoad;
	    }
	}
	else
	{
	    if(posX >= p1.x -20 && posX < p2.x && posY >= p1.y && posY < p2.y)
	        return idRoad;
	}

	return "";
}

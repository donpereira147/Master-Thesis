#include "GeometricOperations.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

#define M_PI           3.14159265358979323846  /* pi */

bool GeometricOperations::isNearlyEqual(float first, float second)
{
    if(std::abs(first - second) < 0.001)
        return 0;
    return 1;
}

double GeometricOperations::distance(Coord a, Coord b)
{
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

bool GeometricOperations::vehicleInFrontOfEV(Coord posAmbulance, Coord posVeh, float orientation)
{
    bool isInFront = false;
    if(isNearlyEqual(orientation, 0) == 0)
    {
        //y tem de ser igual && vehX > ambX
        if(posAmbulance.y == posVeh.y && posVeh.x > posAmbulance.x)
            isInFront = true;
    }
    else if(isNearlyEqual(orientation, -M_PI) == 0)
    {
        if(posAmbulance.y == posVeh.y && posVeh.x < posAmbulance.x)
            isInFront = true;
    }
    else if(isNearlyEqual(orientation, M_PI/2) == 0)
    {
        if(posAmbulance.x == posVeh.x && posVeh.y < posAmbulance.y)
            isInFront = true;
    }
    else if(isNearlyEqual(orientation, -M_PI/2) == 0)
    {
        if(posAmbulance.x == posVeh.x && posVeh.y > posAmbulance.y)
            isInFront = true;
    }

    return isInFront;
}

//0-> none of that, 1->for the intersection, 2->against the intersection
int GeometricOperations::directionIntersection(Coord veh, Coord rsu, float angle)
{
    int direction = 0;
    if(isNearlyEqual(angle, -M_PI) == 0 || isNearlyEqual(angle, 0) == 0)
    {
        //std::cout << "rsux:" << rsu.x << ", rsuy:" << rsu.y << ", vehx:" << veh.x << ", vehy:" << veh.y << ", angle:" << angle << ", 0:" << std::to_string(0) << "\n";
        if((rsu.x > veh.x && isNearlyEqual(angle, 0) == 0)
                || (rsu.x < veh.x && isNearlyEqual(angle, -M_PI) == 0))
        {
            if(std::abs(veh.y-rsu.y) < 40)
                direction = 1;
        }
        else if ((rsu.x > veh.x && isNearlyEqual(angle, -M_PI) == 0)
                || (rsu.x < veh.x && isNearlyEqual(angle, 0) == 0))
       {
            if(std::abs(veh.y-rsu.y) < 40)
                direction = 2;
       }

    }
    else if(isNearlyEqual(angle, M_PI/2) == 0 || isNearlyEqual(angle, -M_PI/2) == 0)
    {

        if ((rsu.y > veh.y && isNearlyEqual(angle, -M_PI/2) == 0)
                || (rsu.y < veh.y &&isNearlyEqual(angle, M_PI/2) == 0))
        {
            if(std::abs(veh.x-rsu.x) < 40)
                direction = 1;
        }
        else if((rsu.y > veh.y && isNearlyEqual(angle, M_PI/2) == 0)
                || (rsu.y < veh.y && isNearlyEqual(angle, -M_PI/2) == 0))
        {
            if(std::abs(veh.x-rsu.x) < 40)
                direction = 2;
        }
    }

    return direction;
}

bool GeometricOperations::eVPassedVehicle(Coord posV, Coord posA, float orientation)
{
    bool passed = false;

    if(isNearlyEqual(orientation, 0) == 0)
        {
            if( posA.x > posV.x)
                passed = true;
        }
        else if(isNearlyEqual(orientation, -M_PI) == 0)
        {
            if( posA.x < posV.x)
                passed = true;
        }
        else if(isNearlyEqual(orientation, M_PI/2) == 0)
        {
            if(posA.y < posV.y)
                passed = true;
        }
        else if(isNearlyEqual(orientation, -M_PI/2) == 0)
        {
            if(posA.y > posV.y)
                passed = true;
        }
    return passed;
}
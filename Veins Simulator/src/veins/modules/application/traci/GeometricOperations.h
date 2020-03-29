#include <stdlib.h>
#include "/veins/base/utils/Coord.h"

class GeometricOperations
{
	public:
		static bool isNearlyEqual(float first, float second);
		static double distance(Coord a, Coord b);
		static bool vehicleInFrontOfEV(Coord posAmbulance, Coord posVeh, float orientation);
		static int directionIntersection(Coord veh, Coord rsu, float angle);
		static bool eVPassedVehicle(Coord posV, Coord posA, float orientation);
};
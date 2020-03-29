#include <stdlib.h>
#include <string>
#include <vector>


class CommonOps
{
    
    public:
	    static std::string createShortIdForSelf(std::string flag, std::string hostName);
	    static std::vector<std::string> split(std::string original, std::string delimiter);
	    static bool canISwitchLanes(bool fewTests, std::string road);
};

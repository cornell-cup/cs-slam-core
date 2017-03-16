#ifndef _SLAM_CONSTANTS
#define _SLAM_CONSTANTS

#include <vector>

#include <memory>

#define Consts Constants::Instance()
class Constants
{
public:
	std::vector<std::string> displays = { "Left", "Right", "Disparity" };

	static std::shared_pointer<Constants> Instance()
	{
		std::shared_pointer<Constants> instance = new Constants();

		return instance;
	}

private:
	Constants() {}
};

#endif
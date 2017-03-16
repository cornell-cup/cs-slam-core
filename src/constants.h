#ifndef _SLAM_CONSTANTS
#define _SLAM_CONSTANTS

#include <vector>
#include <memory>

#define Consts Constants::Instance()
class Constants
{
public:
	std::set<std::string> displays = {"Left", "Right", "Disparity"};

	static std::shared_ptr<Constants> Instance()
	{
		static std::shared_ptr<Constants> instance(new Constants());

		return instance;
	}

private:
	Constants() {}
};

#endif
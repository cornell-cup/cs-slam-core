#pragma once

#include <vector>

#define Consts Constants::Instance()
class Constants
{
public:
	std::vector<std::string> displays = { "Left", "Right", "Dispartiy" };

	static Constants* Instance()
	{
		Constants* instance = new Constants();

		return instance;
	}

private:
	Constants() {}
};
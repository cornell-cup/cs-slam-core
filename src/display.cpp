#include "display.h"

Display::Display()
{

}

Display::~Display() {}

void Display::draw(std::string windowName, cv::Mat& image)
{
	if (!hasWindow(windowName))
		addWindow(windowName);

	cv::imshow(windowName, image);
}

void Display::addWindow(std::string windowName)
{
	if (!hasWindow(windowName))
	{
		_displayNames.insert(windowName);
		cv::namedWindow(windowName);
	}
}

void Display::addWindows(std::set<std::string> windowNames)
{
	for(std::string windowName : windowNames)
	if (!hasWindow(windowName))
	{
		_displayNames.insert(windowName);
		cv::namedWindow(windowName);
	}
}

bool Display::hasWindow(std::string windowName)
{
	return _displayNames.find(windowName) != _displayNames.end();
}

void Display::displayImages(std::unordered_map<std::string, cv::Mat>& displayImages){

    for(auto it: displayImages) {
		if (!hasWindow(it.first))
			addWindow(it.first);
		draw(it.first, it.second);
    }
 }

std::shared_ptr<Display> Display::Instance()
{
		static std::shared_ptr<Display> instance(new Display());

		return instance;
}

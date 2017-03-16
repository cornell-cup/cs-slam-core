#ifndef _SLAM_DISPLAY
#define _SLAM_DISPLAY

#include <vector>
#include <string>
#include <iostream>
#include <set>
#include <unordered_map>
#include <memory>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#define Displays Display::Instance()

class Display {
  private:
    std::set<std::string> _displayNames;
	Display();

  public:
    virtual ~Display();

	void draw(std::string windowName, cv::Mat & image);

	void addWindow(std::string windowName);
	void addWindows(std::set<std::string> windowNames);
	bool hasWindow(std::string windowName);
	void displayImages(std::unordered_map<std::string, cv::Mat>& displayImages);

	static std::shared_ptr<Display> Instance();
};

#endif
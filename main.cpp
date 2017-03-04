#include <vector>
#include <string>

#include "display.h"

int main() {
  std::vector<std::string> displayNames;
  displayNames.push_back("Left");
  displayNames.push_back("Right");
  displayNames.push_back("Disparity");
  SLAMDisplay displayPipe(displayNames);
  return 0;
}
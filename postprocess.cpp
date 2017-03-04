#include "postprocess.h"

PostProcess::PostProcess(){}

PostProcess::~PostProcess(){}

void PostProcess::normalize(cv::Mat& input, cv::Mat& output) {
  cv2.normalize(input, output, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)
}
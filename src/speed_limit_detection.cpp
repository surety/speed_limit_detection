// speed_limit_detection.cpp
// Zeus Polanco Salgado
// December 2021

// For testing ("Works on my computer")
// Compile with C++ 11:
// g++ speedlimit.cpp --std=c++11 $(pkg-config --libs --cflags opencv4 tesseract)

#include "speed_limit_detection.h"
 // pytesseract.image_to_string(threshold,lang='eng',config='--psm 10 --oem 3 -c tessedit_char_whitelist=0123456789')

// Input: Image and bounding box
// Output: Value of sign/s
// Set Image -> Set bounding boxes -> get values -> output values
int * detectSpeedLimit(Mat frameImage, int * boxes){
  int numOfBoxes = sizeof(boxes)/sizeof(boxes[0]);
  int outValues[numOfBoxes];

	tesseract::TessBaseAPI api;
  // Init(datapath, english language)
	if (api.Init(NULL, "eng")){
		cout << "Could not open or find the image" << endl;
	}

  // preprocess sign image using OpenCV
  cvtColor(sign, sign, COLOR_BGR2GRAY);
  threshold(sign, sign, 55, 255, THRESH_BINARY);

  // Use TessBaseAPI to find the value of sign
  api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
  api.SetVariable("tessedit_char_whitelist", "0123456789");
  api.SetVariable("oem", "3");
  api.SetImage((uchar*)sign.data, sign.size().width, sign.size().height, sign.channels(), sign.step1());
  for (int i = 0; i < numOfBoxes; i++){
    // x , y , w , h
    api.SetRectangle(boxes[i][0], boxes[i][1],
                     boxes[i][2], boxes[i][3]); // set bounding box values
    api.Recognize(0);
    outValues[i] = api.GetUTF8Text();
  }
  api.End();
  return outValues;
}
// End of detectSpeedLimit

Mat ROSToCV(const sensor_msgs::image_encodings& ROSImage){
  cv_bridge::CvImagePtr cv_ptr;
  try{
    cv_ptr = cv_bridge::toCvCopy(ROSImage, "rgb8");
  }
  catch (cv_bridge::Exception& e){
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
  return cv_ptr->image;
}

// Main only used for testing purposes
int main(int argc, char** argv){
 Mat image = imread("speedlLimit-55.jpg");

 // Check for failure
 if (image.empty()) {
  cout << "Could not open or find the image" << endl;
  return -1;
 }

 detectSpeedLimit(image);

 return 0;
}
// End of Main

// End of file

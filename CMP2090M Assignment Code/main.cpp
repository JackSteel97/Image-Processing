//main.cpp
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include "Timer.h"
#include "Image.h"
#include "Stacker.h"
#include "Scaler.h"
#include "Utils.h"
using namespace std;

/// <summary>
/// reads images into a vector
/// </summary>
/// <param name="set">the numbered image set to read into memory</param>
/// <returns>Vector containing the images from the given set</returns>
vector<Image> readImagesForStacking(const unsigned int &set) {
	//read all images into vector
	cout << "\n\tReading Images\n";
	cout << "************************************\n";
	Timer timer;
	timer.start();
	vector<Image> images;
	//switch on the given set
	switch (set) {
	case 1:
		//set 1
		images = {
			Image("Images/ImageStacker_set1/IMG_1.ppm"),
			Image("Images/ImageStacker_set1/IMG_2.ppm"),
			Image("Images/ImageStacker_set1/IMG_3.ppm"),
			Image("Images/ImageStacker_set1/IMG_4.ppm"),
			Image("Images/ImageStacker_set1/IMG_5.ppm"),
			Image("Images/ImageStacker_set1/IMG_6.ppm"),
			Image("Images/ImageStacker_set1/IMG_7.ppm"),
			Image("Images/ImageStacker_set1/IMG_8.ppm"),
			Image("Images/ImageStacker_set1/IMG_9.ppm"),
			Image("Images/ImageStacker_set1/IMG_10.ppm"),
			Image("Images/ImageStacker_set1/IMG_11.ppm"),
			Image("Images/ImageStacker_set1/IMG_12.ppm"),
			Image("Images/ImageStacker_set1/IMG_13.ppm")
		};
		break;
	case 2:
		//set 2
		images = {
			Image("Images/ImageStacker_set2/IMG_1.ppm"),
			Image("Images/ImageStacker_set2/IMG_2.ppm"),
			Image("Images/ImageStacker_set2/IMG_3.ppm"),
			Image("Images/ImageStacker_set2/IMG_4.ppm"),
			Image("Images/ImageStacker_set2/IMG_5.ppm"),
			Image("Images/ImageStacker_set2/IMG_6.ppm"),
			Image("Images/ImageStacker_set2/IMG_7.ppm"),
			Image("Images/ImageStacker_set2/IMG_8.ppm"),
			Image("Images/ImageStacker_set2/IMG_9.ppm"),
			Image("Images/ImageStacker_set2/IMG_10.ppm")
		};
		break;
	case 3:
		//set 3
		images = {
			Image("Images/ImageStacker_set3/IMG_1.ppm"),
			Image("Images/ImageStacker_set3/IMG_2.ppm"),
			Image("Images/ImageStacker_set3/IMG_3.ppm"),
			Image("Images/ImageStacker_set3/IMG_4.ppm"),
			Image("Images/ImageStacker_set3/IMG_5.ppm"),
			Image("Images/ImageStacker_set3/IMG_6.ppm"),
			Image("Images/ImageStacker_set3/IMG_7.ppm"),
			Image("Images/ImageStacker_set3/IMG_8.ppm"),
			Image("Images/ImageStacker_set3/IMG_9.ppm"),
			Image("Images/ImageStacker_set3/IMG_10.ppm")
		};
		break;
	case 4:
		//set 4
		images = {
			Image("Images/ImageStacker_set4/IMG_1.ppm"),
			Image("Images/ImageStacker_set4/IMG_2.ppm"),
			Image("Images/ImageStacker_set4/IMG_3.ppm"),
			Image("Images/ImageStacker_set4/IMG_4.ppm"),
			Image("Images/ImageStacker_set4/IMG_5.ppm"),
			Image("Images/ImageStacker_set4/IMG_6.ppm"),
			Image("Images/ImageStacker_set4/IMG_7.ppm"),
			Image("Images/ImageStacker_set4/IMG_8.ppm"),
			Image("Images/ImageStacker_set4/IMG_9.ppm"),
			Image("Images/ImageStacker_set4/IMG_10.ppm")
		};
		break;
	default:
		cout << "\nInvalid Image Set" << endl;
		break;
	}
	cout << "************************************\n";

	timer.stop();

	cout << "Finished in " << timer.getSeconds() << " seconds\n";
	return images;
}

/// <summary>
/// Runs the image stacker
/// </summary>
/// <param name="method">numbered stacking method to use</param>
/// <param name="imageSet">numbered image set to stack</param>
void ImageStacker(const unsigned int &method, const unsigned int &imageSet) {
	//read images into memory
	vector<Image> images = readImagesForStacking(imageSet);
	StackedImage output;
	Timer timer;
	timer.start();
	string fileName = "default.ppm";
	//switch on the stacking method
	switch (method) {
	case 1:
		//mean blending
		cout << "\nMean Blending Images...\n";
		fileName = "MeanOutput.ppm";
		output = Stacker::MeanBlend(images);
		break;
	case 2:
		//median blending (optimised)
		cout << "\nMedian Blending Images...\n";
		fileName = "MedianOutput.ppm";
		output = Stacker::MedianBlendParallel(images);
		break;
	case 3:
		//sigma clipped mean blending (optimised)
		cout << "\nSigma Clipped Mean Blending Images...\n";
		fileName = "SigmaClippedMeanOutput.ppm";
		output = Stacker::SigmaClippedMeanBlendParallel(images, 5);
		break;
	case 4:
		//median blending
		cout << "\nMedian Blending Images...\n";
		fileName = "MedianOutput2.ppm";
		output = Stacker::MedianBlend(images);
		break;
	case 5:
		//sigma clipped mean blending
		cout << "\nSigma Clipped Mean Blending Images...\n";
		fileName = "SigmaClippedMeanOutput2.ppm";
		output = Stacker::SigmaClippedMeanBlend(images, 1);
		break;
	default:
		cout << "Invalid blend method\n";
		return;
	}

	timer.stop();
	cout << "Finished Blending in " << timer.getSeconds() << " seconds\n";
	
	//write to file
	string filePath = "Images/ImageStacker_set" + to_string(imageSet) + "/" + fileName;
	output.writePPM(filePath.c_str());

	//log output
	output.logDetails();

	//delete pixel memory
	output.freeMemory();
	return;
}

/// <summary>
/// run the image scaler
/// </summary>
/// <param name="method">numbered scaling method to use</param>
/// <param name="scale">scale factor</param>
/// <param name="scaleROI">scale a ROI flag</param>
/// <param name="roiLeft">ROI top left x coordinate</param>
/// <param name="roiTop">ROI top left y coordinate</param>
/// <param name="roiWidth">width of ROI</param>
/// <param name="roiHeight">height of ROI</param>
void ImageScaler(const unsigned int &method, const double &scale, const bool &scaleROI = false, const unsigned int &roiLeft = 0, const unsigned int &roiTop = 0, const unsigned int &roiWidth = 0, const unsigned int &roiHeight = 0) {
	Image img;
	std::stringstream fileName;
	cout << "\n";
	Timer timer;
	timer.start();

	//are we using a ROI?
	if (scaleROI) {
		//yes get the region of interest first
		img = Scaler::ExtractRegionOfInterest(Image("Images/Zoom/zImg_1.ppm"), roiLeft, roiTop, roiWidth, roiHeight);
	} else {
		//no, load the whole image
		img = Image("Images/Zoom/zImg_1.ppm");
	}

	ScaledImage output;
	//switch on the scaling method
	switch (method) {
	case 1:
		//nearest neighbour (optimised)
		cout << "\nNearest Neighbour Scaling...\n";
		fileName << "NearestNeighbourScaled" << scale << "x.ppm";
		output = Scaler::NearestNeighbourParallel(img, scale);
		break;
	case 2:
		//bilinear (optimised)
		cout << "\nBilinear Scaling...\n";
		fileName << "BilinearScaled" << scale << "x.ppm";
		output = Scaler::BilinearParallel(img, scale);
		break;
	case 3:
		//bicubic (optimised)
		cout << "\nBicubic Scaling...\n";
		fileName << "BicubicScaled" << scale << "x.ppm";
		output = Scaler::BiCubicParallel(img, scale);
		break;
	case 4:
		//nearest neighbour
		cout << "\nNearest Neighbour Scaling...\n";
		fileName << "NearestNeighbourScaledSerial" << scale << "x.ppm";
		output = Scaler::NearestNeighbour(img, scale);
		break;
	case 5:
		//bilinear
		cout << "\nBilinear Scaling...\n";
		fileName << "BilinearScaledSerial" << scale << "x.ppm";
		output = Scaler::Bilinear(img, scale);
		break;
	case 6:
		//bicubic
		cout << "\nBicubic Scaling...\n";
		fileName << "BicubicScaledSerial" << scale << "x.ppm";
		output = Scaler::BiCubic(img, scale);
		break;
	default:
		cout << "Invalid Scaling Method";
		return;
	}
	timer.stop();

	cout << "Finished Scaling in " << timer.getSeconds() << " seconds\n";

	//write to file
	string filePath = "Images/Zoom/" + fileName.str();	
	output.writePPM(filePath.c_str());

	//log details
	output.logDetails();
	
	//delete pixel memory
	img.freeMemory();
	output.freeMemory();
	return;
}

/// <summary>
/// Display the image stacker menu
/// </summary>
void showImageStackerMenu() {
	clearConsole();
	cout << "IMAGE STACKER\n\n";
	cout << "\t1. Mean Blending\n\t2. Median Blending\n\t3. Sigma Clipped Mean Blending\n";
	cout << "Choose Blending Method: ";
	int choice = getUserInputInteger();

	cout << "\n1. Image set 1\n2. Image set 2\n3. Image set 3\n4. Image set 4\n";
	cout << "Choose Image Set: ";
	int setChoice = getUserInputInteger();

	//run image stacker with user choices
	ImageStacker(choice, setChoice);
}

/// <summary>
/// Display the image scaler menu
/// </summary>
void showImageScalerMenu() {
	clearConsole();
	cout << "IMAGE SCALER\n\n";
	cout << "\t1. Nearest Neighbour\n\t2. Bilinear\n\t3. Bicubic\n";
	cout << "Choose Scaling Method: ";
	int choice = getUserInputInteger();

	cout << "\nEnter a scale factor: ";
	double scaleFactor = getUserInputDouble();

	cout << "\nScale a region of interest?\n1. Yes\n2. No, scale the whole image\n";
	cout << "Choice: ";
	int roiChoice = getUserInputInteger();

	int left, top, width, height;
	//switch on the user's choice to use a ROI
	switch (roiChoice)
	{
	case 1:
		//use ROI
		cout << "Enter X value for top left of ROI: ";
		left = getUserInputInteger();
		cout << "Enter Y value for top left of ROI: ";
		top = getUserInputInteger();
		cout << "Enter width of ROI: ";
		width = getUserInputInteger();
		cout << "Enter height of ROI: ";
		height = getUserInputInteger();
		//run scaler with user choices
		ImageScaler(choice, scaleFactor, true, left, top, width, height);
		break;
	case 2:
		//don't use ROI, run scaler on whole image with user's scale factor
		ImageScaler(choice, scaleFactor);
		break;
	default:
		cout << "\nInvalid ROI Choice!" << endl;
		break;
	}
}

/// <summary>
/// Run the scaler benchmark
/// </summary>
void benchmarkScaler() {
	//output timings to log file
	ofstream logFile;
	logFile.open("Benchmark.txt", ios::app);
	Timer timer;
	time_t benchStart = time(&benchStart);

	string startTime(ctime(&benchStart));
	logFile << "Starting Scaler Benchmark: " << startTime;
	logFile << "Timings include read/write of images";
	
	//run all algorithms at 2x scale factor
	logFile << "\nScale 2x:";

	timer.start();
	ImageScaler(4, 2);
	timer.stop();
	logFile << "\n\tNearest Neighbour: " << timer.getSeconds();

	timer.start();
	ImageScaler(1, 2);
	timer.stop();
	logFile << "\n\tNearest Neighbour Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(5, 2);
	timer.stop();
	logFile << "\n\n\tBilinear: " << timer.getSeconds();

	timer.start();
	ImageScaler(2, 2);
	timer.stop();
	logFile << "\n\tBilinear Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(6, 2);
	timer.stop();
	logFile << "\n\n\tBicubic: " << timer.getSeconds();

	timer.start();
	ImageScaler(3, 2);
	timer.stop();
	logFile << "\n\tBicubic Parallel: " << timer.getSeconds();

	//run all algorithms at 4x scale factor
	logFile << "\nScale 4x:";

	timer.start();
	ImageScaler(4, 4);
	timer.stop();
	logFile << "\n\tNearest Neighbour: " << timer.getSeconds();

	timer.start();
	ImageScaler(1, 4);
	timer.stop();
	logFile << "\n\tNearest Neighbour Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(5, 4);
	timer.stop();
	logFile << "\n\n\tBilinear: " << timer.getSeconds();

	timer.start();
	ImageScaler(2, 4);
	timer.stop();
	logFile << "\n\tBilinear Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(6, 4);
	timer.stop();
	logFile << "\n\n\tBicubic: " << timer.getSeconds();

	timer.start();
	ImageScaler(3, 4);
	timer.stop();
	logFile << "\n\tBicubic Parallel: " << timer.getSeconds();

	//run all algorithms at 10x scale factor
	logFile << "\nScale 10x:";

	timer.start();
	ImageScaler(4, 10);
	timer.stop();
	logFile << "\n\tNearest Neighbour: " << timer.getSeconds();

	timer.start();
	ImageScaler(1, 10);
	timer.stop();
	logFile << "\n\tNearest Neighbour Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(5, 10);
	timer.stop();
	logFile << "\n\n\tBilinear: " << timer.getSeconds();

	timer.start();
	ImageScaler(2, 10);
	timer.stop();
	logFile << "\n\tBilinear Parallel: " << timer.getSeconds();

	timer.start();
	ImageScaler(6, 10);
	timer.stop();
	logFile << "\n\n\tBicubic: " << timer.getSeconds();

	timer.start();
	ImageScaler(3, 10);
	timer.stop();
	logFile << "\n\tBicubic Parallel: " << timer.getSeconds();

	logFile << "\n\n";
	logFile.close();
}

/// <summary>
/// Run image stacker benchmark
/// </summary>
void benchmarkStacker() {
	//output timings to log file
	ofstream logFile;
	logFile.open("Benchmark.txt", ios::app);
	Timer timer;
	time_t benchStart = time(&benchStart);

	string startTime(ctime(&benchStart));
	logFile << "Starting Stacking Benchmark: " << startTime;
	logFile << "Timings include read/write of images";

	//run all algorithms on image set 1
	logFile << "\nImage Set 1:";

	timer.start();
	ImageStacker(1, 1);
	timer.stop();
	logFile << "\n\tMean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(4, 1);
	timer.stop();
	logFile << "\n\tMedian Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(2, 1);
	timer.stop();
	logFile << "\n\tMedian Blending Parallel Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(5, 1);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(3, 1);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Parallel Blending Time: " << timer.getSeconds() << " seconds";

	//run all algorithms on image set 2
	logFile << "\nImage Set 2:";

	timer.start();
	ImageStacker(1, 2);
	timer.stop();
	logFile << "\n\tMean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(4, 2);
	timer.stop();
	logFile << "\n\tMedian Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(2, 2);
	timer.stop();
	logFile << "\n\tMedian Blending Parallel Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(5, 2);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(3, 2);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Parallel Blending Time: " << timer.getSeconds() << " seconds";

	//run all algorithms on image set 3
	logFile << "\nImage Set 3:";

	timer.start();
	ImageStacker(1, 3);
	timer.stop();
	logFile << "\n\tMean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(4, 3);
	timer.stop();
	logFile << "\n\tMedian Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(2, 3);
	timer.stop();
	logFile << "\n\tMedian Blending Parallel Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(5, 3);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(3, 3);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Parallel Blending Time: " << timer.getSeconds() << " seconds";

	//run all algorithms on image set 4
	logFile << "\nImage Set 4:";

	timer.start();
	ImageStacker(1, 4);
	timer.stop();
	logFile << "\n\tMean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(4, 4);
	timer.stop();
	logFile << "\n\tMedian Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(2, 4);
	timer.stop();
	logFile << "\n\tMedian Blending Parallel Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(5, 4);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Blending Time: " << timer.getSeconds() << " seconds";

	timer.start();
	ImageStacker(3, 4);
	timer.stop();
	logFile << "\n\tSigma Clipped Mean Parallel Blending Time: " << timer.getSeconds() << " seconds";

	logFile << "\n\n";
	logFile.close();
}

/// <summary>
/// Display the main menu
/// </summary>
/// <returns>exit code</returns>
int showMainMenu() {
	clearConsole();
	cout << "************************************\n";
	cout << "Image Stacker / Image Scaler\n";
	cout << "************************************\n";
	cout << "MAIN MENU\n";
	cout << "\t1. Image Stacker\n\t2. Image Scaler\n\t3. Benchmark (Outputs results to benchmark.txt, takes about 10 mins)\n\t4. Quit\n";
	cout << "Choose an option: ";
	int choice = getUserInputInteger();

	switch (choice) {
	case 1:
		showImageStackerMenu();
		break;
	case 2:
		showImageScalerMenu();
		break;
	case 3:
		benchmarkScaler();
		benchmarkStacker();
		break;
	case 4:
		return 0;
	}
	//wait for user to continue
	system("pause");
	return 1;
}

/// <summary>
/// Program must be run in the same directory as the Images folder
/// 
/// The Images folder should have the structure:
/// Images:
///		ImageStacker_set1:
///			set 1 images...
///		ImageStacker_set2:
///			set 2 images...
///		ImageStacker_set3:
///			set 3 images...
///		ImageStacker_set4:
///			set 4 images...
///		Zoom:
///			zImg_1.ppm
/// 
/// For best results run from the provided CMP2090M Assignment Code.exe in: Dev\CMP2090M Assignment Code
/// For best results when running from IDE run in Release x64
/// 
/// text file outputs will be written in the same directory as the program executable
/// Image outputs will be written in the same directory as their source image(s)
/// </summary>
/// <returns>Exit code</returns>
int main() {
	//repeat until user chooses to quit
	while (showMainMenu() != 0);
	return 0;
}
#pragma once

//*********************************************
//Image class to hold and allow manipulation of images once read into the code
//from https://www.scratchapixel.com/
//*********************************************

#include <cstdlib> 
#include <cstdio>
#include "Timer.h"
#include "Utils.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

class Image
{
public:
	// Rgb structure, i.e. a pixel 
	struct Rgb
	{
		Rgb() : r(0), g(0), b(0) {}
		Rgb(unsigned char c) : r(c), g(c), b(c) {}
		Rgb(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {}
		bool operator != (const Rgb &c) const
		{
			return c.r != r || c.g != g || c.b != b;
		}
		Rgb& operator *= (const Rgb &rgb)
		{
			r *= rgb.r, g *= rgb.g, b *= rgb.b; return *this;
		}
		Rgb& operator += (const Rgb &rgb)
		{
			r += rgb.r, g += rgb.g, b += rgb.b; return *this;
		}
		friend unsigned char& operator += (unsigned char &f, const Rgb rgb)
		{
			f += (rgb.r + rgb.g + rgb.b) / 3.f; return f;
		}
		unsigned char r, g, b;
	};

	Image() : w(0), h(0), pixels(nullptr) { 
		//empty image
		creationTime = time(&creationTime);
		modifiedTime = time(&modifiedTime);
	}
	Image(const unsigned int &_w, const unsigned int &_h, char *_fileName = "No Source File", const Rgb &c = kBlack) :
		w(_w), h(_h), pixels(NULL), fileName(_fileName)
	{
		creationTime = time(&creationTime);
		modifiedTime = time(&modifiedTime);
		const unsigned int imageSize = w * h;
		pixels = new Rgb[imageSize];
		for (unsigned int i = 0; i < imageSize; ++i)
			pixels[i] = c;
	}
	Image(char* _filename) : fileName(_filename) {
		creationTime = time(&creationTime);
		modifiedTime = time(&modifiedTime);
		this->readPPM(_filename);
		logDetails();
	}
	const Rgb& operator [] (const unsigned int &i) const
	{
		return pixels[i];
	}
	Rgb& operator [] (const unsigned int &i)
	{
		return pixels[i];
	}

	void freeMemory() {
		if (pixels != NULL) {
			delete[] pixels;
		}		
	}

	unsigned int w, h; // Image resolution 
	Rgb *pixels; // 1D array of pixels 
	static const Rgb kBlack, kWhite, kRed, kGreen, kBlue; // Preset colors 

	void virtual logDetails() {
		ofstream logFile;
		logFile.open("DetailsLog.txt", ios::app);
		logFile << "\n======Image======\n";
		logFile << "Source Image: " << fileName << "\n";
		logFile << "Image Width: " << w << "\n";
		logFile << "Image Height: " << h << "\n";
		logFile << "Colour Depth: " << sizeof(pixels[0].r) * 3 << " bit\n";
		logFile << "Image Statistics: " << "\n\tImage Size in memory: " << bytesToAppropriate(sizeof(pixels[0].r) * 3 * h*w + sizeof(h) + sizeof(w) + sizeof(fileName)).str() << "\n";
		string createStr(ctime(&creationTime));
		string modifStr(ctime(&modifiedTime));
		logFile << "\tCreated: " << createStr << "\tLast Modified: " << modifStr;
		logFile << "=================\n";
		logFile.close();
	
	}

	void setFileName(const char *_fileName) {
		fileName = (char*)_fileName;
	}

	void updateModified() {
		modifiedTime = time(&modifiedTime);
	}

	void setColourDepth(int depthInBits) {
		colourDepth = depthInBits;
	}

	//Read ppm files into the code
	//They need to be in 'binary' format (P6) with no comments in the header
	//The first line is the 'P'number - P6 indicates it is a binary file, then the image dimensions and finally the colour range
	//This header is then followed by the pixel colour data
	//eg:	P6
	//		3264 2448
	//		255
	//Open a .ppm file in notepad++ to see this header (caution: they are large files!)
	void readPPM(const char *filename)
	{
		//Remove this cout to prevent multiple outputs
		std::cout << "Reading image..." << std::endl;
		Timer timer;
		timer.start();
		std::ifstream ifs;
		ifs.open(filename, std::ios::binary);
		try {
			if (ifs.fail()) {
				throw("Can't open the input file - is it named correctly/is it in the right directory?");
			}
			std::string header;
			int w, h, b;
			ifs >> header;
			if (strcmp(header.c_str(), "P6") != 0) throw("Can't read the input file - is it in binary format (Has P6 in the header)?");
			ifs >> w >> h >> b;
			this->w = w;
			this->h = h;
			const unsigned int imageSize = w * h;
			//calculate colour bit depth
			this->setColourDepth(log2(pow(b + 1, 3)));
			//std::cout << w << " " << h << std::endl;
			this->pixels = new Image::Rgb[w * h]; // this is throw an exception if bad_alloc 
			ifs.ignore(256, '\n'); // skip empty lines in necessary until we get to the binary data 
			unsigned char pix[3]; // read each pixel one by one and convert bytes to floats 
			for (int i = 0; i < imageSize; ++i) {
				ifs.read(reinterpret_cast<char *>(pix), 3);
				this->pixels[i].r = pix[0];
				this->pixels[i].g = pix[1];
				this->pixels[i].b = pix[2];
			}
			ifs.close();
		}
		catch (const char *err) {
			fprintf(stderr, "%s\n", err);
			ifs.close();
		}
		this->setFileName(filename);
		timer.stop();
		cout << "\tFinished Reading in " << timer.getSeconds() << " seconds\n";
	}

	//Write data out to a ppm file
	//Constructs the header as above
	void writePPM(const char *filename)
	{
		this->setFileName(filename);
		//std::cout << filename << std::endl;
		std::cout << "\nWriting image..." << std::endl;
		Timer timer;
		timer.start();
		if (this->w == 0 || this->h == 0) { fprintf(stderr, "Can't save an empty image\n"); return; }
		std::ofstream ofs;
		try {
			ofs.open(filename, std::ios::binary); // need to spec. binary mode for Windows users 
			if (ofs.fail()) throw("Can't open output file");
			ofs << "P6\n" << this->w << " " << this->h << "\n255\n";
			//std::cout << "P6\n" << img.w << " " << img.h << "\n255\n";
			unsigned char r, g, b;
			// loop over each pixel in the image, clamp and convert to byte format
			const unsigned int imageSize = this->w * this->h;
			for (unsigned int i = 0; i < imageSize; ++i) {
				r = std::min((unsigned char)255, this->pixels[i].r);
				g = std::min((unsigned char)255, this->pixels[i].g);
				b = std::min((unsigned char)255, this->pixels[i].b);
				ofs << r << g << b;
			}
			ofs.close();
			//Confirm image write
			timer.stop();
			
			cout << "\tFinished Writing in " << timer.getSeconds() << " seconds\n";
		}
		catch (const char *err) {
			fprintf(stderr, "%s\n", err);
			ofs.close();
		}
	}

	const int getColourDepth() {
		return colourDepth;
	}

protected:
	char *fileName;
	time_t creationTime;
	time_t modifiedTime;
	int colourDepth;
};

class ScaledImage : public Image {
private:
	double scaleFactor = 0.0;
	char* scalingMethod = "";
public:
	ScaledImage() : Image() {}
	ScaledImage(const unsigned int &_w, const unsigned int &_h, double _scaleFactor,char* _scalingMethod, char *_fileName = "No Source File", const Rgb &c = kBlack) : Image(_w, _h, _fileName, c) {
		scaleFactor = _scaleFactor;
		scalingMethod = _scalingMethod;
	}
	void setScaleFactor(double _scaleFactor) {
		scaleFactor = _scaleFactor;
	}
	double getScaleFactor() {
		return scaleFactor;
	}

	void logDetails() {
		ofstream logFile;
		logFile.open("DetailsLog.txt", ios::app);
		logFile << "\n======Scaled Image======\n";
		logFile << "Source Image: " << fileName << "\n";
		logFile << "Image Width: " << w << "\n";
		logFile << "Image Height: " << h << "\n";
		logFile << "Colour Depth: " << colourDepth << " bit\n";
		logFile << "Image Statistics: " << "\n\tImage Size in memory: " << bytesToAppropriate(sizeof(pixels[0].r) * 3 * h*w + sizeof(h) + sizeof(w) + sizeof(fileName)).str() << "\n";
		string createStr(ctime(&creationTime));
		string modifStr(ctime(&modifiedTime));
		logFile << "\tCreated: " << createStr << "\tLast Modified: " << modifStr;
		logFile << "\tScale Factor: " << scaleFactor << "\n";
		logFile << "========================\n";
		logFile.close();
	}
};

class StackedImage : public Image {
private:
	char* stackingMethod = "";
public:
	StackedImage() : Image() {}
	StackedImage(const unsigned int &_w, const unsigned int &_h, char* _stackingMethod, char *_fileName = "No Source File", const Rgb &c = kBlack) : Image(_w, _h, _fileName, c) {
		stackingMethod = _stackingMethod;
	}
	void setStackingMethod(char* _stackingMethod) {
		stackingMethod = _stackingMethod;
	}
	char* getStackingMethod() {
		return stackingMethod;
	}

	void logDetails() {
		ofstream logFile;
		logFile.open("DetailsLog.txt", ios::app);
		logFile << "\n======Scaled Image======\n";
		logFile << "Source Image: " << fileName << "\n";
		logFile << "Image Width: " << w << "\n";
		logFile << "Image Height: " << h << "\n";
		logFile << "Colour Depth: " << colourDepth << " bit\n";
		logFile << "Image Statistics: " << "\n\tImage Size in memory: " << bytesToAppropriate(sizeof(pixels[0].r) * 3 * h*w + sizeof(h) + sizeof(w) + sizeof(fileName)).str() << "\n";
		string createStr(ctime(&creationTime));
		string modifStr(ctime(&modifiedTime));
		logFile << "\tCreated: " << createStr << "\tLast Modified: " << modifStr;
		logFile << "\tStacking Method: " << stackingMethod << "\n";
		logFile << "========================\n";
		logFile.close();
	}

};

const Image::Rgb Image::kBlack = Image::Rgb(0);
const Image::Rgb Image::kWhite = Image::Rgb(1);
const Image::Rgb Image::kRed = Image::Rgb(1, 0, 0);
const Image::Rgb Image::kGreen = Image::Rgb(0, 1, 0);
const Image::Rgb Image::kBlue = Image::Rgb(0, 0, 1);

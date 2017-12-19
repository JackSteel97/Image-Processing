#pragma once
#include <vector>
#include <ppl.h>
#include <math.h>
#include <stdexcept>
using namespace std;
using namespace Concurrency;

/// <summary>
/// Class for image stacking
/// </summary>
class Stacker {
public:

	/// <summary>
	/// Mean blend images
	/// </summary>
	/// <param name="imgs">images to blend</param>
	/// <returns>Blended output image</returns>
	static StackedImage MeanBlend(vector<Image> &imgs) {
		const unsigned int imageNumber = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		//declare output image
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Mean Blend");
		//set colour depth
		output->setColourDepth(imgs[0].getColourDepth());
		//calculate imageSize
		const unsigned int imageSize = output->h * output->w;
		unsigned char imageCount = 1;
		//iterate through images
		for (it = imgs.begin(); it != imgs.end(); it++, imageCount++) {
			Image cur = *it;
			//iterate through pixels on
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				Image::Rgb curRgb = cur.pixels[pixelIndex];
				//calculate mean iteratively to avoid overflow: http://www.heikohoffmann.de/htmlthesis/node134.html
				output->pixels[pixelIndex].r += (curRgb.r - output->pixels[pixelIndex].r) / imageCount;
				output->pixels[pixelIndex].g += (curRgb.g - output->pixels[pixelIndex].g) / imageCount;
				output->pixels[pixelIndex].b += (curRgb.b - output->pixels[pixelIndex].b) / imageCount;
			}
			//release image memory
			cur.freeMemory();
		}

		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Median blend, using all CPU cores
	/// </summary>
	/// <param name="imgs">images to blend</param>
	/// <returns>Blended output image</returns>
	static StackedImage MedianBlendParallel(vector<Image> &imgs) {
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		//declare output image
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Median Blend");
		//set colour depth
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		//we need to store the values in arrays so they can be easily sorted
		//create these arrays.
		unsigned char** reds = new unsigned char*[imageSize];
		unsigned char** greens = new unsigned char*[imageSize];
		unsigned char** blues = new unsigned char*[imageSize];
		for (unsigned int i = 0; i < imageSize; i++) {
			reds[i] = new unsigned char[imageNum];
			greens[i] = new unsigned char[imageNum];
			blues[i] = new unsigned char[imageNum];
		}

		//iterate through the images in parallel
		parallel_for(size_t(0), imgs.size(), [&imgs, &imageSize, &output, &reds, &greens, &blues](size_t i) {
			//get the current image
			Image cur = imgs.at(i);
			//iterate through the pixels in serial
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				//store the RGB values in the arrays
				reds[pixelIndex][i] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][i] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][i] = cur.pixels[pixelIndex].b;
			}
			//release the memory of the original image now we have the pixels in arrays
			cur.freeMemory();
		});

		//get the mid point
		const unsigned int mid = (unsigned int)ceil((imageNum - 1) / 2);

		//iterate through the pixels in parallel
		parallel_for(size_t(0), size_t(imageSize), [&reds, &greens, &blues, &output, &imageNum, &mid](size_t i) {
			//sort the arrays
			sort(reds[i], reds[i] + imageNum);
			sort(greens[i], greens[i] + imageNum);
			sort(blues[i], blues[i] + imageNum);
			//get the mid point (median) from the array and assign it to the output image, for each channel
			output->pixels[i].r = reds[i][mid];
			output->pixels[i].g = greens[i][mid];
			output->pixels[i].b = blues[i][mid];
			//release memory we no longer need
			delete reds[i];
			delete greens[i];
			delete blues[i];
		});
		//release last of memory used by the arrays
		delete[] reds;
		delete[] greens;
		delete[] blues;
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Median blend images
	/// </summary>
	/// <param name="imgs">images to blend</param>
	/// <returns>Blended output image</returns>
	static StackedImage MedianBlend(vector<Image> &imgs) {
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		//declare output image
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Median Blend");
		//set colour depth
		output->setColourDepth(imgs[0].getColourDepth());
		//calculate image size
		const unsigned int imageSize = output->h * output->w;
		//we need to store the values in arrays so they can be easily sorted
		//create these arrays.
		unsigned char** reds = new unsigned char*[imageSize];
		unsigned char** greens = new unsigned char*[imageSize];
		unsigned char** blues = new unsigned char*[imageSize];
		for (unsigned int i = 0; i < imageSize; i++) {
			reds[i] = new unsigned char[imageNum];
			greens[i] = new unsigned char[imageNum];
			blues[i] = new unsigned char[imageNum];
		}
		
		unsigned int imgCount = 0;
		//iterate through the images
		for (it = imgs.begin(); it != imgs.end(); it++, imgCount++) {
			//get current image
			Image cur = *it;
			//iterate through the pixels of the current image
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				//add RGB values to arrays
				reds[pixelIndex][imgCount] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][imgCount] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][imgCount] = cur.pixels[pixelIndex].b;
			}
			//release memory of original array
			cur.freeMemory();
		}

		//get midpoint
		const unsigned int mid = (unsigned int)ceil((imageNum - 1) / 2);

		//iterate through the pixels
		for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
			//sort the RGB values
			sort(reds[pixelIndex], reds[pixelIndex] + imageNum);
			sort(greens[pixelIndex], greens[pixelIndex] + imageNum);
			sort(blues[pixelIndex], blues[pixelIndex] + imageNum);
			//assign the median value to the output image
			output->pixels[pixelIndex].r = reds[pixelIndex][mid];
			output->pixels[pixelIndex].g = greens[pixelIndex][mid];
			output->pixels[pixelIndex].b = blues[pixelIndex][mid];
			//release memory for these RGB values
			delete reds[pixelIndex];
			delete greens[pixelIndex];
			delete blues[pixelIndex];
		}
		//release last memory of these arrays
		delete[] reds;
		delete[] greens;
		delete[] blues;
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Sigma clipped mean blend, using all CPU cores
	/// </summary>
	/// <param name="imgs">images to blend</param>
	/// <param name="iterations">how many times to repeat</param>
	/// <param name="alphaValue">sigma multiplier</param>
	/// <returns>Blended output image</returns>
	static StackedImage SigmaClippedMeanBlendParallel(vector<Image> &imgs, const unsigned int &iterations, const float &alphaValue = 0.5) {
		//check iterations is valid
		if (iterations < 1) {
			throw new invalid_argument("The number of iterations cannot be less than 1!");
		}
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		cout << "Allocating Memory...\n";
		//declare output image
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Sigma Clipped Mean");
		//set colour depth
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		//similar to median blend, we need to be able to easily sort and remove the RGB values, so we store them in vectors
		//e.g. reds[i][j], where i is the index of a pixel, and j is the index of an original image, giving the value of that pixel for any given image
		vector<vector<unsigned char>> reds(imageSize);
		vector<vector<unsigned char>> greens(imageSize);
		vector<vector<unsigned char>> blues(imageSize);
		//allocate memory for the vectors to reduce overhead during the main loop
		parallel_for(size_t(0), size_t(imageSize), [&reds, &greens, &blues, &imageNum](size_t i) {
			reds[i].reserve(imageNum);
			greens[i].reserve(imageNum);
			blues[i].reserve(imageNum);
			reds[i].resize(imageNum);
			greens[i].resize(imageNum);
			blues[i].resize(imageNum);
		});

		cout << "Memory Allocated.\n";

		cout << "Reading Pixel Values...\n";
		//read pixel RGB values from original images
		//iterate through images, in parallel
		parallel_for(size_t(0), imgs.size(), [&imageSize, &reds, &greens, &blues, &imgs](size_t i) {
			Image cur = imgs[i];
			//iterate through pixels
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				//assign the values to a vector index
				reds[pixelIndex][i] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][i] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][i] = cur.pixels[pixelIndex].b;
			}
			//release the memory used by the original image as it is no longer needed
			cur.freeMemory();
		});

		cout << "Pixels Read.\n";
		cout << "Performing Sigma Clipped Mean...\n";
		// repeat the given number of times
		for (unsigned int iter = 0; iter < iterations; iter++) {
			//iterate through the pixels, in parallel
			parallel_for(size_t(0), size_t(imageSize), [&reds, &greens, &blues, &output, &alphaValue](size_t pixelIndex) {
				//sort the relevant vector
				sort(reds[pixelIndex].begin(), reds[pixelIndex].end());
				sort(greens[pixelIndex].begin(), greens[pixelIndex].end());
				sort(blues[pixelIndex].begin(), blues[pixelIndex].end());

				//calculate the median values for this pixel
				const unsigned char redMedian = reds[pixelIndex][(int)ceil((reds[pixelIndex].size()-1) / 2)];
				const unsigned char greenMedian = greens[pixelIndex][(int)ceil((greens[pixelIndex].size()-1) / 2)];
				const unsigned char blueMedian = blues[pixelIndex][(int)ceil((blues[pixelIndex].size()-1) / 2)];

				//calculate the standard deviation values for this pixel
				const float redStandardDev = calculateStandardDeviation(reds[pixelIndex], reds[pixelIndex].size());
				const float greenStandardDev = calculateStandardDeviation(greens[pixelIndex], greens[pixelIndex].size());
				const float blueStandardDev = calculateStandardDeviation(blues[pixelIndex], blues[pixelIndex].size());

				//calculate the upper and lower bound values for this pixel
				const float redMin = redMedian - (alphaValue*redStandardDev);
				const float redMax = redMedian + (alphaValue*redStandardDev);

				const float greenMin = greenMedian - (alphaValue*greenStandardDev);
				const float greenMax = greenMedian + (alphaValue*greenStandardDev);

				const float blueMin = blueMedian - (alphaValue*blueStandardDev);
				const float blueMax = blueMedian + (alphaValue*blueStandardDev);

				//remove any values outside the bounds for all channels
				for (unsigned int i = 0; i < reds[pixelIndex].size(); i++) {
					const unsigned char redVal = reds[pixelIndex][i];
					if (redVal < redMin || redVal > redMax) {
						remove(reds[pixelIndex], i);
					}
				}

				for (unsigned int i = 0; i < greens[pixelIndex].size(); i++) {
					const unsigned char greenVal = greens[pixelIndex][i];
					if (greenVal < greenMin || greenVal > greenMax) {
						remove(greens[pixelIndex], i);
					}
				}

				for (unsigned int i = 0; i < blues[pixelIndex].size(); i++) {
					const unsigned char blueVal = blues[pixelIndex][i];
					if (blueVal < blueMin || blueVal > blueMax) {
						remove(blues[pixelIndex], i);
					}
				}

				//calculate the mean of the remaining values
				output->pixels[pixelIndex].r = (unsigned char)calculateMean(reds[pixelIndex], reds[pixelIndex].size());
				output->pixels[pixelIndex].g = (unsigned char)calculateMean(greens[pixelIndex], greens[pixelIndex].size());
				output->pixels[pixelIndex].b = (unsigned char)calculateMean(blues[pixelIndex], blues[pixelIndex].size());
			});
		}
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Sigma clipped mean blend images
	/// </summary>
	/// <param name="imgs">images to blend</param>
	/// <param name="iterations">how many times to repeat</param>
	/// <param name="alphaValue">sigma multiplier</param>
	/// <returns>Blended output image</returns>
	static StackedImage SigmaClippedMeanBlend(vector<Image> &imgs, const unsigned int &iterations, const float &alphaValue = 0.5) {
		//ensure iterations is valid
		if (iterations < 1) {
			throw new invalid_argument("The number of iterations cannot be less than 1!");
		}
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		cout << "Allocating Memory...\n";
		//declare output image
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Sigma Clipped Mean");
		//set colour depth
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		//similar to median blend, we need to be able to easily sort and remove the RGB values, so we store them in vectors
		//e.g. reds[i][j], where i is the index of a pixel, and j is the index of an original image, giving the value of that pixel for any given image
		vector<vector<unsigned char>> reds(imageSize);
		vector<vector<unsigned char>> greens(imageSize);
		vector<vector<unsigned char>> blues(imageSize);

		//allocate memory for the vectors to reduce overhead during the main loop
		for (unsigned int i = 0; i < imageSize; i++) {
			reds[i].reserve(imageNum);
			greens[i].reserve(imageNum);
			blues[i].reserve(imageNum);
			reds[i].resize(imageNum);
			greens[i].resize(imageNum);
			blues[i].resize(imageNum);
		}
		cout << "Memory Allocated.\n";

		cout << "Reading Pixel Values...\n";
		unsigned int imageCount = 0;
		//read pixel RGB values from original images
		//iterate through images
		for (it = imgs.begin(); it != imgs.end(); it++, imageCount++) {
			Image cur = *it;
			//iterate through the pixels
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				//assign the values to a vector index
				reds[pixelIndex][imageCount] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][imageCount] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][imageCount] = cur.pixels[pixelIndex].b;
			}
			//release memory used by the original image as it is no longer used
			cur.freeMemory();
		}

		cout << "Pixels Read.\n";
		cout << "Performing Sigma Clipped Mean...\n";
		// repeat the given number of times
		for (unsigned int iter = 0; iter < iterations; iter++) {
			//iterate through the pixels, in parallel
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				//sort the relevant vector
				sort(reds[pixelIndex].begin(), reds[pixelIndex].end());
				sort(greens[pixelIndex].begin(), greens[pixelIndex].end());
				sort(blues[pixelIndex].begin(), blues[pixelIndex].end());

				//calculate the median for each channel
				const unsigned char redMedian = reds[pixelIndex][(int)ceil((reds[pixelIndex].size()-1) / 2)];
				const unsigned char greenMedian = greens[pixelIndex][(int)ceil((greens[pixelIndex].size()-1) / 2)];
				const unsigned char blueMedian = blues[pixelIndex][(int)ceil((blues[pixelIndex].size()-1) / 2)];

				//calculate the standard deviation for each channel
				const float redStandardDev = calculateStandardDeviation(reds[pixelIndex], reds[pixelIndex].size());
				const float greenStandardDev = calculateStandardDeviation(greens[pixelIndex], greens[pixelIndex].size());
				const float blueStandardDev = calculateStandardDeviation(blues[pixelIndex], blues[pixelIndex].size());

				//calculate the lower and upper bounds for each channel
				const float redMin = redMedian - (alphaValue*redStandardDev);
				const float redMax = redMedian + (alphaValue*redStandardDev);

				const float greenMin = greenMedian - (alphaValue*greenStandardDev);
				const float greenMax = greenMedian + (alphaValue*greenStandardDev);

				const float blueMin = blueMedian - (alphaValue*blueStandardDev);
				const float blueMax = blueMedian + (alphaValue*blueStandardDev);

				//remove any values outside of the bounds for each channel
				for (unsigned int i = 0; i < reds[pixelIndex].size(); i++) {
					const unsigned char redVal = reds[pixelIndex][i];
					if (redVal < redMin || redVal > redMax) {
						remove(reds[pixelIndex], i);
					}
				}

				for (unsigned int i = 0; i < greens[pixelIndex].size(); i++) {
					const unsigned char greenVal = greens[pixelIndex][i];
					if (greenVal < greenMin || greenVal > greenMax) {
						remove(greens[pixelIndex], i);
					}
				}

				for (unsigned int i = 0; i < blues[pixelIndex].size(); i++) {
					const unsigned char blueVal = blues[pixelIndex][i];
					if (blueVal < blueMin || blueVal > blueMax) {
						remove(blues[pixelIndex], i);
					}
				}

				//calculate the mean of remaining values and assign to output
				output->pixels[pixelIndex].r = (unsigned char)calculateMean(reds[pixelIndex], reds[pixelIndex].size());
				output->pixels[pixelIndex].g = (unsigned char)calculateMean(greens[pixelIndex], greens[pixelIndex].size());
				output->pixels[pixelIndex].b = (unsigned char)calculateMean(blues[pixelIndex], blues[pixelIndex].size());
			}
		}
		output->updateModified();
		return *output;
	}


private:
	/// <summary>
	/// remove a element from a vector
	/// does not preserve sorted order but is faster than .erase()
	/// </summary>
	/// <param name="set">vector to remove from</param>
	/// <param name="index">index of element to remove</param>
	static void remove(vector<unsigned char> &set, const size_t &index) {
		if (set.size() > 0 && index > 0 && index < set.size()) {
			//overwrite the element to remove with the element at the back of the vector
			set[index] = set.back();
			//pop the element at the back off
			set.pop_back();
		}
	}

	/// <summary>
	/// Calculate the mean of a set of values
	/// </summary>
	/// <param name="values">vector of values</param>
	/// <param name="n">size of vector</param>
	/// <returns>mean of values</returns>
	static float calculateMean(const vector<unsigned char> &values, const size_t &n) {
		float sum = 0.0;
		for (size_t i = size_t(0); i < n; ++i) {
			sum += values[i];
		}
		return sum / n;
	}

	/// <summary>
	/// Calculate the standard deviation of a set of values
	/// </summary>
	/// <param name="values">values to perform calculation on</param>
	/// <param name="n">size of the values vector</param>
	/// <returns>Standard deviation of values</returns>
	static float calculateStandardDeviation(const vector<unsigned char> &values, const size_t &n) {
		float sum = 0.0, mean, standardDeviation = 0.0;

		//calculate mean
		mean = calculateMean(values, n);

		//calculate standard deviation
		for (size_t i = size_t(0); i < n; ++i) {
			standardDeviation += pow(values[i] - mean, 2);
		}

		return sqrt(standardDeviation / n);
	}
};

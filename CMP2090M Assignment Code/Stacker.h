#pragma once
#include <vector>
#include <ppl.h>
#include <math.h>
#include <stdexcept>
using namespace std;
using namespace Concurrency;

class Stacker {
public:
	static StackedImage MeanBlend(vector<Image> &imgs) {
		const unsigned int imageNumber = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Mean Blend");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		//iterate through images
		unsigned char imageCount = 1;
		for (it = imgs.begin(); it != imgs.end(); it++, imageCount++) {
			Image cur = *it;
			//add pixel values to output image
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				Image::Rgb curRgb = cur.pixels[pixelIndex];
				output->pixels[pixelIndex].r += (curRgb.r - output->pixels[pixelIndex].r) / imageCount;
				output->pixels[pixelIndex].g += (curRgb.g - output->pixels[pixelIndex].g) / imageCount;
				output->pixels[pixelIndex].b += (curRgb.b - output->pixels[pixelIndex].b) / imageCount;
			}
			cur.freeMemory();
		}

		output->updateModified();
		return *output;
	}

	static StackedImage MeanBlendParallel(vector<Image> &imgs) {
		const unsigned int imageNum = (unsigned int)imgs.size();
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Mean Blend");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;

		parallel_for(size_t(0), imgs.size(), [&imgs, &imageSize, &output](size_t i) {
			Image cur = imgs.at(i);
			//add pixel values to output image
			const unsigned char currentImage = (unsigned char)i + 1;
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				Image::Rgb curRgb = cur.pixels[pixelIndex];
				output->pixels[pixelIndex].r += (curRgb.r - output->pixels[pixelIndex].r) / currentImage;
				output->pixels[pixelIndex].g += (curRgb.g - output->pixels[pixelIndex].g) / currentImage;
				output->pixels[pixelIndex].b += (curRgb.b - output->pixels[pixelIndex].b) / currentImage;
			}
			cur.freeMemory();
		});

		output->updateModified();
		return *output;
	}

	static StackedImage MedianBlendParallel(vector<Image> &imgs) {
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Median Blend");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		unsigned char** reds = new unsigned char*[imageSize];
		unsigned char** greens = new unsigned char*[imageSize];
		unsigned char** blues = new unsigned char*[imageSize];
		for (unsigned int i = 0; i < imageSize; i++) {
			reds[i] = new unsigned char[imageNum];
			greens[i] = new unsigned char[imageNum];
			blues[i] = new unsigned char[imageNum];
		}

		parallel_for(size_t(0), imgs.size(), [&imgs, &imageSize, &output, &reds, &greens, &blues](size_t i) {
			Image cur = imgs.at(i);
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				reds[pixelIndex][i] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][i] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][i] = cur.pixels[pixelIndex].b;
			}
			cur.freeMemory();
		});

		parallel_for(size_t(0), size_t(imageSize), [&reds, &greens, &blues, &output, &imageNum](size_t i) {
			sort(reds[i], reds[i] + imageNum);
			sort(greens[i], greens[i] + imageNum);
			sort(blues[i], blues[i] + imageNum);

			output->pixels[i].r = reds[i][(int)ceil(imageNum-1 / 2)];
			output->pixels[i].g = greens[i][(int)ceil(imageNum-1 / 2)];
			output->pixels[i].b = blues[i][(int)ceil(imageNum-1 / 2)];
			delete reds[i];
			delete greens[i];
			delete blues[i];
		});
		delete[] reds;
		delete[] greens;
		delete[] blues;
		output->updateModified();
		return *output;
	}

	static StackedImage MedianBlend(vector<Image> &imgs) {
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Median Blend");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		unsigned char** reds = new unsigned char*[imageSize];
		unsigned char** greens = new unsigned char*[imageSize];
		unsigned char** blues = new unsigned char*[imageSize];

		unsigned int imgCount = 0;
		for (it = imgs.begin(); it != imgs.end(); it++, imgCount++) {
			Image cur = *it;
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {

				if (it == imgs.begin()) {
					reds[pixelIndex] = new unsigned char[imageNum];
					greens[pixelIndex] = new unsigned char[imageNum];
					blues[pixelIndex] = new unsigned char[imageNum];
				}
				reds[pixelIndex][imgCount] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][imgCount] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][imgCount] = cur.pixels[pixelIndex].b;
			}
			cur.freeMemory();
		}

		for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
			sort(reds[pixelIndex], reds[pixelIndex] + imageNum);
			sort(greens[pixelIndex], greens[pixelIndex] + imageNum);
			sort(blues[pixelIndex], blues[pixelIndex] + imageNum);

			output->pixels[pixelIndex].r = reds[pixelIndex][(int)ceil(imageNum-1 / 2)];
			output->pixels[pixelIndex].g = greens[pixelIndex][(int)ceil(imageNum-1 / 2)];
			output->pixels[pixelIndex].b = blues[pixelIndex][(int)ceil(imageNum-1 / 2)];
			delete reds[pixelIndex];
			delete greens[pixelIndex];
			delete blues[pixelIndex];
		}

		delete[] reds;
		delete[] greens;
		delete[] blues;
		output->updateModified();
		return *output;
	}


	static StackedImage SigmaClippedMeanBlendParallel(vector<Image> &imgs, const unsigned int &iterations, const float &alphaValue = 0.5) {
		if (iterations < 1) {
			throw new invalid_argument("The number of iterations cannot be less than 1!");
		}
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		cout << "Allocating Memory...\n";

		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Sigma Clipped Mean");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		vector<vector<unsigned char>> reds(imageSize);
		vector<vector<unsigned char>> greens(imageSize);
		vector<vector<unsigned char>> blues(imageSize);

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

		parallel_for(size_t(0), imgs.size(), [&imageSize, &reds, &greens, &blues, &imgs](size_t i) {
			Image cur = imgs[i];
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				reds[pixelIndex][i] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][i] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][i] = cur.pixels[pixelIndex].b;
			}
			cur.freeMemory();
		});

		cout << "Pixels Read.\n";
		cout << "Performing Sigma Clipped Mean...\n";
		for (unsigned int iter = 0; iter < iterations; iter++) {
			parallel_for(size_t(0), size_t(imageSize), [&reds, &greens, &blues, &output, &alphaValue](size_t pixelIndex) {
				sort(reds[pixelIndex].begin(), reds[pixelIndex].end());
				sort(greens[pixelIndex].begin(), greens[pixelIndex].end());
				sort(blues[pixelIndex].begin(), blues[pixelIndex].end());

				const unsigned char redMedian = reds[pixelIndex][(int)ceil(reds[pixelIndex].size()-1 / 2)];
				const unsigned char greenMedian = greens[pixelIndex][(int)ceil(greens[pixelIndex].size()-1 / 2)];
				const unsigned char blueMedian = blues[pixelIndex][(int)ceil(blues[pixelIndex].size()-1 / 2)];

				const float redStandardDev = calculateStandardDeviation(reds[pixelIndex], reds[pixelIndex].size());
				const float greenStandardDev = calculateStandardDeviation(greens[pixelIndex], greens[pixelIndex].size());
				const float blueStandardDev = calculateStandardDeviation(blues[pixelIndex], blues[pixelIndex].size());

				const float redMin = redMedian - (alphaValue*redStandardDev);
				const float redMax = redMedian + (alphaValue*redStandardDev);

				const float greenMin = greenMedian - (alphaValue*greenStandardDev);
				const float greenMax = greenMedian + (alphaValue*greenStandardDev);

				const float blueMin = blueMedian - (alphaValue*blueStandardDev);
				const float blueMax = blueMedian + (alphaValue*blueStandardDev);

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

				output->pixels[pixelIndex].r = (unsigned char)calculateMean(reds[pixelIndex], reds[pixelIndex].size());
				output->pixels[pixelIndex].g = (unsigned char)calculateMean(greens[pixelIndex], greens[pixelIndex].size());
				output->pixels[pixelIndex].b = (unsigned char)calculateMean(blues[pixelIndex], blues[pixelIndex].size());
			});
		}
		output->updateModified();
		return *output;
	}

	static StackedImage SigmaClippedMeanBlend(vector<Image> &imgs, const unsigned int &iterations, const float &alphaValue = 0.5) {
		if (iterations < 1) {
			throw new invalid_argument("The number of iterations cannot be less than 1!");
		}
		const unsigned int imageNum = (unsigned int)imgs.size();
		vector<Image>::const_iterator it;
		cout << "Allocating Memory...\n";

		StackedImage *output = new StackedImage(imgs.at(0).w, imgs.at(0).h, "Sigma Clipped Mean");
		output->setColourDepth(imgs[0].getColourDepth());
		const unsigned int imageSize = output->h * output->w;
		vector<vector<unsigned char>> reds(imageSize);
		vector<vector<unsigned char>> greens(imageSize);
		vector<vector<unsigned char>> blues(imageSize);

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
		for (it = imgs.begin(); it != imgs.end(); it++, imageCount++) {
			Image cur = *it;
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				reds[pixelIndex][imageCount] = cur.pixels[pixelIndex].r;
				greens[pixelIndex][imageCount] = cur.pixels[pixelIndex].g;
				blues[pixelIndex][imageCount] = cur.pixels[pixelIndex].b;

			}
			cur.freeMemory();
		}
		cout << "Pixels Read.\n";
		cout << "Performing Sigma Clipped Mean...\n";
		for (unsigned int iter = 0; iter < iterations; iter++) {
			for (unsigned int pixelIndex = 0; pixelIndex < imageSize; pixelIndex++) {
				sort(reds[pixelIndex].begin(), reds[pixelIndex].end());
				sort(greens[pixelIndex].begin(), greens[pixelIndex].end());
				sort(blues[pixelIndex].begin(), blues[pixelIndex].end());

				const unsigned char redMedian = reds[pixelIndex][(int)ceil(reds[pixelIndex].size()-1 / 2)];
				const unsigned char greenMedian = greens[pixelIndex][(int)ceil(greens[pixelIndex].size()-1 / 2)];
				const unsigned char blueMedian = blues[pixelIndex][(int)ceil(blues[pixelIndex].size()-1 / 2)];

				const float redStandardDev = calculateStandardDeviation(reds[pixelIndex], reds[pixelIndex].size());
				const float greenStandardDev = calculateStandardDeviation(greens[pixelIndex], greens[pixelIndex].size());
				const float blueStandardDev = calculateStandardDeviation(blues[pixelIndex], blues[pixelIndex].size());

				const float redMin = redMedian - (alphaValue*redStandardDev);
				const float redMax = redMedian + (alphaValue*redStandardDev);

				const float greenMin = greenMedian - (alphaValue*greenStandardDev);
				const float greenMax = greenMedian + (alphaValue*greenStandardDev);

				const float blueMin = blueMedian - (alphaValue*blueStandardDev);
				const float blueMax = blueMedian + (alphaValue*blueStandardDev);

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

				output->pixels[pixelIndex].r = (unsigned char)calculateMean(reds[pixelIndex], reds[pixelIndex].size());
				output->pixels[pixelIndex].g = (unsigned char)calculateMean(greens[pixelIndex], greens[pixelIndex].size());
				output->pixels[pixelIndex].b = (unsigned char)calculateMean(blues[pixelIndex], blues[pixelIndex].size());
			}
		}
		output->updateModified();
		return *output;
	}


private:
	//does not preserve sorted order but is faster than .erase()
	static void remove(vector<unsigned char> &set, const size_t &index) {
		if (set.size() > 0 && index > 0 && index < set.size()) {
			set[index] = set.back();
			set.pop_back();
		}
	}


	static float calculateMean(const vector<unsigned char> &values, const size_t &n) {
		float sum = 0.0;
		for (size_t i = size_t(0); i < n; ++i) {
			sum += values[i];
		}
		return sum / n;
	}

	static float calculateStandardDeviation(const vector<unsigned char> &values, const size_t &n) {
		float sum = 0.0, mean, standardDeviation = 0.0;

		mean = calculateMean(values, n);

		for (size_t i = size_t(0); i < n; ++i) {
			standardDeviation += pow(values[i] - mean, 2);
		}

		return sqrt(standardDeviation / n);
	}
};

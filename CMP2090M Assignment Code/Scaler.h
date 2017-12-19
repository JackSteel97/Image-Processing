#pragma once

/// <summary>
/// Class for scaling images
/// </summary>
class Scaler {
public:
	/// <summary>
	/// Nearest neighbour scaling, using all CPU cores
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>original image scaled by scale factor</returns>
	static ScaledImage NearestNeighbourParallel(Image &img, const double &scaleFactor) {
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Nearest Neighbour");
		//set colour depth 
		output->setColourDepth(img.getColourDepth());
		//ratios
		const float xRatio = img.w / (float)newW;
		const float yRatio = img.h / (float)newH;
		//parallel iteration through the rows
		parallel_for(size_t(0), size_t(newH), [&newW, &img, &xRatio, &yRatio, &output](size_t i) {
			float px, py;
			//serial iteration through columns
			//the overhead incurred by making this loop parallel outweighs the speed up
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates on original image
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				//set pixel on output image
				output->pixels[(i*newW) + j] = img.pixels[(int)((py*img.w) + px)];
			}
		});
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Nearest neighbour scaling
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>original image scaled by scale factor</returns>
	static ScaledImage NearestNeighbour(Image &img, const double &scaleFactor) {	
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Nearest Neighbour");
		//set colour depth 
		output->setColourDepth(img.getColourDepth());
		//ratios
		const float xRatio = img.w / (float)newW;
		const float yRatio = img.h / (float)newH;
		float px, py;
		//iterate through rows
		for (unsigned int i = 0; i < newH; i++) {
			//iterate through columns
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates on original image
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				//set pixel on output image
				output->pixels[(i*newW) + j] = img.pixels[(int)((py*img.w) + px)];
			}
		}
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Bilinear scale an image, using all CPU cores
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>original image scaled by scale factor</returns>
	static ScaledImage BilinearParallel(Image &img, const double &scaleFactor) {
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bilinear");
		//get colour depth
		output->setColourDepth(img.getColourDepth());
		//calculate ratios
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;

		//iterate through the rows in parallel
		parallel_for(size_t(0), size_t(newH), [&img, &newW, &xRatio,&yRatio, &output](size_t i) {
			float px, py, diffX, diffY;
			Image::Rgb a, b, c, d;
			//serial iteration through columns
			//the overhead incurred by making this loop parallel outweighs the speed up
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates on original image
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				
				//get the fractional part left over when calculating original coordinates
				diffX = (xRatio*j) - px;
				diffY = (yRatio*i) - py;
				//get index of pixel on original image
				const unsigned int index = (unsigned int)((py*img.w) + px);
				// get 2x2 grid of pixels from original image to interpolate
				a = img.pixels[index];
				b = img.pixels[index + 1];
				c = img.pixels[index + img.w];
				d = img.pixels[index + img.w + 1];

				//interpolate for each channel
				//red
				output->pixels[(i*newW) + j].r = (unsigned char)BilinearInterpolate(a.r, b.r, c.r, d.r, diffX, diffY);
				//green
				output->pixels[(i*newW) + j].g = (unsigned char)BilinearInterpolate(a.g, b.g, c.g, d.g, diffX, diffY);
				//blue
				output->pixels[(i*newW) + j].b = (unsigned char)BilinearInterpolate(a.b, b.b, c.b, d.b, diffX, diffY);
			}
		});

		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Bilinear scale an image
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>Original image scaled by scale factor</returns>
	static ScaledImage Bilinear(Image &img, const double &scaleFactor) {
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH,scaleFactor, "Bilinear");
		//set colour depth
		output->setColourDepth(img.getColourDepth());
		//calculate ratios
		const float xRatio = (img.w-1) / (float)newW;
		const float yRatio = (img.h-1) / (float)newH;
		float px, py,diffX,diffY;
		Image::Rgb a, b, c, d;
		//iterate through rows of scaled image
		for (unsigned int i = 0; i < newH; i++) {
			//iterate through columns of scaled image
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates on original image
				px = floor(j*xRatio);
				py = floor(i*yRatio);

				//get the fractional part left over when calculating original coordinates
				diffX = (xRatio*j) - px;
				diffY = (yRatio*i) - py;
				//get index of pixel on original image
				const unsigned int index = (unsigned int)((py*img.w) + px);
				// get 2x2 grid of pixels from original image to interpolate
				a = img.pixels[index];
				b = img.pixels[index + 1];
				c = img.pixels[index + img.w];
				d = img.pixels[index + img.w + 1];

				//interpolate for each channel
				//red
				output->pixels[(i*newW) + j].r = (unsigned char)BilinearInterpolate(a.r, b.r, c.r, d.r, diffX, diffY);
				//green
				output->pixels[(i*newW) + j].g = (unsigned char)BilinearInterpolate(a.g, b.g, c.g, d.g, diffX, diffY);
				//blue
				output->pixels[(i*newW) + j].b = (unsigned char)BilinearInterpolate(a.b, b.b, c.b, d.b, diffX, diffY);
			}
		}
		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Get a pixel from clamping x and y coordinates
	/// </summary>
	/// <param name="src">image to get pixel from</param>
	/// <param name="x">x coordinate</param>
	/// <param name="y">y coordinate</param>
	/// <returns>Closest pixel to (x,y) that is in range</returns>
	static Image::Rgb getPixel(const Image &src, const int &x, const int &y) {
		//clamp values
		unsigned int xP, yP;
		xP = (unsigned int)Clamp((float)x, (float)0, (float)src.w-1);
		yP = (unsigned int)Clamp((float)y, (float)0, (float)src.h-1);
		//get pixel from clamped values
		return src.pixels[(yP * src.w) + xP];
	}

	/// <summary>
	/// Bicubic scale an image, using all CPU cores
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>Original image scaled by scale factor</returns>
	static ScaledImage BiCubicParallel(Image &img, const double &scaleFactor) {
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bicubic");
		//set colour depth
		output->setColourDepth(img.getColourDepth());
		//calculate ratios
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;
		//iterate through rows in parallel
		parallel_for(size_t(0), size_t(newH), [&newW, &xRatio, &yRatio, &img, &output](size_t i) {
			float ax, ay, xfract, yfract;
			unsigned int px, py;
			Image::Rgb p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16;
			//iterate through columns in serial
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates of original image
				ax = j*xRatio;
				ay = i*yRatio;

				//floor coordinates to get integer indexes
				px = (unsigned int)floor(ax);
				py = (unsigned int)floor(ay);

				//get leftover fractional values from flooring
				xfract = ax - px;
				yfract = ay - py;

				//4x4 grid of pixels required for bicubic interpolation
				//1st row
				p1 = getPixel(img, px - 1, py - 1);
				p2 = getPixel(img, px, py - 1);
				p3 = getPixel(img, px + 1, py - 1);
				p4 = getPixel(img, px + 2, py - 1);
				//2nd row
				p5 = getPixel(img, px - 1, py);
				p6 = getPixel(img, px, py);
				p7 = getPixel(img, px + 1, py);
				p8 = getPixel(img, px + 2, py);
				//3rd row
				p9 = getPixel(img, px - 1, py + 1);
				p10 = getPixel(img, px, py + 1);
				p11 = getPixel(img, px + 1, py + 1);
				p12 = getPixel(img, px + 2, py + 1);
				//4th row
				p13 = getPixel(img, px - 1, py + 2);
				p14 = getPixel(img, px, py + 2);
				p15 = getPixel(img, px + 1, py + 2);
				p16 = getPixel(img, px + 2, py + 2);

				//interpolate rows, clamp values between 0 and 255 to avoid overflow when assigning to image
				//reds
				float Ar = Clamp(cubicInterpolate(p1.r, p2.r, p3.r, p4.r, xfract),0,255);
				float Br = Clamp(cubicInterpolate(p5.r, p6.r, p7.r, p8.r, xfract), 0, 255);
				float Cr = Clamp(cubicInterpolate(p9.r, p10.r, p11.r, p12.r, xfract), 0, 255);
				float Dr = Clamp(cubicInterpolate(p13.r, p14.r, p15.r, p16.r, xfract), 0, 255);

				//greens
				float Ag = Clamp(cubicInterpolate(p1.g, p2.g, p3.g, p4.g, xfract), 0, 255);
				float Bg = Clamp(cubicInterpolate(p5.g, p6.g, p7.g, p8.g, xfract), 0, 255);
				float Cg = Clamp(cubicInterpolate(p9.g, p10.g, p11.g, p12.g, xfract), 0, 255);
				float Dg = Clamp(cubicInterpolate(p13.g, p14.g, p15.g, p16.g, xfract), 0, 255);

				//blues
				float Ab = Clamp(cubicInterpolate(p1.b, p2.b, p3.b, p4.b, xfract), 0, 255);
				float Bb = Clamp(cubicInterpolate(p5.b, p6.b, p7.b, p8.b, xfract), 0, 255);
				float Cb = Clamp(cubicInterpolate(p9.b, p10.b, p11.b, p12.b, xfract), 0, 255);
				float Db = Clamp(cubicInterpolate(p13.b, p14.b, p15.b, p16.b, xfract), 0, 255);

				//interpolate in the y direction on each channel, clamp result between 0 and 255 again.
				//red
				output->pixels[(i*newW) + j].r = (unsigned char)Clamp(cubicInterpolate(Ar, Br, Cr, Dr, yfract), 0, 255);
				//green
				output->pixels[(i*newW) + j].g = (unsigned char)Clamp(cubicInterpolate(Ag, Bg, Cg, Dg, yfract), 0, 255);
				//blue
				output->pixels[(i*newW) + j].b = (unsigned char)Clamp(cubicInterpolate(Ab, Bb, Cb, Db, yfract), 0, 255);
			}
		});

		output->updateModified();
		return *output;
	}

	/// <summary>
	/// Bicubic scale an image
	/// </summary>
	/// <param name="img">image to scale</param>
	/// <param name="scaleFactor">scale multiplier</param>
	/// <returns>Original image scaled by scale factor</returns>
	static ScaledImage BiCubic(Image &img, const double &scaleFactor) {
		//height of scaled image
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		//width of scaled image
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		//declare output image
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bicubic");
		//set colour depth
		output->setColourDepth(img.getColourDepth());
		//calculate ratios
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;
	
		float ax, ay, xfract, yfract;
		unsigned int px, py;
		Image::Rgb p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16;
		//iterate through the rows
		for (unsigned int i = 0; i < newH; i++) {
			//iterate through the 
			for (unsigned int j = 0; j < newW; j++) {
				//get pixel coordinates of original image
				ax = j * xRatio;
				ay = i * yRatio;

				//floor coordinates to get integer indexes
				px = (unsigned int)floor(ax);
				py = (unsigned int)floor(ay);

				//get leftover fractional values from flooring
				xfract = ax - px;
				yfract = ay - py;

				//4x4 grid of pixels required for bicubic interpolation
				//1st row
				p1 = getPixel(img, px-1, py-1);
				p2 = getPixel(img, px, py-1);
				p3 = getPixel(img, px+1, py-1);
				p4 = getPixel(img, px+2,py-1);

				//2nd row
				p5 = getPixel(img,px-1, py);
				p6 = getPixel(img,px,py);
				p7 = getPixel(img,px+1,py);
				p8 = getPixel(img,px+2,py);

				//3rd row
				p9 = getPixel(img,px-1,py+1);
				p10 = getPixel(img,px,py+1);
				p11 = getPixel(img,px+1,py+1);
				p12 = getPixel(img,px+2,py+1);

				//4th row
				p13 = getPixel(img,px-1,py+2);
				p14 = getPixel(img,px,py+2);
				p15 = getPixel(img,px+1,py+2);
				p16 = getPixel(img,px+2,py+2);

				//interpolate rows, clamp values between 0 and 255 to avoid overflow when assigning to image
				//reds
				float Ar = Clamp(cubicInterpolate(p1.r, p2.r, p3.r, p4.r, xfract), 0, 255);
				float Br = Clamp(cubicInterpolate(p5.r, p6.r, p7.r, p8.r, xfract), 0, 255);
				float Cr = Clamp(cubicInterpolate(p9.r, p10.r, p11.r, p12.r, xfract), 0, 255);
				float Dr = Clamp(cubicInterpolate(p13.r, p14.r, p15.r, p16.r, xfract), 0, 255);

				//greens
				float Ag = Clamp(cubicInterpolate(p1.g, p2.g, p3.g, p4.g, xfract), 0, 255);
				float Bg = Clamp(cubicInterpolate(p5.g, p6.g, p7.g, p8.g, xfract), 0, 255);
				float Cg = Clamp(cubicInterpolate(p9.g, p10.g, p11.g, p12.g, xfract), 0, 255);
				float Dg = Clamp(cubicInterpolate(p13.g, p14.g, p15.g, p16.g, xfract), 0, 255);

				//blues
				float Ab = Clamp(cubicInterpolate(p1.b, p2.b, p3.b, p4.b, xfract), 0, 255);
				float Bb = Clamp(cubicInterpolate(p5.b, p6.b, p7.b, p8.b, xfract), 0, 255);
				float Cb = Clamp(cubicInterpolate(p9.b, p10.b, p11.b, p12.b, xfract), 0, 255);
				float Db = Clamp(cubicInterpolate(p13.b, p14.b, p15.b, p16.b, xfract), 0, 255);

				//interpolate in the y direction on each channel, clamp result between 0 and 255 again.
				//red
				output->pixels[(i*newW) + j].r = (unsigned char)Clamp(cubicInterpolate(Ar, Br, Cr, Dr, yfract),0,255);
				//green
				output->pixels[(i*newW) + j].g = (unsigned char)Clamp(cubicInterpolate(Ag, Bg, Cg, Dg, yfract),0,255);
				//blue
				output->pixels[(i*newW) + j].b = (unsigned char)Clamp(cubicInterpolate(Ab, Bb, Cb, Db, yfract),0,255);
			}
		}
		output->updateModified();
		return *output;
	}


	/// <summary>
	/// Extract a region of interest from a given image
	/// </summary>
	/// <param name="img">original image</param>
	/// <param name="left">top left x coordinate of ROI</param>
	/// <param name="top">top left y coordinate of ROI</param>
	/// <param name="width">width of ROI</param>
	/// <param name="height">height of ROI</param>
	/// <returns>Extracted region of interest as new image</returns>
	static Image ExtractRegionOfInterest(Image &img, const unsigned int &left, const unsigned int &top, const unsigned int &width, const unsigned int &height) {
		//declare output image
		Image *output = new Image(width, height);
		//set colour depth
		output->setColourDepth(img.getColourDepth());
		cout << "\nExtracting ROI...\n";
		//calculate width coordinate on original image
		const unsigned int newWidth = width + left;
		//calculate height coordinate on original image
		const unsigned int newHeight = height + top;
		//ensure ROI is within bounds of original image
		if (left >= img.w || top >= img.h || newWidth > img.w || newHeight > img.h) {
			return *output;
		}
		unsigned int outCount = 0;
		//iterate through rows
		for (unsigned int y = top; y < newHeight; y++) {
			//iterate through columns
			for (unsigned int x = left; x < newWidth; x++) {
				//add pixel from original image to output image
				output->pixels[outCount] = img.pixels[(y*img.h) + x];
				outCount++;
			}
		}
		cout << "ROI Extracted.\n";
		output->updateModified();
		return *output;
	}

private:
	/// <summary>
	/// Bilinear interpolate values
	/// </summary>
	/// <param name="A">Value 1</param>
	/// <param name="B">Value 2</param>
	/// <param name="C">Value 3</param>
	/// <param name="D">Value 4</param>
	/// <param name="w">fractional value in x direction</param>
	/// <param name="h">fraction value in y direction</param>
	/// <returns>Interpolated value</returns>
	static float BilinearInterpolate(float A, float B, float C, float D, float w, float h) {
		return A*(1 - w)*(1 - h) + B*w*(1 - h) + C*h*(1 - w) + D*w*h;
	}

	/// <summary>
	/// Cubic interpolate values
	/// Formula source: http://www.paulinternet.nl/?page=bicubic
	/// </summary>
	/// <param name="A">Value 1</param>
	/// <param name="B">Value 2</param>
	/// <param name="C">Value 3</param>
	/// <param name="D">Value 4</param>
	/// <param name="x">fractional multiplier value</param>
	/// <returns>Interpolated value</returns>
	static float cubicInterpolate(float A, float B, float C, float D, float x) {
		return float(B + 0.5 * x*(C - A + x*(2.0*A - 5.0*B + 4.0*C - D + x*(3.0*(B - C) + D - A))));
	}
};
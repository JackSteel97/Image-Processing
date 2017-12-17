#pragma once

class Scaler {
public:
	static ScaledImage NearestNeighbourParallel(Image &img, const double &scaleFactor) {
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Nearest Neighbour");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = img.w / (float)newW;
		const float yRatio = img.h / (float)newH;
		float t = 0.f;
		parallel_for(size_t(0), size_t(newH), [&newW, &img, &xRatio, &yRatio, &output](size_t i) {
			float px, py;
			for (unsigned int j = 0; j < newW; j++) {
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				output->pixels[(i*newW) + j] = img.pixels[(int)((py*img.w) + px)];
			}
		});
		output->updateModified();
		return *output;
	}

	static ScaledImage NearestNeighbour(Image &img, const double &scaleFactor) {	
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Nearest Neighbour");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = img.w / (float)newW;
		const float yRatio = img.h / (float)newH;
		float px, py;
		for (unsigned int i = 0; i < newH; i++) {
			for (unsigned int j = 0; j < newW; j++) {
				px = floor(j*xRatio);
				py = floor(i*yRatio);
				output->pixels[(i*newW) + j] = img.pixels[(int)((py*img.w) + px)];
			}
		}
		output->updateModified();
		return *output;
	}

	static ScaledImage BilinearParallel(Image &img, const double &scaleFactor) {
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bilinear");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;

		parallel_for(size_t(0), size_t(newH), [&img, &newW, &xRatio,&yRatio, &output](size_t i) {
			float px, py, diffX, diffY;
			Image::Rgb a, b, c, d;
			for (int j = 0; j < newW; j++) {
				px = floor(j*xRatio);
				py = floor(i*yRatio);

				diffX = (xRatio*j) - px;
				diffY = (yRatio*i) - py;
				const unsigned int index = (unsigned int)((py*img.w) + px);
				a = img.pixels[index];
				b = img.pixels[index + 1];
				c = img.pixels[index + img.w];
				d = img.pixels[index + img.w + 1];

				//red
				output->pixels[(i*newW) + j].r = BilinearInterpolate(a.r, b.r, c.r, d.r, diffX, diffY);
				//green
				output->pixels[(i*newW) + j].g = BilinearInterpolate(a.g, b.g, c.g, d.g, diffX, diffY);
				//blue
				output->pixels[(i*newW) + j].b = BilinearInterpolate(a.b, b.b, c.b, d.b, diffX, diffY);
			}
		});

		output->updateModified();
		return *output;
	}

	static ScaledImage Bilinear(Image &img, const double &scaleFactor) {
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH,scaleFactor, "Bilinear");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = (img.w-1) / (float)newW;
		const float yRatio = (img.h-1) / (float)newH;
		float px, py,diffX,diffY;
		Image::Rgb a, b, c, d;
		for (int i = 0; i < newH; i++) {
			for (int j = 0; j < newW; j++) {
				px = floor(j*xRatio);
				py = floor(i*yRatio);

				diffX = (xRatio*j) - px;
				diffY = (yRatio*i) - py;
				const unsigned int index = (unsigned int)((py*img.w) + px);
				a = img.pixels[index];
				b = img.pixels[index + 1];
				c = img.pixels[index + img.w];
				d = img.pixels[index + img.w + 1];

				//red
				output->pixels[(i*newW) + j].r = BilinearInterpolate(a.r, b.r, c.r, d.r, diffX, diffY);
				//green
				output->pixels[(i*newW) + j].g = BilinearInterpolate(a.g, b.g, c.g, d.g, diffX, diffY);
				//blue
				output->pixels[(i*newW) + j].b = BilinearInterpolate(a.b, b.b, c.b, d.b, diffX, diffY);
			}
		}
		output->updateModified();
		return *output;
	}

	static Image::Rgb getPixel(const Image &src,const int &x,const int &y) {
		//clamp values
		unsigned int xP, yP;
		xP = Clamp(x, 0, src.w-1);
		yP = Clamp(y, 0, src.h-1);
		return src.pixels[(yP * src.w) + xP];
	}

	static ScaledImage BiCubicParallel(Image &img, const double &scaleFactor) {
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bicubic");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;

		parallel_for(size_t(0), size_t(newH), [&newW, &xRatio, &yRatio, &img, &output](size_t i) {
			float px, py, ax, ay, xfract, yfract;
			Image::Rgb p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16;
			for (int j = 0; j < newW; j++) {
				ax = j * xRatio;
				ay = i*yRatio;
				px = floor(ax);
				py = floor(ay);

				xfract = ax - px;
				yfract = ay - py;

				p1 = getPixel(img, px - 1, py - 1);
				p2 = getPixel(img, px, py - 1);
				p3 = getPixel(img, px + 1, py - 1);
				p4 = getPixel(img, px + 2, py - 1);

				p5 = getPixel(img, px - 1, py);
				p6 = getPixel(img, px, py);
				p7 = getPixel(img, px + 1, py);
				p8 = getPixel(img, px + 2, py);

				p9 = getPixel(img, px - 1, py + 1);
				p10 = getPixel(img, px, py + 1);
				p11 = getPixel(img, px + 1, py + 1);
				p12 = getPixel(img, px + 2, py + 1);

				p13 = getPixel(img, px - 1, py + 2);
				p14 = getPixel(img, px, py + 2);
				p15 = getPixel(img, px + 1, py + 2);
				p16 = getPixel(img, px + 2, py + 2);

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

				//red
				output->pixels[(i*newW) + j].r = Clamp(cubicInterpolate(Ar, Br, Cr, Dr, yfract), 0, 255);
				//green
				output->pixels[(i*newW) + j].g = Clamp(cubicInterpolate(Ag, Bg, Cg, Dg, yfract), 0, 255);
				//blue
				output->pixels[(i*newW) + j].b = Clamp(cubicInterpolate(Ab, Bb, Cb, Db, yfract), 0, 255);
			}

		});

		output->updateModified();
		return *output;
	}

	static ScaledImage BiCubic(Image &img, const double &scaleFactor) {
		const unsigned int newH = (unsigned int)floor(img.h * scaleFactor);
		const unsigned int newW = (unsigned int)floor(img.w * scaleFactor);
		ScaledImage *output = new ScaledImage(newW, newH, scaleFactor, "Bicubic");
		output->setColourDepth(img.getColourDepth());
		const float xRatio = (img.w - 1) / (float)newW;
		const float yRatio = (img.h - 1) / (float)newH;
	
		float px, py, ax, ay, xfract, yfract;
		Image::Rgb p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16;
		for (int i = 0; i < newH; i++) {
			for (int j = 0; j < newW; j++) {
				ax = j * xRatio;
				ay = i*yRatio;
				px = floor(ax);
				py = floor(ay);

				xfract = ax - px;
				yfract = ay - py;

				p1 = getPixel(img, px-1, py-1);
				p2 = getPixel(img, px, py-1);
				p3 = getPixel(img, px+1, py-1);
				p4 = getPixel(img, px+2,py-1);
				
				p5 = getPixel(img,px-1, py);
				p6 = getPixel(img,px,py);
				p7 = getPixel(img,px+1,py);
				p8 = getPixel(img,px+2,py);

				p9 = getPixel(img,px-1,py+1);
				p10 = getPixel(img,px,py+1);
				p11 = getPixel(img,px+1,py+1);
				p12 = getPixel(img,px+2,py+1);

				p13 = getPixel(img,px-1,py+2);
				p14 = getPixel(img,px,py+2);
				p15 = getPixel(img,px+1,py+2);
				p16 = getPixel(img,px+2,py+2);

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

				//red
				output->pixels[(i*newW) + j].r = Clamp(cubicInterpolate(Ar, Br, Cr, Dr, yfract),0,255);
				//green
				output->pixels[(i*newW) + j].g = Clamp(cubicInterpolate(Ag, Bg, Cg, Dg, yfract),0,255);
				//blue
				output->pixels[(i*newW) + j].b = Clamp(cubicInterpolate(Ab, Bb, Cb, Db, yfract),0,255);
			}
		}
		output->updateModified();
		return *output;
	}



	static Image ExtractRegionOfInterest(Image &img, const unsigned int &left, const unsigned int &top, const unsigned int &width, const unsigned int &height) {
		Image *output = new Image(width, height);
		output->setColourDepth(img.getColourDepth());
		cout << "\nExtracting ROI...\n";
		const unsigned int newWidth = width + left;
		const unsigned int newHeight = height + top;
		if (left >= img.w || top >= img.h || newWidth > img.w || newHeight > img.h) {
			return *output;
		}
		unsigned int outCount = 0;
		for (unsigned int y = top; y < newHeight; y++) {
			for (unsigned int x = left; x < newWidth; x++) {
				output->pixels[outCount] = img.pixels[(y*img.h) + x];
				outCount++;
			}
		}
		cout << "ROI Extracted.\n";
		output->updateModified();
		return *output;
	}

private:
	static float BilinearInterpolate(unsigned char A, unsigned char B, unsigned char C, unsigned char D, float w, float h) {
		return A*(1 - w)*(1 - h) + B*w*(1 - h) + C*h*(1 - w) + D*w*h;
	}

	static float cubicInterpolate(unsigned char A, unsigned char B, unsigned char C, unsigned char D, float x) {
		return B + 0.5 * x*(C - A + x*(2.0*A - 5.0*B + 4.0*C - D + x*(3.0*(B - C) + D - A)));
	}
};
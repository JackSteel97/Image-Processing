#pragma once
#include <math.h>
#include <sstream>

using namespace std;

std::stringstream bytesToAppropriate(const unsigned long numberOfBytes) {
	std::stringstream result;

	unsigned long const Gigabyte = (unsigned long)pow(2, 30);
	unsigned long const Megabyte = (unsigned long)pow(2, 20);
	unsigned long const Kilobyte = (unsigned long)pow(2, 10);

	if (numberOfBytes >= Gigabyte) {
		result << (double)numberOfBytes / Gigabyte << " GB";
	}
	else if (numberOfBytes >= Megabyte) {
		result << (double)numberOfBytes / Megabyte << " MB";

	}
	else if (numberOfBytes >= Kilobyte) {
		result << (double)numberOfBytes / Kilobyte << " KB";
	}
	else {
		result << numberOfBytes << " Bytes";
	}

	return result;
}

float Clamp(const float &val, const float &min, const float &max) {
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

void clearConsole() {
	cout << flush;
	system("CLS");
}


int getUserInputInteger() {
	string input = "";
	cin >> input;
	int choice = atoi(input.c_str());
	return choice;
}

double getUserInputDouble() {
	string input = "";
	cin >> input;
	double choice = atof(input.c_str());
	return choice;
}


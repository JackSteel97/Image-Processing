#pragma once
#include <math.h>
#include <sstream>

using namespace std;

/// <summary>
/// Convert a number of bytes to an appropriate value with suffix
/// </summary>
/// <param name="numberOfBytes">number of bytes to convert</param>
/// <returns>A string in the form "[Number] [Suffix]"</returns>
std::stringstream bytesToAppropriate(const unsigned long numberOfBytes) {
	std::stringstream result;

	//constants for number of bytes in each order of magnitude
	unsigned long const Gigabyte = (unsigned long)pow(2, 30);
	unsigned long const Megabyte = (unsigned long)pow(2, 20);
	unsigned long const Kilobyte = (unsigned long)pow(2, 10);

	//are there more bytes than one gigabyte?
	if (numberOfBytes >= Gigabyte) {
		//yes, we can express as an amount of GB
		result << (double)numberOfBytes / Gigabyte << " GB";
	} else if (numberOfBytes >= Megabyte) {
		//no, but there are enough to express as an amount of MB
		result << (double)numberOfBytes / Megabyte << " MB";
	} else if (numberOfBytes >= Kilobyte) {
		//no, but there are enough to express as an amount of KB
		result << (double)numberOfBytes / Kilobyte << " KB";
	} else {
		//no, just leave it as bytes
		result << numberOfBytes << " Bytes";
	}
	return result;
}

/// <summary>
/// Clamp a value between min and Max
/// </summary>
/// <param name="val">value to clamp</param>
/// <param name="min">minimum allowed value</param>
/// <param name="max">maximum allowed value</param>
/// <returns>min if value below min, Max if above Max, otherwise value</returns>
float Clamp(const float &val, const float &min, const float &max) {
	if (val < min) {
		return min;
	}
	if (val > max) {
		return max;
	}
	return val;
}

/// <summary>
/// Clear the console screen
/// </summary>
void clearConsole() {
	cout << flush;
	system("CLS");
}

/// <summary>
/// Get a integer from user input
/// </summary>
/// <returns>User's input as integer</returns>
int getUserInputInteger() {
	string input = "";
	cin >> input;
	int choice = atoi(input.c_str());
	return choice;
}

/// <summary>
/// Get a double from user input
/// </summary>
/// <returns>User's input as double</returns>
double getUserInputDouble() {
	string input = "";
	cin >> input;
	double choice = atof(input.c_str());
	return choice;
}


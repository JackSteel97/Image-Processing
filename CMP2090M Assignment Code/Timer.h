#pragma once
#include <ctime>

class Timer {
private:
	clock_t startT;
	clock_t endT;
public:
	void start() {
		startT = clock();
	}

	void stop() {
		endT = clock();
	}

	double getSeconds() {
		return (endT - startT) / static_cast<double>(CLOCKS_PER_SEC);
	}
};

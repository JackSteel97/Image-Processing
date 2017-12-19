#pragma once
#include <ctime>

/// <summary>
/// Class for timing durations
/// </summary>
class Timer {
private:
	clock_t startT;
	clock_t endT;
public:
	/// <summary>
	/// Start the stopwatch
	/// </summary>
	void start() {
		startT = clock();
	}

	/// <summary>
	/// Stop the stopwatch
	/// </summary>
	void stop() {
		endT = clock();
	}

	/// <summary>
	/// Get the duration
	/// </summary>
	/// <returns>Number of seconds between start and stop</returns>
	double getSeconds() {
		return (endT - startT) / static_cast<double>(CLOCKS_PER_SEC);
	}
};

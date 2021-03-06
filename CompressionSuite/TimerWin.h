// Copyright � 2006-2007 Glenn Fiedler
// Part of the PixelToaster Framebuffer Library - http://www.pixeltoaster.com
//
// Ripped out and removed abstract base class

#ifndef __TIMER_WIN_H__
#define __TIMER_WIN_H__

#include <windows.h>


class Timer
{
public:

	Timer()
	{
		QueryPerformanceFrequency( (LARGE_INTEGER*) &_frequency );
		reset();
	}

	void reset()
	{
		QueryPerformanceCounter( (LARGE_INTEGER*) &_timeCounter );
		_deltaCounter = _timeCounter;
		_time = 0.0;
	}

	double time()
	{
		__int64 counter;
		QueryPerformanceCounter( (LARGE_INTEGER*) &counter );
		_time += ( counter - _timeCounter ) / (double) _frequency;
		_timeCounter = counter;
		return _time;
	}

	double delta()
	{
		__int64 counter;
		QueryPerformanceCounter( (LARGE_INTEGER*) &counter );
		double delta = (counter - _deltaCounter) / (double) _frequency;
		_deltaCounter = counter;
		return delta;
	}

	double resolution()
	{
		return 1.0 / (double) _frequency;
	}

	void wait( double seconds )
	{
		Sleep( int(seconds*1000) );
	}

private:

	double _time;               ///< current time in seconds
	__int64 _timeCounter;       ///< raw 64bit timer counter for time
	__int64 _deltaCounter;      ///< raw 64bit timer counter for delta
	__int64 _frequency;         ///< raw 64bit timer frequency
};

#endif // __TIMER_WIN_H__

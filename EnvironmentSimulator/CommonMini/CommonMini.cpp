/* 
 * esmini - Environment Simulator Minimalistic 
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

#include <stdarg.h> 
#include <stdio.h>
#include <iostream>

#include "CommonMini.hpp"

#define DEBUG_TRACE
#define LOG_FILENAME "log.txt"



std::string CombineDirectoryPathAndFilepath(std::string dir_path, std::string file_path)
{
	std::string path = file_path;

	if (file_path[0] != '/')
	{
		// Relative path. Make sure it starts with ".." or "./"
		if (path[0] != '.')
		{
			path.insert(0, "./");
		}
		if (dir_path != "")
		{
			// Combine with directory path
			path.insert(0, dir_path + '/');
		}
	}

	return path;
}

double GetAbsAngleDifference(double angle1, double angle2)
{
	double diff = fmod(angle2 - angle1, 2 * M_PI);

	if (diff < 0)
	{
		diff += 2 * M_PI;
	}

	if (diff > M_PI)
	{
		diff = 2 * M_PI - diff;
	}

	return diff;
}

double GetAngleDifference(double angle1, double angle2)
{
	double diff = fmod(angle2 - angle1, 2 * M_PI);

	if (diff < 0)
	{
		diff += 2 * M_PI;
	}

	return diff;
}

double GetAngleSum(double angle1, double angle2)
{
	double sum = fmod(angle2 + angle1, 2 * M_PI);

	if (sum < 0)
	{
		sum += 2 * M_PI;
	}

	return sum;
}

int GetIntersectionOfTwoLineSegments(double ax1, double ay1, double ax2, double ay2, double bx1, double by1, double bx2, double by2, double &x3, double &y3)
{
	// Inspiration: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

	double t_demonitator = (ax1 - ax2) * (by1 - by2) - (ay1 - ay2) * (bx1 - bx2);
	
	if(fabs(t_demonitator) < SMALL_NUMBER)
	{
		return -1;
	}

	double t = ((ax1 - bx1) * (by1 - by2) - (ay1 - by1) * (bx1 - bx2)) / t_demonitator;

	x3 = ax1 + t * (ax2 - ax1);
	y3 = ay1 + t * (ay2 - ay1);

	return 0;
}

bool PointInBetweenVectorEndpoints(double x3, double y3, double x1, double y1, double x2, double y2, double &sNorm)
{
	bool inside;

	if (fabs(y2 - y1) < SMALL_NUMBER && fabs(x2 - x1) < SMALL_NUMBER)
	{
		// Point - not really a line 
		// Not sure if true of false should be returned
		sNorm = 0;
		inside = true;
	}
	else if (fabs(x2 - x1) < fabs(y2 - y1))  // Line is steep (more vertical than horizontal
	{
		sNorm = (y3 - y1) / (y2 - y1);
		if (y2 > y1)  // ascending
		{
			inside = !(y3 < y1 || y3 > y2);
		}
		else
		{
			inside = !(y3 > y1 || y3 < y2);
		}
	}
	else
	{
		sNorm = (x3 - x1) / (x2 - x1);
		if (x2 > x1)  // forward
		{
			inside = !(x3 < x1 || x3 > x2);
		}
		else
		{
			inside = !(x3 > x1 || x3 < x2);
		}
	}
	return inside;
}

int PointSideOfVec(double px, double py, double vx1, double vy1, double vx2, double vy2)
{
	// Use cross product
	return SIGN(GetCrossProduct2D((vx2 - vx1), (px - vx1), (vy2 - vy1), (py - vy1)));
}

double PointDistance2D(double x0, double y0, double x1, double y1)
{
	return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0) * (y1 - y0));
}

void ProjectPointOnVector2D(double x, double y, double vx1, double vy1, double vx2, double vy2, double &px, double &py)
{
	// Project the given point on the straight line between geometry end points
	// https://stackoverflow.com/questions/1811549/perpendicular-on-a-line-from-a-given-point

	double dx = vx2 - vx1;
	double dy = vy2 - vy1;

	if (fabs(dx) < SMALL_NUMBER && fabs(dy) < SMALL_NUMBER)
	{
		// Line too small - projection not possible, copy first point position
		px = vx1;
		py = vy1;
	}
	else
	{
		double k = (dy * (x - vx1) - dx * (y - vy1)) / (dy*dy + dx*dx);
		px = x - k * dy;
		py = y + k * dx;
	}
}

double GetLengthOfLine2D(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

double GetLengthOfVector3D(double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

void SwapByteOrder(unsigned char *buf, int data_type_size, int buf_size)
{
	unsigned char *ptr = buf;
	unsigned char tmp;

	if (data_type_size < 2)
	{
		// No need to swap for one byte data types
		return;
	}

	for (int i = 0; i < buf_size / data_type_size; i++)
	{
		for (int j = 0; j < data_type_size / 2; j++)
		{
			tmp = ptr[j];
			ptr[j] = ptr[data_type_size - j - 1];
			ptr[data_type_size - j - 1] = tmp;
		}
		ptr += data_type_size;
	}
}

#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)

	#include <windows.h>
	#include <process.h>

	__int64 SE_getSystemTime()
	{
		return timeGetTime();
	}

	void SE_sleep(unsigned int msec)
	{
		Sleep(msec);
	}

#else

	#include <chrono>

	using namespace std::chrono;

	__int64 SE_getSystemTime()
	{
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	void SE_sleep(unsigned int msec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(msec)));
	}

#endif

std::string DirNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : fname.substr(0, pos);
}

std::string FileNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : fname.substr(pos+1);
}

double GetCrossProduct2D(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}

double GetDotProduct2D(double x1, double y1, double x2, double y2)
{
	return x1 * x2 + y1 * y2;
}

Logger::Logger()
{
#ifndef SUPPRESS_LOG
	file_.open(LOG_FILENAME);
	if (file_.fail())
	{
		throw std::iostream::failure(std::string("Cannot open file: ") + LOG_FILENAME);
	}
#endif
	callback_ = 0;
}

Logger::~Logger()
{
	if (file_.is_open())
	{
		file_.close();
	}

	callback_ = 0;
}

void Logger::Log(char const* file, char const* func, int line, char const* format, ...)
{
	static char complete_entry[2048];
	static char message[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(message, 1024, format, args);

#ifdef DEBUG_TRACE
	snprintf(complete_entry, 2048, "%s / %d / %s(): %s", file, line, func, message);
#else
	strncpy(complete_entry, message, 1024);
#endif

	if (file_.is_open())
	{
		file_ << complete_entry << std::endl;
		file_.flush();
	}

	if (callback_)
	{
		callback_(complete_entry);
	}

	va_end(args);
}

Logger& Logger::Inst()
{
	static Logger instance;
	return instance;
}

SE_Thread::~SE_Thread()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)

#else
	if (thread_.joinable())
	{
		thread_.join();
	}
#endif
}

void SE_Thread::Start(void(*func_ptr)(void*), void *arg)
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	thread_ = (void*)_beginthread(func_ptr, 0, arg);
#else
	thread_ = std::thread(func_ptr, arg);
#endif
}


void SE_Thread::Wait()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	WaitForSingleObject((HANDLE)thread_, 1000);  // Should never need to wait for more than 1 sec
#else
	thread_.join();
#endif
}

SE_Mutex::SE_Mutex()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	mutex_ = (void*)CreateMutex(
		NULL,              // default security attributes
		0,             // initially not owned
		NULL);             // unnamed mutex

	if (mutex_ == NULL)
	{
		LOG("CreateMutex error: %d\n", GetLastError());
		mutex_ = 0;
	}
#else

#endif
}


void SE_Mutex::Lock()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	WaitForSingleObject(mutex_, 1000);  // Should never need to wait for more than 1 sec
#else
	mutex_.lock();
#endif
}

void SE_Mutex::Unlock()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	ReleaseMutex(mutex_);
#else
	mutex_.unlock();
#endif
}

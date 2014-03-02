#ifndef _IE_TIMER_H_
#define _IE_TIMER_H_

#ifdef _WIN32
    
    #pragma once
    #include <windows.h>
	#include <assert.h>

    class IETimer{
    public:
		IETimer(){
			LARGE_INTEGER li;
			BOOL re = QueryPerformanceFrequency(&li);
			assert(re);
			freq_ = double(li.QuadPart / LONGLONG(1000));
			start_();
		}
		
		~IETimer(){};
		
		void restart(){
			start_();
		}
		
		double elapsed() const {
			LARGE_INTEGER li;
			QueryPerformanceCounter(&li);
			return double(li.QuadPart - begin_) / freq_;
		}
	
    private:
		void start_(){
			LARGE_INTEGER li;
			BOOL re = QueryPerformanceCounter(&li);
			assert(re);
			begin_ = li.QuadPart;
		}

		double freq_;
		LONGLONG begin_;
    };

#else

    #include <time.h>
    #include <sys/time.h>

    class IETimer{
    public:
		IETimer(){
			start_();
		}

		~IETimer(){};

		void restart(){
			start_();
		}

		double elapsed() const {
			double d;
			struct timeval tv;
			gettimeofday(&tv, NULL);
			d = tv.tv_sec * 1e+3 + (double)tv.tv_usec * 1e-3;
			return d - begin_;
		}

    private:
		void start_(){
			struct timeval tv;
			gettimeofday(&tv, NULL);
			begin_ = tv.tv_sec * 1e+3 + (double)tv.tv_usec * 1e-3;
		}

		double begin_;
    };

#endif //_WIN32

#endif //_IE_TIMER_H_

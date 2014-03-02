#include "stdafx.h"

#include "OutputError.h"

#undef ERROR
#include <glog/logging.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

bool        OutputError::m_IsInited = false;
int         OutputError::m_LogBufSize;
LOG_LEVEL   OutputError::m_LogLevel;
std::string OutputError::m_LogDirPath;
LogList     OutputError::m_LogList;

void OutputError::Init(const char* exePath, const char* logDirPath, LOG_LEVEL log_level)
{
	if (m_IsInited == false) {
		FLAGS_log_dir = logDirPath;
		m_LogDirPath = logDirPath;

		SetLogLevel(log_level);

		google::InitGoogleLogging(exePath);
		m_IsInited = true;
	}
}

void OutputError::End()
{
	if (m_IsInited) {
		google::ShutdownGoogleLogging();
		m_IsInited = false;
	}
}


void OutputError::PushLog(LOG_LEVEL log_level, const char* str)
{
	if(m_IsInited){
		switch(log_level){
			case LOG_LEVEL::_INFO:
				LOG(INFO) << str;
				break;
			case LOG_LEVEL::_WARN:
				LOG(WARNING) << str;
				break;
			case LOG_LEVEL::_ERROR:
				LOG(ERROR) << str;
				break;
			case LOG_LEVEL::_FATAL:
				LOG(FATAL) << str;
				break;
			default:
				assert(0);
				break;
		}
	}
}

void OutputError::PushLog(
	LOG_LEVEL log_level,
	const char *str,
	const char *v1)
{
	std::string _s = str;
	_s += v1;
	PushLog(log_level, _s.c_str());
}

void OutputError::PushLog(
	LOG_LEVEL log_level,
	const char *str,
	int val)
{
	std::stringstream ss;
	ss << str << val;
	PushLog(log_level, ss.str().c_str());
}

//void OutputError::PushLog(const char* file, int line, LOG_LEVEL log_level, const char *str)
//{
//	if(m_IsSetLogFilePath){
//		if(log_level <= m_LogLevel){
//			char* log = (char*) malloc(strlen(str) + _MAX_PATH + 24);
//			switch(log_level){
//				case LOG_LEVEL::_ERROR:
//					sprintf(log, "%s(%d) error : %s\n", file, line, str);
//					break;
//				case LOG_LEVEL::_WARN:
//					sprintf(log, "%s(%d) warning : %s\n", file, line, str);
//					break;
//				case LOG_LEVEL::_INFO:
//					sprintf(log, "%s(%d) info : %s\n", str);
//					break;
//			}
//			
//			std::string buf = log;
//			m_LogList.push_back(buf);
//			free(log);
//
//			if(m_LogList.size() > m_LogBufSize){
//				FlushLog();
//			}
//		}
//	}
//}

//void OutputError::FlushLog()
//{
//	if(m_LogList.empty()) return;
//
//	if(m_IsSetLogFilePath){
//		FILE *fp;
//		if((fp = fopen(m_LogFilePath.c_str(), "a")) == NULL){
//		}
//
//		LogList::iterator itr = m_LogList.begin();
//		for(; itr != m_LogList.end(); itr++){
//			fputs((*itr).c_str(), fp);
//		}
//		fclose(fp);
//	}
//}

void OutputError::Alert(const char *str)
{
	PushLog(LOG_LEVEL::_ERROR, str);
	::MessageBox(NULL, str, "", MB_OK);
}

void OutputError::Alert(
	const char *str,
	const char *v1)
{
	std::string _s = str;
	_s += v1;
	Alert(_s.c_str());
}

void OutputError::SetLogLevel(LOG_LEVEL log_level)
{
	m_LogLevel = log_level;

	switch(log_level){
		case LOG_LEVEL::_INFO:
			FLAGS_minloglevel = 0;
			break;
		case LOG_LEVEL::_WARN:
			FLAGS_minloglevel = 1;
			break;
		case LOG_LEVEL::_ERROR:
			FLAGS_minloglevel = 2;
			break;
		case LOG_LEVEL::_FATAL:
			FLAGS_minloglevel = 3;
			break;
		default:
			assert(0);
			break;
	}
}
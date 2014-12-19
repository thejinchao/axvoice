#pragma once

/** get md5 code of file*/
std::string md5File(const char* szFileName);

struct AutoLock
{
	AutoLock(LPCRITICAL_SECTION _lock) : lock(_lock){ 
		::EnterCriticalSection(lock); 
	}

	~AutoLock() {
		::LeaveCriticalSection(lock);
	}

	LPCRITICAL_SECTION lock;
};

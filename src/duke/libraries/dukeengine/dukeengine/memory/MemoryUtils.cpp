/*
 * MemoryUtils.cpp
 *
 *  Created on: 2 avr. 2012
 *      Author: Guillaume Chatelet
 */

#ifdef _WIN32
#include <windows.h>
unsigned long long getTotalSystemMemory() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#elif defined __APPLE__
#include <stdint.h>
#include <sys/types.h>
#include <sys/sysctl.h>
unsigned long long getTotalSystemMemory()
{
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    u_int namelen = sizeof(mib) / sizeof(mib[0]);
    uint64_t size = 0;
    size_t len = sizeof(size);
    sysctl(mib, namelen, &size, &len, NULL, 0);
    return size;
}
#else
#include <unistd.h>
unsigned long long getTotalSystemMemory()
{
    const unsigned long long pages = sysconf(_SC_PHYS_PAGES);
    const unsigned long long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#endif

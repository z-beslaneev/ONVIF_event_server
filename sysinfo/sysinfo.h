#ifndef H_SYSINFO
#define H_SYSINFO

#include <stdexcept>

// all functions can throw std::runtime_error

std::string GetMac();

std::string GetMicroSDCID();

std::string GetCPUSerial();

#endif

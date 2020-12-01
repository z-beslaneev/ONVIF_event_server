#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>

namespace
{
	char* getFormattedTime(void)
	{
		// local
		std::time_t lt = std::time(0);
		std::tm* now = std::localtime(&lt);

		// Must be static, otherwise won't work
		static char _retval[20];
		strftime(_retval, sizeof(_retval), "%Y-%m-%d %H:%M:%S", now);

		return _retval;
	}
}


class AtomicWriter {
    std::ostringstream st;
    std::ostream &stream;
public:
    AtomicWriter(std::ostream &s=std::cerr):stream(s) { }
    template <typename T>
    AtomicWriter& operator<<(T const& t) {
        st << t;
        return *this;
    }
    AtomicWriter& operator<<( std::ostream&(*f)(std::ostream&) ) {
        st << f;
        return *this;
    }
	~AtomicWriter() { st << std::endl;  stream << st.str(); }
};

// Main log macro
#define __LOG__(logLevel)  AtomicWriter() << getFormattedTime() << " " << logLevel << " [" << __func__ << "] [ " << __FILE__ << ":" << __LINE__<< "] "

#define LOG_ERROR __LOG__("ERROR")
#define LOG_WARNING __LOG__("WARNING")
#define LOG_INFO __LOG__("INFO")

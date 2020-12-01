#ifndef H_MD5UTIL
#define H_MD5UTIL

#include <string>
#include <sstream>
#include <openssl/md5.h>

std::string StringToMD5(const std::string& input)
{
	unsigned char md5digest[MD5_DIGEST_LENGTH];

	auto charInp = reinterpret_cast<unsigned const char*>(input.c_str());
	
	MD5(charInp ,input.size(), md5digest);

	std::stringstream stream; 
	
	for (int index = 0; index < MD5_DIGEST_LENGTH; ++index) 
		// use u_int16_t because u_int8_t is typedef to unsigned char
		// char in stream inereted by symbol
		stream << std::hex << static_cast<std::uint16_t>(md5digest[index]);

	return stream.str();
}

#endif

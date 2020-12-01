//	soapdefs.h
//	Place this file in the same directory as stdsoap2.h
//	This file will be included in stdsoap2.h when compiling with
//	-DWITH_SOAPDEFS_H (see stdsoap2.h line 16)
//	See extras/logging.cpp for customer logging
//	
//	Runtime/Customer logging by Mike Helmick
//	Copyright (c) 2002 - Mike Helmick. Convergys IMG. All Rights Reserved.
//	This contributed code si covered under the MPL 1.1 license

#ifndef SOAPDEFS_H
#define SOAPDEFS_H

#include <assert.h>
#include <stdio.h>

#define ONVIF_LOG_BUFFER_SIZE 16384

#ifndef _MSC_VER
#	define sprintf_s snprintf
#endif

// This macro should be called only inside DBGLOG, therefore we have to free buffer
// in case of error.
#define SOAP_MESSAGE(buffer, ...) \
	if (sprintf_s(buffer, ONVIF_LOG_BUFFER_SIZE, __VA_ARGS__) <= 0) { assert(0); free(buffer); break; }

// Log only sent and received data. Also see implementation SoapWrapper.cpp.
#define DBGLOG(DBGFILE, CMD) \
	if (SOAP_INDEX_##DBGFILE != SOAP_INDEX_TEST) \
		do \
		{ \
			char* fdebug = (char*)malloc(ONVIF_LOG_BUFFER_SIZE); \
			if (!fdebug) { assert(0); break; } \
			CMD; \
			soapLogMessage(soap, SOAP_INDEX_##DBGFILE, fdebug, strlen(fdebug)); \
			free(fdebug); \
		} while(0)

#define DBGMSG(DBGFILE, MSG, LEN) soapLogMessage(soap, SOAP_INDEX_##DBGFILE, MSG, LEN);

struct soap;
#ifdef __cplusplus
extern "C"
#endif
void soapLogMessage(const struct soap*, int, const char*, size_t);

#endif

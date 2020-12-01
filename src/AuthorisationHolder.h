#ifndef ONVIF_AUTHORISATION_HOLDER_H
#define ONVIF_AUTHORISATION_HOLDER_H

#include <gSoap/wsseapi.h>

namespace Onvif
{

class AuthorisationHolder 
{
public:

	static AuthorisationHolder& getInstance();

	bool verifyPassword(struct soap *soap);
	
private:
	AuthorisationHolder() = default;
	~AuthorisationHolder() = default;
		
	AuthorisationHolder(AuthorisationHolder const&) = delete;
	AuthorisationHolder& operator= (AuthorisationHolder const&) = delete;

};
}

#endif


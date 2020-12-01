#include "AuthorisationHolder.h"

#include <gSoap/httpda.h>

#include <primitives/Config.h>

namespace Onvif
{

	Onvif::AuthorisationHolder& AuthorisationHolder::getInstance()
	{
		static AuthorisationHolder holder;

		return holder;
	}

bool AuthorisationHolder::verifyPassword(struct soap *soap)
{
	auto& config = Config::getInstance();
	static const std::string authrealm = config.getOption("authrealm");
	const std::string hash = config.getOption("hash");

	int noncelen;
	const char *nonce;
	nonce = soap_base642s(soap, hash.c_str(), NULL, 0, &noncelen);
	const std::string passwd(nonce, noncelen);

	const std::string userid = config.getOption("userid");

	if (soap->authrealm && soap->userid && authrealm == soap->authrealm && userid == soap->userid
		&& http_da_verify_post(soap, passwd.c_str()) == SOAP_OK)
	{
		soap_wsse_delete_Security(soap);
		return true;
	}

	if (soap->header && soap->header->wsse__Security)
	{
		const char *username = soap_wsse_get_Username(soap);

		if (username && username == userid && soap_wsse_verify_Password(soap, passwd.c_str()) == SOAP_OK)
		{
			soap_wsse_delete_Security(soap);
			return true;
		}
	}

	soap->authrealm = authrealm.c_str();
	soap_wsse_delete_Security(soap);
	return false;
}

}
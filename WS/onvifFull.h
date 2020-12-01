// SOAP HEADER

//gsoapopt w

//gsoap wsd service method-header-part:   Probe wsa__MessageID
//gsoap wsd service method-header-part:   Probe wsa__To
//gsoap wsd service method-header-part:   Probe wsa__Action
//gsoap wsd service method-action:        Probe http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/Probe
int __wsd__Probe (struct wsd__ProbeType *wsd__Probe, void);

//gsoap wsd service method-header-part:   ProbeMatches wsa__MessageID
//gsoap wsd service method-header-part:   ProbeMatches wsa__RelatesTo
//gsoap wsd service method-header-part:   ProbeMatches wsa__To
//gsoap wsd service method-header-part:   ProbeMatches wsa__Action
//gsoap wsd service method-action:        ProbeMatches http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ProbeMatches
int __wsd__ProbeMatches (struct wsd__ProbeMatchesType *wsd__ProbeMatches, void);


#import "onvif.h"

/******************************************************************************\
*                                                                            *
* FUNCTIONS                                            *
*                                                                            *
*                                                                            *
\******************************************************************************/

// struct SOAP_ENV__Header

struct SOAP_ENV__Header
{
	mustUnderstand wsa__AttributedURI* wsa__MessageID 0;
	mustUnderstand wsa__Relationship* wsa__RelatesTo 0;
	mustUnderstand wsa__EndpointReferenceType* wsa__From 0;
	mustUnderstand wsa__EndpointReferenceType* wsa__ReplyTo 0;
	mustUnderstand wsa__EndpointReferenceType* wsa__FaultTo 0;
	mustUnderstand wsa__AttributedURI* wsa__To 0;
	mustUnderstand wsa__AttributedURI* wsa__Action 0;
	mustUnderstand wsd__AppSequenceType* wsd__AppSequence 0;
    int                                  __size                        ;
    _XML                                *__any                         ;	///< Catch any element content in XML string.
}; 

#import "wsse.h"	// wsse = <http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd>

// Theses directives adds http://www.onvif.org/ver10/network/wsdl to namespace map table.
//gsoap dn schema namespace:	http://www.onvif.org/ver10/network/wsdl
//gsoap dn schema elementForm:	qualified

// gsoap does not push to namespace map namespaces with empty definitions,
// so we define dummy type to prevent this behavior.
typedef std::string dn__dummy;

// Theses directives adds http://www.onvif.org/ver10/topics to namespace map table.
//gsoap tns1 schema namespace: http://www.onvif.org/ver10/topics
//gsoap tns1 schema elementForm:	qualified

// gsoap does not push to namespace map namespaces with empty definitions,
// so we define dummy type to prevent this behavior.
typedef std::string tns1__dummy;


//gsoap xmime schema namespace:	 http://www.w3.org/2005/05/xmlmime
//gsoap xmime schema elementForm:	qualified


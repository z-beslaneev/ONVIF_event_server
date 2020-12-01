
// Customize wsnt_TopicExpressionType to prevent ignoring message topic.
class wsnt__TopicExpressionTypeCustom
{
public:
	// was:
	// struct soap_dom_element __any;	/* external */
	xsd__QName	    __any                         ;
	@xsd__anyURI        Dialect                        1;
	@xsd__anyAttribute  __anyAttribute                ;
	xsd__anyType        __mixed                       0;
	struct soap         *soap                          ;
};
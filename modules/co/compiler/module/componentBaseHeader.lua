local function template( writer, c, t )
	c.header( writer, c, "Base class generated for component '", t.fullName, "'" )

	writer( [[

#ifndef _]], t.fullNameUpperUnderline, [[_BASE_H_
#define _]], t.fullNameUpperUnderline, [[_BASE_H_

]] )

	local headers = t.includedHeaders
	for i = 1, #headers do
		writer( "#include <", headers[i], ">\n" )
	end

	for fullName, type in pairs( t.forwardDeclTypes ) do
		writer( "#include <", type.headerName, ">\n" )
	end

	writer( "#include <co/reserved/ComponentBase.h>\n\n" )

	c.utils.openNamespaces( writer, c.moduleName )

	writer( "\n" )

	local facets = t.facets
	local receptacles = t.receptacles

	for i, itf in ipairs( facets ) do
		writer( [[
//! ]], t.fullName, [[ has a facet named ']], itf.name, [[', of type ]], itf.type.fullName, ".\n", [[
class ]], t.name , [[_]], itf.type.fullNameUnderline, [[ : public ]], itf.type.cppName, "\n", [[
{
public:
	virtual co::IInterface* getInterface();
	virtual co::IPort* getFacet();
};

]] )
	end

	writer( [[
/*!
	Inherit from this class to implement the component ']], t.fullName, [['.
 */
class ]], t.name, [[_Base : public co::ComponentBase]] )

	if #facets > 0 then
		for i, itf in ipairs( facets ) do
			writer( ",\n\tpublic ", t.name, "_", itf.type.fullNameUnderline )
		end
	end

	writer( [[

{
public:
	]], t.name, [[_Base();
	virtual ~]], t.name, [[_Base();

	// co::IService Methods:
	co::IObject* getProvider();
	void serviceRetain();
	void serviceRelease();

	// co::IObject Methods:
	co::IComponent* getComponent();
	co::IService* getServiceAt( co::IPort* );
	void setServiceAt( co::IPort*, co::IService* );
]] )

	if #receptacles > 0 then
		writer( "\nprotected:" )

		for i, itf in ipairs( receptacles ) do
			writer( [[

	//! Get the service at receptacle ']], itf.name, [[', of type ]], itf.type.fullName, ".\n", [[
	virtual ]], itf.type.cppName, [[* ]], t.formatAccessor( "get", itf.name ), [[Service() = 0;

	//! Set the service at receptacle ']], itf.name, [[', of type ]], itf.type.fullName, ".\n", [[
	virtual void ]], t.formatAccessor( "set", itf.name ), [[Service( ]], itf.type.cppName, [[* ]], itf.name, [[ ) = 0;
]] )
		end
	end

	writer( "};\n\n" )

	c.utils.closeNamespaces( writer, c.moduleName )

	writer( [[

#endif // _]], t.fullNameUpperUnderline, [[_BASE_H_
]] )

end

return template

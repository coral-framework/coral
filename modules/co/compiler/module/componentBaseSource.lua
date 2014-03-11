local function template( writer, c, t )
	c.header( writer, c, "Base class generated for component '", t.fullName, "'" )

	writer( [[

#include "]], t.name, [[_Base.h"
#include <co/Coral.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/IInterface.h>

]] )

	c.revisionCheck( writer, c, t )

	c.utils.openNamespaces( writer, c.moduleName )

	if c.moduleName ~= 'co' then
		writer( [[

void moduleRetain();
void moduleRelease();
]] )
	end

	local facets = t.facets
	local receptacles = t.receptacles

	for i, itf in ipairs( facets ) do
		writer( [[

//------ ]], t.fullName, [[ has a facet named ']], itf.name, [[', of type ]], itf.type.fullName, [[ ------//

co::IInterface* ]], t.name, [[_]], itf.type.fullNameUnderline, [[::getInterface()
{
	return co::typeOf<]], itf.type.cppName, [[>::get();
}

co::IPort* ]], t.name, [[_]], itf.type.fullNameUnderline, [[::getFacet()
{
	co::IComponent* component = static_cast<co::IComponent*>( co::getType( "]], t.fullName, [[" ) );
	assert( component );
	co::IPort* facet = static_cast<co::IPort*>( component->getMember( "]], itf.name, [[" ) );
	assert( facet );
	return facet;
}
]] )
	end

	writer( [[

//------ ]], t.name, [[_Base ------//

]], t.name, [[_Base::]], t.name, [[_Base()
{
]] )

	if c.moduleName == 'co' then
		writer( "\t// empty\n" )
	else
		writer( "\tmoduleRetain();\n" )
	end

	writer( [[
}

]], t.name, [[_Base::~]], t.name, [[_Base()
{
]] )

	if c.moduleName == 'co' then
		writer( "\t// empty\n" )
	else
		writer( "\tmoduleRelease();\n" )
	end

	writer( [[
}

co::IObject* ]], t.name, [[_Base::getProvider()
{
	return this;
}

void ]], t.name, [[_Base::serviceRetain()
{
	incrementRefCount();
}

void ]], t.name, [[_Base::serviceRelease()
{
	decrementRefCount();
}

co::IComponent* ]], t.name, [[_Base::getComponent()
{
	co::IType* type = co::getType( "]], t.fullName, [[" );
	assert( type->getKind() == co::TK_COMPONENT );
	return static_cast<co::IComponent*>( type );
}

co::IService* ]], t.name, [[_Base::getServiceAt( co::IPort* port )
{
	checkValidPort( port );
	co::IService* res = NULL;
	switch( port->getIndex() )
	{
]] )

	for i, itf in ipairs( facets ) do
		writer( "\tcase ", itf.index, ":\t\tres = static_cast<", itf.type.cppName, "*>( this ); break;\n" )
	end

	for i, itf in ipairs( receptacles ) do
		writer( "\tcase ", itf.index, ":\t\tres = ", t.formatAccessor( "get", itf.name, "Service" ), "(); break;\n" )
	end

	writer( [[
	default:	raiseUnexpectedPortIndex();
	}
	return res;
}

void ]], t.name, [[_Base::setServiceAt( co::IPort* receptacle, co::IService* service )
{
	checkValidReceptacle( receptacle );
]] )

	if #receptacles > 0 then
		writer( [[
	switch( receptacle->getIndex() )
	{
]] )

		for i, itf in ipairs( receptacles ) do
			writer( "\tcase ", itf.index, ":\t\t", t.formatAccessor( "set", itf.name, "Service" ),
				"( co::cast<", itf.type.cppName, ">( service ) ); break;\n" )
		end

		writer( [[
	default:	raiseUnexpectedPortIndex();
	}
]] )
	else
		writer( [[
	raiseUnexpectedPortIndex();
	CORAL_UNUSED( service );
]] )
	end

	writer( "}\n\n" )

	c.utils.closeNamespaces( writer, c.moduleName )

end

return template

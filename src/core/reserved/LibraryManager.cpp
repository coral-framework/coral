#include "LibraryManager.h"
#include "../utils/Library.h"
#include <map>
#include <vector>

typedef std::map<co::IModulePart*, co::RefPtr<co::Library> > LibraryMap;
typedef std::vector<co::RefPtr<co::Library> > LibraryList;

namespace
{
	LibraryMap sg_loadedLibs;
	LibraryList sg_releasedLibs;
	bool sg_noDlClose( false );
}

void co::LibraryManager::add( co::IModulePart* part, co::Library* library )
{
	if( sg_noDlClose )
		library->setNoDlClose( true );

	sg_loadedLibs.insert( LibraryMap::value_type( part, library ) );
}

void co::LibraryManager::release( co::IModulePart* part )
{
	LibraryMap::iterator it = sg_loadedLibs.find( part );
	if( it == sg_loadedLibs.end() )
	{
		assert( false );
		return;
	}

	// unload the library
	sg_releasedLibs.push_back( it->second );
	sg_loadedLibs.erase( it );
}

void co::LibraryManager::releaseAll()
{
	for( LibraryMap::iterator it = sg_loadedLibs.begin(); it != sg_loadedLibs.end(); ++it )
		sg_releasedLibs.push_back( it->second );
	sg_loadedLibs.clear();
}

void co::LibraryManager::flush()
{
	sg_releasedLibs.clear();
}

void co::LibraryManager::setNoDlClose()
{
	sg_noDlClose = true;
}

#ifndef _CO_RESERVED_LIBRARYMANAGER_H_
#define _CO_RESERVED_LIBRARYMANAGER_H_

#include <co/Platform.h>

namespace co {

class Library;
class IModulePart;

/*!
	\brief Reserved, auxiliary static class that keeps track of all loaded
	module libraries (instances of co::Library) until they can be unloaded.
 */
class CORAL_EXPORT LibraryManager
{
public:
	//! Registers a module part and its associated library.
	static void add( IModulePart* part, Library* library );

	/*!
		Releases the library associated with a module part,
		so it can be unloaded by the next flush().
	 */
	static void release( IModulePart* part );

	//! Releases all registered libraries.
	static void releaseAll();

	//! Unloads all released libraries that are still loaded.
	static void flush();

	/*!
		Call this before initialization to skip dlclose() calls when unloading
		a module on UNIX. This may be useful if you are post-analyzing your
		application with a tool such as valgrind. Has no effect on Windows.
	 */
	static void setNoDlClose();
};

} // namespace co

#endif // _CO_RESERVED_LIBRARYMANAGER_H_

#ifndef _CO_LIBRARY_H_
#define _CO_LIBRARY_H_

#include <co/reserved/RefCounted.h>
#include <string>

namespace co {

/*!
	Platform-independent handling of dynamic-link libraries (DLLs).
 
	The concept of a DLL for this class is platform independent; i.e. a DLL could be
	a shared object (\c .so) on UNIX, a dynamic library (\c .dylib) on Mac OS, or a
	dynamic-link library (\c .dll) on Windows.
 
	Each Library instance operates on a single library file. Basic operations are:
	  - load(): loads the library. Calling load() is optional, since resolve() calls it internally.
	  - resolve(): resolves the address of an exported library symbol.
	  - unload(): attempts to unload the library.
	Load/unload errors are raised as exceptions.
 
	\par Load Hints
	In UNIX-based systems, it is possible to control \e when and \e how library symbols are bound
	by specifying <em>load hints</em> (see setLoadHints()). By default, all symbols are bound when
	the library is loaded, and exported symbols are "hidden" from other modules (i.e. only accessible
	through resolve()). All load hints are currently ignored on Windows.
 */
class CORAL_EXPORT Library : public RefCounted
{
public:
	//! Hints for when and how library symbols should be bound.
	enum LoadHint
	{
		LazyBindFunctionsHint	= 1, //!< Only bind functions when they're called for the first time.
		GlobalSymbolsHint		= 2, //!< All exported symbols should be made available to other modules.
	};

public:
	//! Constructs an object to operate on the library file specified by \c fileName.
	Library( const std::string& fileName );

	//! Destructor. Unloads the library if it is still loaded.
	virtual ~Library();

	//! Returns the library file name.
	inline const std::string& getFileName() const { return _fileName; }

	//! Returns whether the library is currently loaded.
	inline bool isLoaded() const { return _handle != 0; }

	//! Returns the underlying OS-specific DLL handle.
	inline void* getHandle() const { return _handle; }

	//! Returns the currently set load hints (see LoadHint).
	inline int8 getLoadHints() const { return _loadHints; }

	/*!
		Sets the current load hints (any of the constants in LoadHint, \c OR'ed).
		This method must be called before any call to load() or resolve().
	 */
	inline void setLoadHints( int8 hints ) { _loadHints = hints; }

	/*!
		If this is set to true, dlclose() will not be called to unload the
		library on UNIX, leaving the shared-object on memory when the process
		ends. This may be useful if you are post-analyzing your application
		with a tool such as valgrind. This has no effect on Windows.		
	 */
	inline void setNoDlClose( bool noDlClose ) { _noDlClose = noDlClose; }

	/*!
		Loads the library (if not currently loaded) using the current load hints.
		Note that you are not required to call this method explicitly, since
		resolve() will call it when needed.
		\throw ModuleLoadException if an error occurs while loading the library.
	 */
	void load();

	/*!
		Resolves the address of an exported library symbol.
		Returns 0 if the library was successfully loaded, but the symbol could not be found.
		\throw ModuleLoadException if the library could not be loaded.
	 */
	void* resolve( const char* symbolName );

	/*!
		Attempts to unload the library, if currently loaded.

		After calling this method, isLoaded() will return false. However, on most systems it is not
		guaranteed that the DLL will really be unloaded by the OS. For instance, on UNIX-based
		systems this method only decrements the library's reference count.

		\throw ModuleLoadException if an error occurs while unloading the library.
	 */
	void unload();

private:
	Library( const Library& other );
	Library& operator=( const Library& other );

	void doLoad();
	void* doResolve( const char* symbolName );
	void doUnload();

	void getLastErrorMessage( std::string& message );
	void assertIsLoaded();

private:
	std::string _fileName;
	void* _handle;
	int8 _loadHints;
	bool _noDlClose; // if true, won't call dlclose() on UNIX (default = false)
};

}; // namespace co

#endif // _CO_LIBRARY_H_

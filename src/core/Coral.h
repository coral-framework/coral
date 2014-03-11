#ifndef _CO_CORAL_H_
#define _CO_CORAL_H_

#include <co/Common.h>
#include <co/IService.h>
#include <co/IInterface.h>

namespace co {

// Forward Decls:
class IObject;
class ISystem;

//------ Coral Path -----------------------------------------------------------

/*!
	\brief Returns the list of type repositories in use by the system.
	All paths are guaranteed to be absolute pathnames.
	\ingroup setup
 */
CORAL_EXPORT Slice<std::string> getPaths();

/*!
	\brief Adds one or more type repositories for use by the system.

	The passed string may contain a single directory or a list of directories,
	separated by comma or semicolon (or colon, on UNIX systems).

	This function will normalize and absolutize all directory paths.

	\ingroup setup
 */
CORAL_EXPORT void addPath( const std::string& path );

	
//------ CSL Flags ------------------------------------------------------------

//! Flags for toggling optional language features.
enum CSLFlags
{
	CSL_ANNOTATIONS		= 1, //!< Load and process annotations.
	CSL_CPPBLOCKS		= 2, //!< Load C++ blocks as 'co.CppBlock' annotations.
	CSL_DOCUMENTATION	= 4  //!< Load comments as 'co.Documentation' annotations.
};

/*!
	Returns the current state of the CSL flags.
	Default is co::CSL_ANNOTATIONS (i.e. comments and C++ blocks are ignored).
 */
CORAL_EXPORT uint8 getCSLFlags();

/*!
	Sets the current state of the CSL flags.
 */
CORAL_EXPORT void setCSLFlags( uint8 flags );


//------ Bootstrap and Shutdown -----------------------------------------------

/*!
	\brief Returns the primary co.ISystem service.

	This first call to this function (ever, or since the last call to co::shutdown())
	initializes the Coral framework.

	\ingroup setup
 */
CORAL_EXPORT ISystem* getSystem();

/*!
	\brief Tears down and destroys the system, releasing all memory and resources.
	\ingroup setup
 */
CORAL_EXPORT void shutdown();


//------ General Utility Functions --------------------------------------------

/*!
	\brief Utility function to retrieve or load a type by name.
	This is equivalent to calling: \code co::getSystem()->getTypes()->getType( fullName ) \endcode
	Please refer to co::ITypeManager::getType() for the list of exceptions this function may throw.
	\ingroup convenience
 */
CORAL_EXPORT IType* getType( const std::string& fullName );

/*!
	\brief Utility function to get or create an IArray whose elements are of type \a elementType.
	This is equivalent to calling: \code co::getSystem()->getTypes()->getArrayOf( elementType ) \endcode
	Please refer to co::ITypeManager::getArrayOf() for the list of exceptions this function may throw.
	\ingroup convenience
 */
CORAL_EXPORT IArray* getArrayOf( IType* elementType );

/*!
	\brief Utility function to instantiate a component given its full type name.
	This is equivalent to calling: \code co::getType( fullName )->getReflector()->newInstance() \endcode
	Please note that this function may raise all exceptions raised by the aforementioned methods.
	\ingroup convenience
 */
CORAL_EXPORT IObject* newInstance( const std::string& fullName );

/*!
	\brief Utility function to get the best provider of \a serviceType for clients of type \a clientType.
	If \a clientType is null this function will retrieve the service's global instance.
	Please refer to co::IServiceManager::getServiceForType() for the list of exceptions this function may throw.
	\ingroup convenience
 */
CORAL_EXPORT IService* getServiceForType( IInterface* serviceType, IInterface* clientType );

/*!
	\brief Utility function to get the best provider of \a serviceType for the given \a clientInstance.
	This is equivalent to calling: \code co::getSystem()->getServices()->getServiceForInstance() \endcode
	Please refer to co::IServiceManager::getServiceForInstance() for the list of exceptions this function may throw.
	\ingroup convenience
 */
CORAL_EXPORT IService* getServiceForInstance( IInterface* serviceType, IService* clientInstance );

/*!
	\brief Template function to get a global service by its type.
	Please refer to co::IServiceManager::getService() for the list of exceptions this function may throw.
	\ingroup convenience
 */
template<typename T>
inline T* getService()
{
	return static_cast<T*>( getServiceForType( typeOf<T>::get(), NULL ) );
}

/*!
	\brief Template function to get a <tt>clientType</tt>-specialized service by its type.
	This picks the most appropriate service instance available for clients of the given \a clientType.
	Please refer to co::IServiceManager::getServiceForType() for the list of exceptions this function may throw.
	\ingroup convenience
 */
template<typename T>
inline T* getService( IInterface* clientType )
{
	return static_cast<T*>( getServiceForType( typeOf<T>::get(), clientType ) );
}

/*!
	\brief Template function to get a <tt>clientInstance</tt>-specialized service by its type.
	This picks the most appropriate service instance available for the given \a clientInstance.
	Please refer to co::IServiceManager::getServiceForInstance() for the list of exceptions this function may throw.
	\ingroup convenience
 */
template<typename T>
inline T* getService( IService* clientInstance )
{
	return static_cast<T*>( getServiceForInstance( typeOf<T>::get(), clientInstance ) );
}

/*!
	\brief Searches the Coral path looking for a file in a specific module's dir.
	This can be used to resolve the absolute pathname of a module resource file.
	\param[in] moduleName Full name of the module where the file is located.
	\param[in] fileName Exact name of the file we're looking for in the module's dir.
	\param[out] filePath set with the absolute path to the file, if one is found.
	\return true if a file was found; false if the search failed.
 */
CORAL_EXPORT bool findFile( const std::string& moduleName, const std::string& fileName, std::string& filePath );

} // namespace co

/******************************************************************************

 Copyright (c) 2009-2012 Tecgraf, PUC-Rio.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 *****************************************************************************/

#endif // _CO_CORAL_H_

#ifndef _CO_REFPTR_H_
#define _CO_REFPTR_H_

namespace co {

/*!
	\brief A smart pointer that provides reference counting for Coral services.
	\tparam T a service type (co::IService).
 */
template<class T>
class RefPtr
{
public:
	//! Creates an invalid (null) RefPtr.
	inline RefPtr() : _ptr( nullptr )
	{;}

	//! Initializes the RefPtr to reference a given service.
	inline RefPtr( T* p ) : _ptr( p )
	{
		tryRetain();
	}

	//! Copy constructor.
	inline RefPtr( const RefPtr& other ) : _ptr( other._ptr )
	{
		tryRetain();
	}

	//! Destructor.
	inline ~RefPtr()
	{
		tryRelease();
		#ifndef CORAL_NDEBUG
		_ptr = nullptr;
		#endif
	}

	//! Returns the service referenced by this RefPtr.
	inline T* get() const { return _ptr; }

	//! Sets the service referenced by this RefPtr.
	inline void set( T* p )
	{
		if( _ptr != p )
		{
			T* old = _ptr;
			_ptr = p;
			tryRetain();
			if( old ) old->serviceRelease();
		}
	}

	/*!
		Efficiently swaps the internal pointers of two RefPtr's.
		This saves two serviceRetain() and two serviceRelease() calls.
	 */
	inline void swap( RefPtr& other )
	{
		T* temp = _ptr;
		_ptr = other._ptr;
		other._ptr = temp;
	}

	//! ADL-based overload for swap().
	friend inline void swap( RefPtr& a, RefPtr& b ) { a.swap( b ); }

	//! Assignment operator.
	inline RefPtr& operator=( const RefPtr& other )
	{
		set( other._ptr );
		return *this;
	}

	//! Pointer assignment operator.
	inline RefPtr& operator=( T* p )
	{
		set( p );
		return *this;
	}

	//! Comparison operator for RefPtr's.
	//@{
	inline bool operator==( const RefPtr& other ) const { return _ptr == other._ptr; }
	inline bool operator!=( const RefPtr& other ) const { return _ptr != other._ptr; }
	inline bool operator<(const RefPtr& other ) const { return _ptr < other._ptr; }
	inline bool operator>(const RefPtr& other) const { return _ptr > other._ptr; }
	//@}

	//! Comparison operator for service pointers.
	//@{
	inline bool operator==( const T* p ) const { return _ptr == p; }
	inline bool operator!=( const T* p ) const { return _ptr != p; }
	inline bool operator<( const T* p ) const { return _ptr < p; }
	inline bool operator>( const T* p ) const { return _ptr > p; }
	//@}

	//! Dereference operator.
	inline T& operator*() const { return *_ptr; }

	//! Member access operator.
	inline T* operator->() const { return _ptr; }

	//! Invalid (i.e. null) pointer test operator.
	inline bool operator!() const { return _ptr == nullptr; }

	//! Returns whether we have a valid service pointer (i.e. not null).
	inline bool isValid() const { return _ptr != nullptr; }

private:
	inline void tryRetain() { if( _ptr ) _ptr->serviceRetain(); }
	inline void tryRelease() { if( _ptr ) _ptr->serviceRelease(); }

private:
	T* _ptr;
};

} // namespace co

#endif // _CO_REFPTR_H_

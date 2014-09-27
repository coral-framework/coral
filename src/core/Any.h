#ifndef _CO_ANY_H_
#define _CO_ANY_H_

#include <co/Coral.h>
#include <co/IEnum.h>
#include <co/IArray.h>
#include <co/IStruct.h>
#include <co/IException.h>
#include <co/IComponent.h>
#include <co/INativeClass.h>
#include <ostream>

namespace co {

class Any;
class AnyValue;

//! Private namespace with auxiliary code for co::Any.
namespace __any {

union Data {
  // Storage for primitives:
  bool b;
  int8 i8;
  int16 i16;
  int32 i32;
  int64 i64;
  double d;

  // Multipurpose pointers:
  void* ptr;
  const void* cptr;
  int8* bytes;
  std::string* str;
  IService* service;
  Data* deref;

  // Constructors
  Data() : d(0) { ; }
  Data(const void* ptr) : cptr(ptr) { ; }
};

struct State {
  IType* type;  // Variable Type
#if defined(CORAL_CC_MSVC)
  size_t  // MSVC refuses to merge different-sized types in a bitfield
#else
  bool
#endif
      isIn : 1;  // True if the var is 'in'; False if it is 'out'
  size_t size    // Only used for 'in' arrays and C-strings
#if CORAL_POINTER_SIZE == 4
      : 31;
#else
      : 63;
#endif
  Data data;  // Storage for primitives and pointers

  // Default constructor
  State() : type(BASIC_TYPES[TK_NULL].get()), isIn(false), size(0) { ; }

  // Output variable constructor
  State(IType* type, const void* ptr)
      : type(type), isIn(false), size(0), data(ptr) {
    ;
  }
};

template <typename T> struct Ref {
  inline static void set(State& s, const T& v) { s.data.cptr = &v; }
  inline static T& get(State& s) { return *reinterpret_cast<T*>(s.data.ptr); }
};

template <typename T, TypeKind kind> struct Value {
  inline static void set(State& s, const T& v) {
    *reinterpret_cast<T*>(&s.data) = v;
  }
  inline static const T& get(State& s) {
    return *reinterpret_cast<T*>(&s.data);
  }
};

template <typename T> struct Value<T, TK_STRING> : public Ref<T> {};
template <typename T> struct Value<T, TK_STRUCT> : public Ref<T> {};
template <typename T> struct Value<T, TK_NATIVECLASS> : public Ref<T> {};

template <typename T, bool isIn> struct Tag {
  typedef typeOf<T> VT;
  inline static void tag(State& s) {
    static_assert(VT::kind != TK_NULL, "unsupported type");
    s.type = VT::get();
    s.isIn = isIn;
  }
};

template <typename T> struct Variable {
  inline static void tag(State& s) { Variable<const T&>::tag(s); }
  inline static void set(State& s, const T& v) {
    Variable<const T&>::set(s, v);
  }
  inline static T get(State& s) { return Variable<const T&>::get(s); }
};

template <typename T> struct Variable<T&> : public Tag<T, false> {
  inline static void set(State& s, T& v) { Ref<T>::set(s, v); }
  inline static T& get(State& s) { return Ref<T>::get(s); }
};

template <typename T> struct Variable<const T&> : public Tag<T, true> {
  typedef Value<T, kindOf<T>::kind> ValueHelper;
  inline static void set(State& s, const T& v) { ValueHelper::set(s, v); }
  inline static const T& get(State& s) { return ValueHelper::get(s); }
};

template <>
struct Variable<const AnyValue&> : public Tag<AnyValue, false>,
                                   public Ref<AnyValue> {};
template <> struct Variable<AnyValue&> : public Variable<const AnyValue&> {};

template <typename T> struct IllegalRecursiveAny {
  inline static void tag(State&) {
    static_assert(sizeof(T) < 0, "illegal recursive co::Any");
  }
};
template <> struct Variable<Any&> : public IllegalRecursiveAny<Any&> {};
template <>
struct Variable<const Any&> : public IllegalRecursiveAny<const Any&> {};

template <TypeKind, bool isIn, typename T> struct Pointer {
  inline static void tag(State&) {
    static_assert(sizeof(T) < 0, "unexpected pointer type");
  }
};

template <bool isIn, typename T>
struct Pointer<TK_NATIVECLASS, isIn, T> : public Tag<T, isIn>, Ref<T> {};

template <bool isIn, typename T>
struct Pointer<TK_INTERFACE, isIn, T> : public Tag<T, true> {
  inline static void set(State& s, T v) {
    if (v) s.type = v->getInterface();
    s.data.service = v;
  }
  inline static T get(State& s) { return static_cast<T>(s.data.service); }
};

template <typename T>
struct Variable<T*&> : public Pointer<kindOf<T*>::kind, false, T*> {};
template <typename T>
struct Variable<T* const&> : public Pointer<kindOf<T*>::kind, true, T* const> {
};

template <typename T> struct Variable<const T* const&> {
  inline static void tag(State&) {
    static_assert(sizeof(T) < 0, "illegal const pointer");
  }
};
template <typename T>
struct Variable<const T*&> : public Variable<const T* const&> {};

template <typename T> struct Variable<RefPtr<T>&> {
  typedef typeOf<T> IT;
  inline static void tag(State& s) {
    static_assert(IT::kind == TK_INTERFACE,
                  "illegal RefPtr to a non-interface type");
    s.type = IT::get();
    s.isIn = false;
  }
  inline static void set(State& s, RefPtr<T>& v) { s.data.ptr = &v; }
  inline static RefPtr<T>& get(State& s) {
    return *reinterpret_cast<RefPtr<T>*>(s.data.ptr);
  }
};

template <typename T> struct Variable<const RefPtr<T>&> {
  typedef Variable<T* const&> AsPtr;
  inline static void tag(State& s) { AsPtr::tag(s); }
  inline static void set(State& s, const RefPtr<T>& v) {
    AsPtr::set(s, v.get());
  }
};

template <typename T>
struct Variable<const Slice<T>&> : public Tag<Slice<T>, true> {
  inline static void set(State& s, const Slice<T>& v) {
    s.data.cptr = v.begin();
    s.size = v.getSize();
  }
  inline static Slice<T> get(State& s) {
    return Slice<T>(reinterpret_cast<T*>(s.data.ptr), s.size);
  }
};

template <typename T>
struct Variable<Slice<T>&> : public Variable<const Slice<T>&> {};
template <typename T>
struct Variable<TSlice<T>&> : public Variable<const Slice<T>&> {};
template <typename T>
struct Variable<const TSlice<T>&> : public Variable<const Slice<T>&> {};

template <typename T>
struct Variable<std::vector<T>&> : public Tag<std::vector<T>, false>,
                                   public Ref<std::vector<T>> {};

template <typename T> struct Variable<const std::vector<T>&> {
  typedef Variable<Slice<T>&> AsSlice;
  inline static void tag(State& s) { AsSlice::tag(s); }
  inline static void set(State& s, const std::vector<T>& v) {
    AsSlice::set(s, v);
  }
};

template <typename T> struct Variable<const std::vector<co::RefPtr<T>>&> {
  typedef Variable<Slice<T*>&> AsSlice;
  inline static void tag(State& s) { AsSlice::tag(s); }
  inline static void set(State& s, const std::vector<co::RefPtr<T>>& v) {
    AsSlice::set(s, v);
  }
};

}  // namespace __any

/*!
  \brief An intermediate representation for variables in the Coral type system,
  used for reflection.

  By design, the state of a co::Any is \e transient, so its instances <b>should
  only be allocated
  on the stack</b>. This class is optimized for use in C++ method signatures (as
  a parameter or
  return type) and is <b>unsafe for use as a field type or member variable</b>.

  \par Supported Features:
    - Type checks, including compliance with type const'ness.
    - Automatic conversion between primitive values (boolean, arithmetic types
  and enum).
    - Type-safe storage and retrieval of pointers and references.
    - Storage and retrieval of arrays as pointers to \c std::vector's or \c
  co::Slice's.

  \par What is NOT supported:
    - Storing exceptions.
    - Reference counting of services. Stored service pointers are \b not
  considered active references.

  \par Values vs. References
    For efficiency, this class only allows the storage of values for \c enums
  and <em>primitive types</em>
    (i.e. \c bool, <tt>[u]int{8|16|32}</tt> and \c double). All other
  types must be stored and
    retrieved <em>by reference</em> (i.e. either as pointers or actual
  references), and the programmer must
    guarantee referred objects outlive their co::Any instances.
    \par
    For variables stored by reference, it is possible to retrieve a pointer even
  if the passed variable was
    a reference. However, the opposite (retrieving a reference where a pointer
  was passed) is \b not
    allowed, because the pointer could be NULL. This is not to be confused with
  retrieving a reference to a
    pointer (<tt>Foo*&</tt>), which is allowed if the passed variable was truly
  a reference to a pointer.

  \par Arrays
    An array can be passed using three possible representations:
      -# A \c co::Slice, which is the most generic representation but cannot be
  used to add/remove elements;
      -# A \c std::vector, which is useful when the receiver needs to add/remove
  elements to/from the array;
    \par
    These types are \e always considered to be \b mutable. In other words, all
  arrays passed as \c std::vectors
    or \c co::RefVectors can have their contents changed. To prevent the
  addition/removal of elements, one should
    pass the array as a \c co::Slice instead. Moreover, to prevent existing
  elements from being modified,
    one should pass a \c co::Slice of \c <b>const</b> elements.
    \par
    For instance, an array passed as a <tt>co::Slice<std::string></tt> is
  completely immutable &mdash;
    as opposed to a <tt>std::vector<std::string></tt>, which is fully mutable.
    \par
    Arrays represented by \c std::vectors or \c co::RefVectors must always be
  passed and retrieved
    <b>by reference</b>, while \c co::Ranges must always be passed and retrieved
  <b>by value</b>.
    \par
    Arrays must generally be retrieved by the exact same type they were passed.
  However, when
    retrieving \c co::Ranges the following coercion rules apply:
      - It is possible to retrieve a <tt>co::Slice<\e ValueType></tt> from an
  array passed as a
        <tt>std::vector<\e ValueType></tt>.
      - It is possible to retrieve a <tt>co::Slice<\e SuperType*></tt> from an
  array
        passed as either a <tt>co::Slice<\e SubType* [const]></tt>, a
        <tt>std::vector<\e SubType*></tt> or a <tt>co::RefVector<\e
  SubType></tt>.
    \par
    Non-listed cases must be retrieved by exact type.
    In all rules, \e ValueType is a Coral value (such as a \c bool or a \c
  struct), optionally
    passed by reference; and both \e SubType and \e SuperType are Coral
  interfaces, where \e SubType
    is a subtype of \e SuperType, or both are the same interface.

  \par Automatic Conversions for Values
    All types storable as values in a co::Any can be converted to any other
  value type.
    This includes \c enums and all number types,
    and excludes types passed by reference (most notably strings).
    \par
    Conversion rules are the same applied for \c static_casts by your C++
  compiler.
    Enums are seen as \c co::uint32 values. However, when converting from a
  different type, a check is made
    to ensure the result maps to a valid enum identifier (otherwise, a
  co::IllegalCastException is raised).
 */
class CORAL_EXPORT Any {
 public:
  //! Variable state descriptor.
  __any::State state;

  //! Creates a null co::Any.
  inline Any() : state() { ; }

  //! Creates a reference to any non-const Coral variable.
  template <typename T> inline Any(T& var) { set(var); }

  //! Creates a reference to any const Coral variable.
  template <typename T> inline Any(const T& var) { set(var); }

  //! Creates a reference to a statically-allocated array.
  template <typename T, size_t size> inline Any(T (&array)[size]) {
    setIn(Slice<T>(array));
  }

  //! Creates a reference to any variable reflectively. \see set()
  inline Any(bool isIn, IType* type, const void* addr, size_t size = 0) {
    set(isIn, type, addr, size);
  }

  //! Copy constructor.
  inline Any(const Any& other) : state(other.state) { ; }
  inline Any(Any& other) : state(other.state) { ; }

  //! Destructor.
  inline ~Any() { ; }

  //! Clears the co::Any (makes it null).
  inline void clear() { state.type = BASIC_TYPES[TK_NULL].get(); }

  //! \name Variable Introspection
  //@{

  //! Whether this co::Any is null.
  inline bool isNull() const { return state.type->getKind() == TK_NULL; }

  //! Whether this co::Any is not null.
  inline bool isValid() const { return !isNull(); }

  //! Returns the type of the stored variable.
  inline IType* getType() const { return state.type; }

  //! Returns the kind of the stored variable.
  inline TypeKind getKind() const { return state.type->getKind(); }

  //! Whether the stored variable is in the 'input' form.
  inline bool isIn() const { return state.isIn; }

  //! Whether the stored variable is in the 'output' form.
  inline bool isOut() const { return !state.isIn; }

  //! Returns the equivalent variable in the 'input' form.
  Any asIn() const;

  /*!
    \brief Returns whether two co::Any's contain equal values.
    Performs a value comparison by derefering both variables.
    The dereferenced values must be of the exact same type.
   */
  bool equals(const Any& other) const;

  /*!
    \brief Returns the number of elements contained in this co::Any.
    For arrays, this is the number of elements in the array; for other
    types this is always 1.
   */
  size_t getCount() const;

  /*!
    \brief Returns the individual size of the elements in this co::Any.
    For arrays, this is the size of each array element; for other types
    this is the size reported by the type's reflector.
   */
  size_t getElementSize() const;

  /*!
    Retrieves a stored variable.
    \throw IllegalCastException if there is no valid conversion from the
          stored variable's type to the requested type \a T.
   */
  template <typename T> inline T get() const { return Getter<T>::get(*this); }

  template <typename T> inline void get(T& v) const {
    Any any(v);
    any.put(*this);
  }

  //@}
  //! \name Automatic Variable Storage
  //@{

  /*!
    Automatically stores any variable supported by the Coral type system.
    Previous contents are discarded.

    This method uses template meta-programming to statically infer a variable's
    type.
    If you need to set a co::Any dynamically at runtime, use the "Custom Storage
    Methods" instead.

    \warning If you don't specify the template variable \a T explicitly, due to
    language limitations
      the compiler will generally miss the fact that a variable's type is
    'const' and/or a reference.
   */

  template <typename T> inline void set(T& var) {
    __any::Variable<T&>::tag(state);
    __any::Variable<T&>::set(state, var);
  }

  template <typename T> inline void setIn(const T& var) { set<const T&>(var); }

  //@}
  /*!
    \name Custom Variable Storage
    These are alternative versions of set() that offer manual control over the
    passed variables.

    \warning
      These methods are intended for advanced users and should be used with
    great care.
      They only contain assertions and do not throw exceptions. Incorrect usage
    may
      cause access violations and memory corruption.
   */
  //@{

  /*!
    Points to any variable supported by the Coral type system.
    \param isIn Whether the variable is in the 'input' or 'output' form.
    \param type Type of the variable.
    \param addr Address of the value associated with the variable.
    \param size Only used if the variable is an input array or a C-string.
   */
  void set(bool isIn, IType* type, const void* addr, size_t size = 0);

  //@}

  /*!
    Stores \a value onto the location pointed to by this 'out' variable.
    \throw IllegalStateException if this is not an output variable.
    \throw IllegalCastException if \a value cannot be converted to
          the output variable's type.
   */
  void put(Any value) const;

  //! Swaps the contents of two co::Any's.
  inline void swap(Any& other) {
    __any::State tmp(state);
    state = other.state;
    other.state = tmp;
  }

  /*!
    \brief Swaps the vector pointers of two 'out arrays' of the same type.
   */
  void swapArrays(const Any& other) const;

  /*!
    \brief Resizes an 'out array' by inserting or erasing elements
    from the vector. This basically mimics std::vector::resize().
   */
  void resize(size_t n) const;

  //! Returns the array element at the given \a index.
  Any at(size_t index) const;

  //! Index operator for access to array elements. \sa at().
  inline Any operator[](size_t index) const { return at(index); }

  //! Tests whether \a this and \a other refer to the same variable.
  bool operator==(const Any& other) const;

  //! Inequality test operator.
  inline bool operator!=(const Any& other) const { return !(*this == other); }

  //! Assignment operator.
  template <typename T> inline Any& operator=(T& v) {
    set<T>(v);
    return *this;
  }
  inline Any& operator=(const Any& other) {
    state = other.state;
    return *this;
  }
  inline Any& operator=(Any& other) {
    state = other.state;
    return *this;
  }

 protected:
  void cast(Any& to) const;

  template <typename T> struct GetByRef {
    inline static T get(const Any& in) {
      Any out;
      __any::Variable<T>::tag(out.state);
      in.cast(out);
      return __any::Variable<T>::get(out.state);
    }
  };

  template <typename T> struct GetByCopy {
    inline static T get(const Any& in) {
      T v;
      Any out(v);
      out.put(in);
      return v;
    }
  };

  template <typename T> struct Getter : public GetByCopy<T> {};
  template <typename T> struct Getter<T&> : public GetByRef<T&> {};
  template <typename T> struct Getter<T*> : public GetByRef<T*> {};
  template <typename T> struct Getter<T* const> : public GetByRef<T* const> {};
  template <typename T> struct Getter<Slice<T>> : public GetByRef<Slice<T>> {};
};

/*!
  ????
 */
class CORAL_EXPORT AnyValue {
 public:
  //! Default constructor.
  AnyValue() { ; }

  inline AnyValue(Any var) { copy(var); }

  //! Copy constructor.
  inline AnyValue(const AnyValue& other) { copy(other._any); }

  //! Destructor.
  inline ~AnyValue() { clear(); }

  //! Whether this co::AnyValue is null.
  inline bool isNull() const { return _any.isNull(); }

  //! Whether this co::AnyValue is not null.
  inline bool isValid() const { return _any.isValid(); }

  //! Returns the type of the contained value.
  inline IType* getType() const { return _any.getType(); }

  //! Returns the kind of the contained value.
  inline TypeKind getKind() const { return _any.getKind(); }

  //! Returns an 'out' co::Any that references this co::AnyValue's value.
  inline const Any& getAny() const { return _any; }

  template <typename T> inline T get() const { return _any.get<T>(); }

  template <typename T> inline void get(T& v) const { return _any.get<T>(v); }

  /*!
    Destroys any stored value and makes this any null.
   */
  void clear();

  inline void set(Any var) { copy(var); }

  /*!
    Creates a default-constructed instance of the given \a type.
    \throw ???
   */
  void* create(IType* type);

  /*!
    Creates an instance of the complex value type \a T and makes this
    co::Any reference it.
   */
  template <typename T> inline T& create() {
    return *reinterpret_cast<T*>(create(typeOf<T>::get()));
  }

  /*!
    Converts the current value to the given \a type.
    If the co::AnyValue is null this method works like create().
    \throw IllegalCastException if the value cannot be converted
          to the target \a type.
   */
  void convert(IType* type);

  //! Swaps the contents of two co::AnyValue's.
  inline void swap(AnyValue& other) { _any.swap(other._any); }

  //! Tests whether our value equals that of \a other.
  inline bool operator==(const AnyValue& other) const {
    return _any.equals(other.getAny());
  }

  //! Inequality test operator.
  inline bool operator!=(const AnyValue& other) const {
    return !(*this == other);
  }

  //! Assignment operator.
  inline AnyValue& operator=(const AnyValue& other) {
    copy(other._any);
    return *this;
  }

  inline AnyValue& operator=(Any var) {
    copy(var);
    return *this;
  }

 private:
  void copy(Any var);

 private:
  Any _any;
};

template <> struct kindOf<Any> { static const TypeKind kind = TK_ANY; };
template <> struct kindOf<AnyValue> { static const TypeKind kind = TK_ANY; };
inline void swap(Any& a, Any& b) { a.swap(b); }
inline void swap(AnyValue& a, AnyValue& b) { a.swap(b); }

}  // namespace co

// std::swap specialization for co::Any:
namespace std {
template <> inline void swap(co::Any& a, co::Any& b) { a.swap(b); }
template <> inline void swap(co::AnyValue& a, co::AnyValue& b) { a.swap(b); }
}  // namespace std

//! Outputs the type of variable in a co::Any. Usage: "os << any.state".
//\relates co::Any
CORAL_EXPORT std::ostream& operator<<(std::ostream& os,
                                      const co::__any::State& s);

//! Outputs the value stored in a co::Any. \relates co::Any
CORAL_EXPORT std::ostream& operator<<(std::ostream& os, const co::Any& any);

//! Outputs the value stored in a co::AnyValue. \relates co::AnyValue
inline std::ostream& operator<<(std::ostream& os, const co::AnyValue& v) {
  return os << v.getAny();
}

#endif  // _CO_ANY_H_

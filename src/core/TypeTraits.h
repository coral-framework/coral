#ifndef _CO_TYPETRAITS_H_
#define _CO_TYPETRAITS_H_

#include <co/Platform.h>
#include <co/TypeKind.h>
#include <type_traits>
#include <string>
#include <vector>

namespace co {

// Forward Decls:
class IEnum;
class IType;
class IArray;
class IObject;
class IStruct;
class IService;
class IException;
class IInterface;
class IComponent;
class INativeClass;
template <typename T> class Slice;
template <typename T> class RefPtr;

// Maps TypeKinds to their names in CSL.
// Warning: assert( i >= 0 && i < co::TK_COUNT ).
extern CORAL_EXPORT const std::string TK_STRINGS[];

// Maps the basic type kinds to their IType (or NULL if not a basic type).
// Warning: assert( i >= 0 && i < co::TK_COUNT ).
extern CORAL_EXPORT RefPtr<IType> BASIC_TYPES[];

//---- Internal Helper Functions -----------------------------------------------

// Just like co::getType() but takes a C-string instead of a std::string.
CORAL_EXPORT IType* getTypeByLiteralName(const char* fullName);

// Returns the TypeKind of an IType.
CORAL_EXPORT TypeKind getKind(IType* type);

// Attempts to get a service of a certain 'type' from 'object'.
CORAL_EXPORT IService* findServiceByType(IObject* object, IInterface* type);

// Gets a service of a certain 'type' from 'object', or raises an exception.
CORAL_EXPORT IService* getServiceByType(IObject* object, IInterface* type);

// Attempts to get a service from 'object' by port name.
CORAL_EXPORT IService* findServiceByName(IObject* object,
                                         const std::string& portName);

// Gets a service from 'object' by port name, or raises an exception.
CORAL_EXPORT IService* getServiceByName(IObject* object,
                                        const std::string& portName);

// Binds a 'service' to the receptacle 'receptacleName' in 'object'.
CORAL_EXPORT void setServiceByName(IObject* object,
                                   const std::string& receptacleName,
                                   IService* service);

//---- Kind Trait Operators (to distinguish all kinds of types) ----------------

//! Returns true for integer types.
inline bool isInteger(TypeKind k) { return k >= TK_BOOL && k <= TK_INT64; }

// Returns true for integer and floating-point types.
inline bool isNumber(TypeKind k) { return k >= TK_BOOL && k <= TK_DOUBLE; }

// Returns true for numbers and enumeration types.
inline bool isCountable(TypeKind k) { return isNumber(k) || k == TK_ENUM; }

// Returns true for all value types.
inline bool isValue(TypeKind k) { return k > TK_NULL && k <= TK_NATIVECLASS; }

// Returns true for reference types.
inline bool isReference(TypeKind k) { return k == TK_INTERFACE; }

// Returns true if you can declare vars and pass parameters of this kind.
inline bool isData(TypeKind k) { return k > TK_NULL && k <= TK_INTERFACE; }

// Returns true if users can define new types of this kind.
inline bool isCustom(TypeKind k) { return k >= TK_STRUCT || k == TK_ENUM; }

// Returns true for value types that require custom reflectors.
inline bool isComplexValue(TypeKind k) {
  return k == TK_STRUCT || k == TK_NATIVECLASS;
}

// Returns true for types with members (ICompositeType).
inline bool isComposite(TypeKind k) {
  return k >= TK_STRUCT && k <= TK_COMPONENT;
}

// Returns true for types that support inheritance.
inline bool hasInheritance(TypeKind k) { return k == TK_INTERFACE; }

// Returns true if a built-in IReflector is available for the type.
inline bool hasBuiltInReflector(TypeKind k) { return k < TK_STRUCT; }

//---- Type Traits: kindOf<T>::kind --------------------------------------------

template <typename> struct kindOf { static const TypeKind kind = TK_NULL; };

// common implementation of co::kindOf<T>:
template <TypeKind k> struct kindOfBase { static const TypeKind kind = k; };

// specializations for basic types:
template <> struct kindOf<bool> : public kindOfBase<TK_BOOL> {};
template <> struct kindOf<int8> : public kindOfBase<TK_INT8> {};
template <> struct kindOf<int16> : public kindOfBase<TK_INT16> {};
template <> struct kindOf<int32> : public kindOfBase<TK_INT32> {};
template <> struct kindOf<int64> : public kindOfBase<TK_INT64> {};
template <> struct kindOf<double> : public kindOfBase<TK_DOUBLE> {};
template <> struct kindOf<std::string> : public kindOfBase<TK_STRING> {};

// specialization for container types:
template <typename T> struct kindOf<Slice<T>> : public kindOfBase<TK_ARRAY> {};
template <typename T> struct kindOf<RefPtr<T>> : public kindOf<T> {};
template <typename T>
struct kindOf<std::vector<T>> : public kindOfBase<TK_ARRAY> {};

// for unknown pointers, use the dereferenced type
template <typename T> struct kindOf<T*> : public kindOf<T> {};
template <typename T> struct kindOf<T* const> : public kindOf<T*> {};

// specialization for co::TypeKind (must go here to avoid cyclic dependencies)
template <> struct kindOf<TypeKind> : public kindOfBase<TK_ENUM> {};

//---- co::typeDescriptorFor<TypeKind>::Type maps a TK to its descriptor -------

template <TypeKind> struct typeDescriptorFor { typedef IType Type; };
template <> struct typeDescriptorFor<TK_ARRAY> { typedef IArray Type; };
template <> struct typeDescriptorFor<TK_ENUM> { typedef IEnum Type; };
template <> struct typeDescriptorFor<TK_EXCEPTION> { typedef IException Type; };
template <> struct typeDescriptorFor<TK_STRUCT> { typedef IStruct Type; };
template <> struct typeDescriptorFor<TK_NATIVECLASS> {
  typedef INativeClass Type;
};
template <> struct typeDescriptorFor<TK_INTERFACE> { typedef IInterface Type; };
template <> struct typeDescriptorFor<TK_COMPONENT> { typedef IComponent Type; };

//---- co::nameOf<T>::get() returns the full name of a Coral type --------------

template <typename T> struct nameOf {
  static const char* get() { return TK_STRINGS[kindOf<T>::kind].c_str(); }
};

//! Common implementation of co::nameOf<T> for arrays:
template <typename ET> struct nameOfArrayBase {
  static const char* get() {
    static const std::string s_name(std::string(nameOf<ET>::get()) + "[]");
    return s_name.c_str();
  }
};

// specialization for array types:
template <typename T> struct nameOf<Slice<T>> : public nameOfArrayBase<T> {};
template <typename T>
struct nameOf<std::vector<T>> : public nameOfArrayBase<T> {};

// if necessary dereference pointer types
template <typename T> struct nameOf<T*> : public nameOf<T> {};
template <typename T> struct nameOf<T* const> : public nameOf<T*> {};
template <typename T> struct nameOf<RefPtr<T>> : public nameOf<T> {};

// specialization for co::TypeKind (must go here to avoid cyclic dependencies)
template <> struct nameOf<TypeKind> {
  static const char* get() { return "co.TypeKind"; }
};

//----- co::typeOf<T>::get() returns the IType instance of a Coral type --------

template <typename T> struct typeOf {
  static const TypeKind kind = kindOf<T>::kind;
  typedef typename typeDescriptorFor<kind>::Type Descriptor;

  static Descriptor* get() {
    IType* type = getTypeByLiteralName(nameOf<T>::get());
    assert(getKind(type) == kind);
    return static_cast<Descriptor*>(type);
  }
};

template <typename T> struct typeOfBasic {
  static const TypeKind kind = kindOf<T>::kind;
  typedef IType Descriptor;
  static Descriptor* get() {
    return reinterpret_cast<Descriptor**>(BASIC_TYPES)[kind];
  }
};

// specializations for basic types:
template <> struct typeOf<bool> : public typeOfBasic<bool> {};
template <> struct typeOf<int8> : public typeOfBasic<int8> {};
template <> struct typeOf<int16> : public typeOfBasic<int16> {};
template <> struct typeOf<int32> : public typeOfBasic<int32> {};
template <> struct typeOf<int64> : public typeOfBasic<int64> {};
template <> struct typeOf<double> : public typeOfBasic<double> {};
template <> struct typeOf<std::string> : public typeOfBasic<std::string> {};

//--- Basic RTTI functions -----------------------------------------------------

// Returns whether a 'service' is of a given 'type'.
// This method always returns true if 'service' is null.
CORAL_EXPORT bool isA(IService* service, IInterface* type);

template <typename SubType> inline bool isA(IService* service) {
  return isA(service, typeOf<SubType>::get());
}

// Raises an IllegalCastException if 'service' if not of a given 'type'.
// No exception is raised if 'service' is null.
CORAL_EXPORT void ensureIsA(IService* service, IInterface* type);

// Downcasts a service, raising an IllegalCastException on failure.
// The cast always succeeds for null pointers (result is a null pointer).
// Raises IllegalCastException if the service is not of the requested type.
template <typename SubType> inline SubType* cast(IService* service) {
  ensureIsA(service, typeOf<SubType>::get());
  return static_cast<SubType*>(service);
}

}  // namespace co

// Outputs the name of a TypeKind.
inline std::ostream& operator<<(std::ostream& out, co::TypeKind kind) {
  return out << co::TK_STRINGS[kind];
}

#endif  // _CO_TYPETRAITS_H_

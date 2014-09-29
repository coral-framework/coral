#ifndef _CO_PRIVATE_REFLECTORBASE_H_
#define _CO_PRIVATE_REFLECTORBASE_H_

#include "ComponentBase.h"
#include <co/IField.h>
#include <co/IMethod.h>
#include <co/IInterface.h>
#include <co/IReflector.h>
#include <co/IllegalArgumentException.h>
#include <sstream>

namespace co {

class IllegalCastException;

//! ReflectorBase provides a facet named 'reflector', of type co::IReflector.
class CORAL_EXPORT ReflectorBase_co_Reflector : public IReflector {
 public:
  virtual IInterface* getInterface();
  virtual IPort* getFacet();
};

/*!
  Inherit from this class to implement a IReflector.
 */
class CORAL_EXPORT ReflectorBase : public ComponentBase,
                                   public ReflectorBase_co_Reflector {
 public:
  ReflectorBase();
  virtual ~ReflectorBase();

  // co::IService methods:
  IObject* getProvider();
  void serviceRetain();
  void serviceRelease();

  // co::IObject methods:
  IComponent* getComponent();
  IService* getServiceAt(IPort*);
  void setServiceAt(IPort*, IService*);

  // co::IReflector methods:
  void createValues(void* ptr, size_t numValues);
  void copyValues(const void* fromPtr, void* toPtr, size_t numValues);
  void destroyValues(void* ptr, size_t numValues);
  bool compareValues(const void* a, const void* b, size_t numValues);
  IObject* newInstance();
  IService* newDynamicProxy(IDynamicServiceProvider* handler);
  void getField(const Any& instance, IField* ai, const Any& value);
  void setField(const Any& instance, IField* ai, const Any& value);
  void invoke(const Any& instance, IMethod* mi, Slice<Any> args,
              const Any& returnValue);
  void raise(const std::string& message);

 protected:
  template <typename T> void callDestructor(T* address) { address->~T(); }

  //! Raises co::IllegalArgumentException if handler is NULL.
  void checkValidDynamicProvider(IDynamicServiceProvider* provider);

  //! Raises co::MissingInputException if \a numArgs is lesser than \a mi's
  // expected number of args.
  void checkNumArguments(IMethod* mi, size_t numArgs);

  //! Raises an exception because setField() was called on a read-only field.
  void raiseFieldIsReadOnly(IField* ai);

  //! Re-raises a co::IllegalCastException with info about which method
  // parameter raised the exception.
  void raiseArgumentTypeException(IMethod* mi, int argIndex,
                                  const IllegalCastException& e);

  /*!
    Raises a co::IllegalArgumentException for cases (that "should never happen")
    where a member index is out of range.
   */
  void raiseUnexpectedMemberIndex();

  //! Raises co::NotSupportedException, e.g. for nonimplemented methods.
  void raiseNotSupportedException();
};

/****************************************************************************/
/* Auxiliary templates used internally by the generated reflectors          */
/****************************************************************************/

CORAL_EXPORT void* checkInstance(const Any& any, ICompositeType* ct,
                                 IMember* member);

template <typename T> T* checkInstance(const Any& any, IMember* member) {
  return reinterpret_cast<T*>(checkInstance(any, typeOf<T>::get(), member));
}

}  // namespace co

#endif  // _CO_PRIVATE_REFLECTORBASE_H_

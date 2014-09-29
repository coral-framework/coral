#ifndef _CO_PRIVATE_REFCOUNTED_H_
#define _CO_PRIVATE_REFCOUNTED_H_

#include <co/Platform.h>
#include <co/RefPtr.h>

namespace co {

/*!
  \brief Reusable base class for objects that should be reference counted.
  For use in conjunction with co::RefPtr.
 */
class CORAL_EXPORT RefCounted {
 public:
  RefCounted() : _refCount(0) { ; }

  RefCounted(const RefCounted&) : _refCount(0) { ; }

  inline RefCounted& operator=(const RefCounted&) { return *this; }

  inline void serviceRetain() const { ++_refCount; }

  inline void serviceRelease() const {
    if (--_refCount <= 0) delete this;
  }

  inline int32 getRefCount() const { return _refCount; }

 protected:
  virtual ~RefCounted();

 private:
  mutable int32 _refCount;
};

}  // namespace co

#endif  // _CO_PRIVATE_REFCOUNTED_H_

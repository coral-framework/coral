#include "TestHelper.h"
#include <co/RefPtr.h>
#include <co/ISystem.h>
#include <co/INamespace.h>
#include <co/ITypeManager.h>
#include <co/ITypeTransaction.h>
#include <co/private/StringTokenizer.h>
#include <gtest/gtest.h>

co::ITypeBuilder* TestHelper::createBuilder(co::TypeKind kind,
                                            const std::string& fullTypeName) {
  co::INamespace* ns = co::getSystem()->getTypes()->getRootNS();

  std::string current, next;
  co::StringTokenizer st(fullTypeName, ".");
  st.getNext(current);
  while (st.getNext(next)) {
    co::INamespace* childNS = ns->findChildNamespace(current);
    if (!childNS) childNS = ns->defineChildNamespace(current);
    ns = childNS;
    std::swap(current, next);
  }

  co::ITypeBuilder* typeBuilder = ns->defineType(current, kind);
  EXPECT_TRUE(typeBuilder != NULL);

  return typeBuilder;
}

co::IType* TestHelper::type(const std::string& fullTypeName) {
  return co::getSystem()->getTypes()->findType(fullTypeName);
}

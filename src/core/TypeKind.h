#ifndef _CO_TYPEKIND_H_
#define _CO_TYPEKIND_H_

namespace co {

enum TypeKind {
  TK_NULL,
  TK_BOOL,
  TK_INT8,
  TK_INT16,
  TK_INT32,
  TK_UINT8,
  TK_UINT16,
  TK_UINT32,
  TK_FLOAT,
  TK_DOUBLE,
  TK_ENUM,
  TK_STRING,
  TK_ANY,
  TK_ARRAY,
  TK_STRUCT,
  TK_NATIVECLASS,
  TK_INTERFACE,
  TK_COMPONENT,
  TK_EXCEPTION,
  TK_COUNT
};

}  // namespace co

#endif  // _CO_TYPEKIND_H_

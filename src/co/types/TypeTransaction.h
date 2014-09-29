#ifndef _CO_TYPETRANSACTION_H_
#define _CO_TYPETRANSACTION_H_

#include "TypeTransaction_Base.h"
#include <co/ITypeBuilder.h>

namespace co {
	
class TypeBuilder;
class TypeManager;

/*!
	Implements co.ITypeTransaction.
 */
class TypeTransaction : public TypeTransaction_Base
{
public:
	TypeTransaction();
	virtual ~TypeTransaction();

	// internal methods:
	void addTypeBuilder( ITypeBuilder* typeBuilder );

	// ITypeTransaction methods:
	TSlice<ITypeBuilder*> getTypeBuilders();
	void commit();
	void rollback();

private:
	void reset();

private:
	bool _commitAttempted;
	bool _commitSucceeded;
	bool _rolledBack;

	typedef std::vector<ITypeBuilderRef> TypeBuilderList;
	TypeBuilderList _builders;
};

} // namespace co

#endif

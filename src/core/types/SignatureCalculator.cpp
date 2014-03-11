#include "SignatureCalculator.h"
#include <co/Uuid.h>
#include <co/IEnum.h>
#include <co/IArray.h>
#include <co/IMethod.h>
#include <co/IStruct.h>
#include <co/ITypeManager.h>
#include <co/IInterface.h>
#include <co/IField.h>
#include <co/IParameter.h>
#include <co/IComponent.h>
#include <co/IPort.h>
#include <co/INativeClass.h>

namespace co {

namespace {
	/*
		Changing this constant will automatically change ALL signatures.
		Use this to signal type incompatibilities in new Coral versions.
	 */
	const uint8 SIGNATURE_ALGORITHM_VERSION = 1;

	// A byte to clearly delimiter an element and avoid ambiguities
	const uint8 DELIMITER = ';';
}

SignatureCalculator::SignatureCalculator( IType* type ) : _type( type ),
	_fullSignatureHash( CryptoHash::Sha1 ), _binarySignatureHash( CryptoHash::Sha1 )
{
	assert( _type );
}

SignatureCalculator::~SignatureCalculator()
{
	// empty
}

void SignatureCalculator::calculate()
{
	calculateSignatures();
}

inline void getUuidFromHash( CryptoHash& hash, Uuid& uuid )
{
	CryptoHash::Result result;
	hash.getResult( result );
	result.toUuid( uuid );
}

void SignatureCalculator::calculateSignatures()
{
	addDataToSignatures( SIGNATURE_ALGORITHM_VERSION );

	const uint8 kind = _type->getKind();
	addDataToSignatures( kind );

	_fullSignatureHash.addData( _type->getFullName() );

	switch( _type->getKind() )
	{
	case TK_BOOL:			break;
	case TK_INT8:			break;
	case TK_INT16:			break;
	case TK_INT32:			break;
	case TK_UINT8:			break;
	case TK_UINT16:			break;
	case TK_UINT32:			break;
	case TK_FLOAT:			break;
	case TK_DOUBLE:			break;
	case TK_ENUM:			fillEnumSignatureData(); break;
	case TK_STRING:			break;
	case TK_ANY:			break;
	case TK_ARRAY:			fillArraySignatureData(); break;
	case TK_STRUCT:			fillStructSignatureData(); break;
	case TK_NATIVECLASS:	fillNativeClassSignatureData(); break;
	case TK_INTERFACE:		fillInterfaceSignatureData(); break;
	case TK_COMPONENT:		fillComponentSignatureData(); break;
	case TK_EXCEPTION:		break;
	default:
		assert( false );
	}

	getUuidFromHash( _fullSignatureHash, _fullSignature );
	getUuidFromHash( _binarySignatureHash, _binarySignature );
}

void SignatureCalculator::fillArraySignatureData()
{
	IArray* array = static_cast<IArray*>( _type );
	IType* elementType = array->getElementType();
	_fullSignatureHash.addData( elementType->getFullSignature() );
	_binarySignatureHash.addData( elementType->getBinarySignature() );
}

void SignatureCalculator::fillEnumSignatureData()
{
	IEnum* enumType = static_cast<IEnum*>( _type );
	TSlice<std::string> range = enumType->getIdentifiers();
	for( ; range; range.popFirst() )
	{
		addDataToSignatures( range.getFirst() );

		// add a delimiter to avoid ambiguities
		addDataToSignatures( DELIMITER );
	}
}

void SignatureCalculator::fillStructSignatureData()
{
	IStruct* structType = static_cast<IStruct*>( _type );
	TSlice<IField*> fields = structType->getFields();
	for( ; fields; fields.popFirst() )
	{
		IField* ai = fields.getFirst();
		assert( !ai->getIsReadOnly() );

		IType* type = ai->getType();
		_fullSignatureHash.addData( type->getFullSignature() );
		_binarySignatureHash.addData( type->getBinarySignature() );

		_fullSignatureHash.addData( ai->getName() );
	}
}

void SignatureCalculator::fillNativeClassSignatureData()
{
	INativeClass* nativeType = static_cast<INativeClass*>( _type );
	CORAL_UNUSED( nativeType );
	fillClassTypeData();
}

void SignatureCalculator::fillInterfaceSignatureData()
{
	_binarySignatureHash.addData( _type->getFullName() );

	// add the baseType signature
	IInterface* baseType = static_cast<IInterface*>( _type )->getBaseType();
	if( baseType )
	{
		_fullSignatureHash.addData( baseType->getFullSignature() );
		_binarySignatureHash.addData( baseType->getBinarySignature() );
	}

	fillClassTypeData();
}

void SignatureCalculator::fillComponentSignatureData()
{
	IComponent* component = static_cast<IComponent*>( _type );

	TSlice<IPort*> interfaces = component->getPorts();
	for( ; interfaces; interfaces.popFirst() )
	{
		IPort* itf = interfaces.getFirst();

		const uint8 isFacet = itf->getIsFacet() ? 1 : 0;
		addDataToSignatures( isFacet );

		IType* type = itf->getType();
		_fullSignatureHash.addData( type->getFullSignature() );
		_binarySignatureHash.addData( type->getBinarySignature() );

		addDataToSignatures( itf->getName() );
	}
}

void SignatureCalculator::fillClassTypeData()
{
	IClassType* classType = static_cast<IClassType*>( _type );

	TSlice<IField*> fields = classType->getFields();
	for( ; fields; fields.popFirst() )
	{
		IField* ai = fields.getFirst();

		const uint8 isReadOnly = ai->getIsReadOnly() ? 1 : 0;
		addDataToSignatures( isReadOnly );

		addTypeWithNoSignature( ai->getType() );
		addDataToSignatures( ai->getName() );
	}

	TSlice<IMethod*> methods = classType->getMethods();
	for( ; methods; methods.popFirst() )
	{
		IMethod* mi = methods.getFirst();

		IType* returnType = mi->getReturnType();
		if( returnType )
			addTypeWithNoSignature( returnType );
		else
			addDataToSignatures( Uuid::null() );

		addDataToSignatures( mi->getName() );

		TSlice<IParameter*> parameters = mi->getParameters();
		for( ; parameters; parameters.popFirst() )
		{
			IParameter* pi = parameters.getFirst();

			const uint8 isIn = pi->getIsIn() ? 1 : 0;
			addDataToSignatures( isIn );

			const uint8 isOut = pi->getIsOut() ? 1 : 0;
			addDataToSignatures( isOut );

			addTypeWithNoSignature( pi->getType() );

			_fullSignatureHash.addData( pi->getName() );
		}
	}
}

void SignatureCalculator::addTypeWithNoSignature( IType* type )
{
	const uint8 kind = type->getKind();
	addDataToSignatures( kind );
	addDataToSignatures( type->getFullName() );
}

} // namespace co

#include "TestHelper.h"

#include <co/Coral.h>
#include <co/IEnum.h>
#include <co/IPort.h>
#include <co/RefPtr.h>
#include <co/IField.h>
#include <co/IArray.h>
#include <co/IStruct.h>
#include <co/ISystem.h>
#include <co/IComponent.h>
#include <co/INamespace.h>
#include <co/IInterface.h>
#include <co/INativeClass.h>
#include <co/ITypeBuilder.h>
#include <co/ITypeManager.h>
#include <co/IMethodBuilder.h>
#include <co/ITypeTransaction.h>
#include <co/IllegalNameException.h>
#include <co/MissingInputException.h>
#include <co/IllegalArgumentException.h>
#include <co/NotSupportedException.h>

#include <gtest/gtest.h>

// General tests:

TEST( TypeBuilderTests, cyclicDependencies )
{
	co::RefPtr<co::ITypeBuilder> structABuilder = TestHelper::createBuilder( co::TK_STRUCT, "ciclicDepTest.structs.A" );

	// type struct A depends on type struct B, which must be loaded if not loaded yet
	// check if B exists : B should not exist yet
	EXPECT_TRUE( TestHelper::type( "ciclicDepTest.structs.B" ) == NULL );

	// create B
	co::RefPtr<co::ITypeBuilder> structBBuilder = TestHelper::createBuilder( co::TK_STRUCT, "ciclicDepTest.structs.B" );

	// B struct depends on type A, creating a dependency cycle
	// check if A exists: although type A hasn't finished being created, it must be
	// available from the moment its builder is created, so its type should exist by now
	co::IType* typeA = TestHelper::type( "ciclicDepTest.structs.A" );

	EXPECT_TRUE( typeA != NULL );

	EXPECT_NO_THROW( structBBuilder->defineField( "fieldOfTypeA", typeA, false ) );

	// createType the B type
	co::IType* typeB = structBBuilder->createType();
	EXPECT_NO_THROW( structABuilder->defineField( "fieldOfTypeB", typeB, false ) );

	// create type A
	co::IType* createdTypeA = structABuilder->createType();

	EXPECT_TRUE( typeA == createdTypeA );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}


TEST( TypeBuilderTests, typeNameInvalidIdentifier )
{
	EXPECT_THROW( co::getSystem()->getTypes()->getRootNS()->defineType( "A.Dot.Separated.TypeName", co::TK_STRUCT ), co::IllegalNameException );
}

// IComponent:

TEST( TypeBuilderTests, componentInvalidDefinitions )
{
	// a dummy type to be used in the test
	co::IType* anyType = co::getSystem()->getTypes()->findType( "any" );

	// create builder
	co::RefPtr<co::ITypeBuilder> cbuilder = TestHelper::createBuilder( co::TK_COMPONENT, "TypeBuilderTests_aComponent" );

	// a dummy interface to be used in the tests
	co::RefPtr<co::ITypeBuilder> auxbuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_componentInterface" );
	auxbuilder->defineField( "test", anyType, true );

	co::IInterface* testInterface = co::cast<co::IInterface>( auxbuilder->createType() );

	// invalid component class definition
	EXPECT_THROW( cbuilder->defineIdentifier( "identifier" ), co::NotSupportedException );
	EXPECT_THROW( cbuilder->defineField( "fieldName", anyType, false ), co::NotSupportedException );
	EXPECT_THROW( cbuilder->defineBaseType( testInterface ), co::NotSupportedException );
	EXPECT_THROW( cbuilder->definePort( "testIntMember", NULL, false ), co::IllegalArgumentException );
	EXPECT_THROW( cbuilder->defineMethod( "testMethod" ), co::NotSupportedException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeBuilderTests, componentDefinition )
{
	co::RefPtr<co::ITypeBuilder> componentBuilder = TestHelper::createBuilder( co::TK_COMPONENT, "TypeBuilderTests.IComponent" );
	co::RefPtr<co::ITypeBuilder> interfaceBuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests.TestInterfaceType" );

	interfaceBuilder->defineField( "testName", TestHelper::type( "string" ), true);

	co::IInterface* interfaceTest = co::cast<co::IInterface>( interfaceBuilder->createType() );

	componentBuilder->definePort( "Receptacle", interfaceTest, false );
	componentBuilder->definePort( "Facet", interfaceTest, true );

	EXPECT_THROW( componentBuilder->definePort( "NullInterface", NULL, false ), co::IllegalArgumentException );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	co::IComponent* component = co::cast<co::IComponent>( componentBuilder->createType() );
	ASSERT_TRUE( component != NULL );

	ASSERT_TRUE( component->getPorts().getSize() == 2 );

	co::IMember* memberInfo = component->getMember( "Receptacle" );
	ASSERT_TRUE( memberInfo != NULL );
	co::IPort* receptacle = co::cast<co::IPort>( memberInfo );
	ASSERT_TRUE( receptacle != NULL );

	memberInfo = component->getMember( "Facet" );
	ASSERT_TRUE( memberInfo != NULL );
	co::IPort* facet = co::cast<co::IPort>( memberInfo );
	ASSERT_TRUE( facet != NULL );

	ASSERT_TRUE( receptacle->getName() == "Receptacle" );
	ASSERT_TRUE( receptacle->getType() != NULL );
	ASSERT_TRUE( receptacle->getType() == interfaceTest );
	ASSERT_TRUE( receptacle->getIsFacet() == false );

	ASSERT_TRUE( facet->getName() == "Facet" );
	ASSERT_TRUE( facet->getType() != NULL );
	ASSERT_TRUE( facet->getType() == interfaceTest );
	ASSERT_TRUE( facet->getIsFacet() == true );
}

TEST( TypeBuilderTests, componentMissingInput )
{
	co::RefPtr<co::ITypeBuilder> componentBuilder = TestHelper::createBuilder( co::TK_COMPONENT, "TypeBuilderTests.InvalidComponentType" );

	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::MissingInputException );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );

	EXPECT_TRUE( TestHelper::type( "TypeBuilderTests.InvalidComponentType" ) == NULL );
}

TEST( TypeBuilderTests, componentGetInterfaces )
{
	co::RefPtr<co::ITypeBuilder> componentBuilder = TestHelper::createBuilder( co::TK_COMPONENT, "ComponentTests.secondComponentType" );
	co::RefPtr<co::ITypeBuilder> interfaceBuilder = TestHelper::createBuilder( co::TK_INTERFACE, "ComponentTests.secondTestInterfaceType" );

	interfaceBuilder->defineField( "testName",  TestHelper::type( "string" ), true);
	co::IInterface* interfaceTest = co::cast<co::IInterface>( interfaceBuilder->createType() );

	componentBuilder->definePort( "Client_1", interfaceTest, false );
	componentBuilder->definePort( "Provid_1", interfaceTest, true );
	componentBuilder->definePort( "Provid_2", interfaceTest, true );
	componentBuilder->definePort( "Client_2", interfaceTest, false );
	componentBuilder->definePort( "Provid_3", interfaceTest, true );
	componentBuilder->definePort( "Client_3", interfaceTest, false );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	co::IComponent* component = co::cast<co::IComponent>( componentBuilder->createType() );
	ASSERT_TRUE( component != NULL );

	ASSERT_TRUE( component->getPorts().getSize() == 6 );

	co::TSlice<co::IPort*> facets = component->getFacets();
	ASSERT_TRUE( facets.getSize() == 3 );
	for( ; facets; facets.popFirst() )
		ASSERT_TRUE( facets.getFirst()->getIsFacet() );

	co::TSlice<co::IPort*> receptacles = component->getReceptacles();
	ASSERT_TRUE( receptacles.getSize() == 3 );
	for( ; receptacles; receptacles.popFirst() )
		ASSERT_FALSE( receptacles.getFirst()->getIsFacet() );
}

// IEnum:

TEST( TypeBuilderTests, enumInvalidDefinition )
{
	co::RefPtr<co::ITypeBuilder> ebuilder = TestHelper::createBuilder( co::TK_ENUM, "TypeBuilderTests_anEnum" );

	// dummy type to be used in the test
	co::IType* anyType = co::getSystem()->getTypes()->findType( "any" );

	// a dummy interface to be used in the tests
	co::RefPtr<co::ITypeBuilder> auxbuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_enumSuperInterface" );
	auxbuilder->defineField( "test", anyType, true );

	co::IInterface* testInterface = co::cast<co::IInterface>( auxbuilder->createType() );

	EXPECT_THROW( ebuilder->defineIdentifier( "3identifier" ), co::IllegalNameException );
	EXPECT_THROW( ebuilder->defineField( "fieldName", anyType, false ), co::NotSupportedException );
	EXPECT_THROW( ebuilder->defineBaseType( testInterface ), co::NotSupportedException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeBuilderTests, enumDefinition )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "EnumTests.BuilderTestEnum" );

	EXPECT_THROW( builder->defineIdentifier( "" ), co::IllegalNameException );

	builder->defineIdentifier( "VALUE_0" );
	builder->defineIdentifier( "VALUE_1" );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	co::IType* type = builder->createType();
	ASSERT_TRUE( type != NULL );

	// check if the enum got the correct identifiers
	co::IEnum* enumType = co::cast<co::IEnum>( type );
	ASSERT_TRUE( enumType != NULL );

	EXPECT_EQ( enumType->getIdentifiers().getFirst(), "VALUE_0" );
	EXPECT_EQ( enumType->getIdentifiers().getLast(), "VALUE_1" );
}

TEST( TypeBuilderTests, enumMissingInput )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "EnumTests.MissingInputEnum" );

	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::MissingInputException );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );

	EXPECT_TRUE( TestHelper::type( "EnumTests.MissingInputEnum" )  == NULL );
}

TEST( TypeBuilderTests, enumCheckIdentifiers )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "EnumTests.CheckIdentifiersTestEnum" );

	builder->defineIdentifier( "VALUE_0" );
	builder->defineIdentifier( "VALUE_1" );
	builder->defineIdentifier( "VALUE_2" );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	co::IType* type = builder->createType();
	co::IEnum* enumType = co::cast<co::IEnum>( type );

	ASSERT_EQ( enumType->getValueOf( "" ) , -1 );
	ASSERT_EQ( enumType->getValueOf( "ND" ) , -1 );
	ASSERT_EQ( enumType->getValueOf( "VALUE_0" ) , 0 );
	ASSERT_EQ( enumType->getValueOf( "VALUE_1" ) , 1 );
	ASSERT_EQ( enumType->getValueOf( "VALUE_2" ) , 2 );
}

TEST( TypeBuilderTests, enumInsertDuplicateIdentifier )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "EnumTests.InsertDuplicateIdentifierTestEnum" );

	builder->defineIdentifier( "VALUE_0" );

	EXPECT_THROW( builder->defineIdentifier( "VALUE_0" ), co::IllegalNameException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

TEST( TypeBuilderTests, enumCheckValidIdentifiers )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_ENUM, "EnumTests.checkValidIdentifiersEnum" );

	EXPECT_NO_THROW( builder->defineIdentifier( "VALUE" ) );
	EXPECT_NO_THROW( builder->defineIdentifier( "LALALA_123_FFF" ) );
	EXPECT_NO_THROW( builder->defineIdentifier( "AnotherValue" ) );

	EXPECT_THROW( builder->defineIdentifier( "1VAL" ) , co::IllegalNameException );
	EXPECT_THROW( builder->defineIdentifier( "VAL!@#$" ) , co::IllegalNameException );
	EXPECT_THROW( builder->defineIdentifier( "VAL~" ) , co::IllegalNameException );
	EXPECT_THROW( builder->defineIdentifier( "" ) , co::IllegalNameException );
	EXPECT_THROW( builder->defineIdentifier( " VAL" ) , co::IllegalNameException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

// IException:

TEST( TypeBuilderTests, exceptionCreation )
{
	co::INamespace* rootNS = co::getSystem()->getTypes()->getRootNS();

	co::RefPtr<co::ITypeBuilder> builder = rootNS->defineType( "aTypeName", co::TK_EXCEPTION );
	ASSERT_TRUE( builder.isValid() );
	EXPECT_EQ( builder->getNamespace(), rootNS );
	EXPECT_EQ( builder->getKind(), co::TK_EXCEPTION );
	EXPECT_EQ( builder->getTypeName(), "aTypeName" );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

TEST( TypeBuilderTests, exceptionsAfterCreation )
{
	co::INamespace* rootNS = co::getSystem()->getTypes()->getRootNS();

	co::RefPtr<co::ITypeBuilder> builder = rootNS->defineType( "exceptionsAfterCreationTestType", co::TK_STRUCT );
	builder->defineField( "name", TestHelper::type( "string" ), false );

	EXPECT_NO_THROW( builder->createType() );

	// after the creation of the type, no changes can be made to the typeBuilder.
	EXPECT_THROW( builder->defineField( "fullName", TestHelper::type( "string" ), false ), co::NotSupportedException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

// IInterface:

TEST( TypeBuilderTests, interfaceInvalidDefinition )
{
	co::RefPtr<co::ITypeBuilder> ibuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_anInvalidInterface" );

	// dummy type to be used in the test
	co::IType* anyType = co::getSystem()->getTypes()->findType( "any" );

	// a dummy interface to be used in the tests
	co::RefPtr<co::ITypeBuilder> auxbuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_superInterface" );
	auxbuilder->defineField( "test", anyType, true );

	co::IInterface* testInterface = co::cast<co::IInterface>( auxbuilder->createType() );

	// invalid interface definitions
	EXPECT_THROW( ibuilder->defineIdentifier( "identifier" ), co::NotSupportedException );
	EXPECT_THROW( ibuilder->defineField( "2fieldName", anyType, false ), co::IllegalNameException );
	EXPECT_THROW( ibuilder->defineBaseType( NULL ), co::IllegalArgumentException );
	EXPECT_THROW( ibuilder->definePort( "testIntMember", testInterface, false ), co::NotSupportedException );
	EXPECT_THROW( ibuilder->defineMethod( "$testMethod" ), co::IllegalNameException );
	EXPECT_THROW( ibuilder->defineMethod( "" ), co::IllegalNameException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeBuilderTests, interfaceDefinition )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_INTERFACE, "builderTest.AInterfaceType" );

	// define builderTest.AInterfaceType:
	co::IType* stringType = TestHelper::type( "string" );
	EXPECT_NO_THROW( builder->defineField( "testName", stringType, true ) );
	EXPECT_NO_THROW( builder->defineField( "testSecondName", stringType, false ) );
	co::RefPtr<co::IMethodBuilder> mb = builder->defineMethod( "childMethod" );
	EXPECT_NO_THROW( mb->createMethod() );

	// define builderTest.ASuperInterfaceType:
	co::RefPtr<co::ITypeBuilder> superBuilder = TestHelper::createBuilder( co::TK_INTERFACE, "builderTest.ASuperInterfaceType" );
	superBuilder->defineField( "name", stringType, false );
	co::RefPtr<co::IMethodBuilder> mb2;
	EXPECT_NO_THROW( ( mb2 = superBuilder->defineMethod( "parentMethod" ) ) );
	EXPECT_NO_THROW( mb2->createMethod() );

	// get the super-type before even finishing creating it (it should be available)
	co::IInterface* super = co::cast<co::IInterface>( TestHelper::type( "builderTest.ASuperInterfaceType" ) );
	EXPECT_TRUE( super != NULL );

	// try inheriting from an incompatible type
	EXPECT_THROW( builder->defineBaseType( stringType ), co::IllegalArgumentException );

	// add the super-type to the interface type
	builder->defineBaseType( super );

	// try multiple inheritance
	EXPECT_THROW( builder->defineBaseType( builder->getInterface() ), co::NotSupportedException );

	EXPECT_NO_THROW( superBuilder->createType() );

	co::IInterface* itf = co::cast<co::IInterface>( builder->createType() );
	EXPECT_TRUE( itf != NULL );

	ASSERT_TRUE( itf->getMember( "name" ) != NULL );
	ASSERT_TRUE( itf->getMember( "parentMethod" ) != NULL );
	ASSERT_TRUE( itf->getMember( "childMethod" ) != NULL );
	ASSERT_TRUE( itf->getMember( "testName" ) != NULL );
	ASSERT_TRUE( itf->getMember( "testSecondName" ) != NULL );

	ASSERT_EQ( 1, itf->getMethods().getSize() );
	ASSERT_EQ( 2, itf->getFields().getSize() );

	ASSERT_EQ( itf->getBaseType(), super );
	ASSERT_EQ( itf->getSuperTypes().getSize(), 0 );
	ASSERT_EQ( super->getSubTypes().getSize(), 0 );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	ASSERT_EQ( itf->getSuperTypes().getSize(), 2 );
	ASSERT_EQ( itf->getSuperTypes().getFirst(), super );
	ASSERT_EQ( itf->getSubTypes().getSize(), 0 );

	ASSERT_EQ( super->getSuperTypes().getSize(), 1 );
	ASSERT_EQ( super->getSubTypes().getSize(), 1 );
	ASSERT_EQ( super->getSubTypes().getFirst(), itf );
}

TEST( TypeBuilderTests, interfaceGetMethods )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_INTERFACE, "interfaceGetMethodsTest.Foo" );

	builder->defineField( "test", TestHelper::type( "string" ), true );
	co::IInterface* itf = co::cast<co::IInterface>( builder->createType() );

	ASSERT_NO_THROW( itf->getMethods() );

	ASSERT_EQ( 0, itf->getMethods().getSize() );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

// INativeClass:

TEST( TypeBuilderTests, nativeClassInvalidDefinitions )
{
	// a dummy type to be used in the test
	co::IType* anyType = co::getSystem()->getTypes()->findType( "any" );

	// create builder
	co::RefPtr<co::ITypeBuilder> nbuilder = TestHelper::createBuilder( co::TK_NATIVECLASS, "TypeBuilderTests_anNativeClass" );

	// a dummy interface to be used in the tests
	co::RefPtr<co::ITypeBuilder> auxbuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_anInterface" );
	auxbuilder->defineField( "test", anyType, true );

	co::IInterface* testInterface = co::cast<co::IInterface>( auxbuilder->createType() );

	// invalid native class definition
	EXPECT_THROW( nbuilder->defineIdentifier( "identifier" ), co::NotSupportedException );
	EXPECT_THROW( nbuilder->defineField( "0_fieldName", anyType, false ), co::IllegalNameException );
	EXPECT_THROW( nbuilder->defineBaseType( testInterface ), co::NotSupportedException );
	EXPECT_THROW( nbuilder->definePort( "testIntMember", testInterface, false ), co::NotSupportedException );
	EXPECT_THROW( nbuilder->defineMethod( "" ), co::IllegalNameException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeBuilderTests, nativeClassDefinition )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_NATIVECLASS, "NativeClassbuilderTest.BuilderTestNativeClass" );

	builder->defineField( "testName", TestHelper::type( "string" ), true );
	builder->defineField( "age", TestHelper::type( "uint32" ), false );

	co::RefPtr<co::IMethodBuilder> mb = builder->defineMethod( "childMethod" );
	mb->createMethod();

	co::INativeClass* nativeType = co::cast<co::INativeClass>( builder->createType() );

	ASSERT_TRUE( nativeType->getMember( "testName" ) != NULL );
	ASSERT_TRUE( nativeType->getMember( "age" ) != NULL );
	ASSERT_TRUE( nativeType->getMember( "childMethod" ) != NULL );
	ASSERT_EQ( 1, nativeType->getMethods().getSize() );
	ASSERT_EQ( 2, nativeType->getFields().getSize() );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );
}

TEST( TypeBuilderTests, nativeClassMemberClash )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_NATIVECLASS, "NativeClassbuilderTest.MemberClashTestNativeClass" );

	builder->defineField( "testName", TestHelper::type( "string" ), true );

	co::RefPtr<co::IMethodBuilder> mb = builder->defineMethod( "getTestName" );
	mb->createMethod();

	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::IllegalNameException );
	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

// IStruct:

TEST( TypeBuilderTests, structInvalidDefinition )
{
	co::RefPtr<co::ITypeBuilder> sbuilder = TestHelper::createBuilder( co::TK_STRUCT, "TypeBuilderTests_anStruct" );

	// dummy type to be used in the test
	co::IType* anyType = co::getSystem()->getTypes()->findType( "any" );

	// a dummy interface to be used in the tests
	co::RefPtr<co::ITypeBuilder> auxbuilder = TestHelper::createBuilder( co::TK_INTERFACE, "TypeBuilderTests_anInterface" );
	auxbuilder->defineField( "test", anyType, true );

	co::IInterface* testInterface = co::cast<co::IInterface>( auxbuilder->createType() );

	// invalid struct definitions
	EXPECT_THROW( sbuilder->defineIdentifier( "identifier" ), co::NotSupportedException );
	EXPECT_THROW( sbuilder->defineField( "1fieldName", anyType, false ), co::IllegalNameException );
	EXPECT_THROW( sbuilder->defineBaseType( testInterface ), co::NotSupportedException );
	EXPECT_THROW( sbuilder->definePort( "testIntMember", testInterface, false ), co::NotSupportedException );
	EXPECT_THROW( sbuilder->defineMethod( "testMethod" ), co::NotSupportedException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );
}

TEST( TypeBuilderTests, structMissingInput )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_STRUCT, "aStructType" );

	EXPECT_THROW( co::getSystem()->getTypes()->getTransaction()->commit(), co::MissingInputException );

	EXPECT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->rollback() );

	EXPECT_TRUE( TestHelper::type( "aStructType" ) == NULL );
}

TEST( TypeBuilderTests, structDefinition )
{
	co::RefPtr<co::ITypeBuilder> builder = TestHelper::createBuilder( co::TK_STRUCT, "StructTests.BuidlerTestStruct" );

	co::IType* uint32Type = TestHelper::type( "uint32" );

	builder->defineField( "myField", uint32Type, false );

	EXPECT_THROW( builder->defineField( "myNullTypeField", NULL, false ), co::IllegalArgumentException );
	EXPECT_THROW( builder->defineField( "myReadOnlyField", uint32Type, true ), co::IllegalArgumentException );
	ASSERT_NO_THROW( co::getSystem()->getTypes()->getTransaction()->commit() );

	co::IStruct* structType = co::cast<co::IStruct>( TestHelper::type( "StructTests.BuidlerTestStruct" ) );
	ASSERT_TRUE( structType != NULL );

	co::TSlice<co::IField*> attibutes = structType->getFields();
	ASSERT_TRUE( attibutes.getSize() == 1 );

	co::IField* attr = attibutes.getFirst() ;

	ASSERT_TRUE( attr->getName() == "myField" );
	ASSERT_TRUE( attr->getType() == uint32Type );
	ASSERT_TRUE( attr->getIsReadOnly() == false );
}

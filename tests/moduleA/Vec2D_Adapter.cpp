#include "Vec2D_Adapter.h"
#include <cmath>

double moduleA::Vec2D_Adapter::getX( moduleA::Vec2D& self )
{
	return self.x;
}

void moduleA::Vec2D_Adapter::setX( moduleA::Vec2D& self, double x )
{
	self.x = x;
}

double moduleA::Vec2D_Adapter::getY( moduleA::Vec2D& self )
{
	return self.y;
}

void moduleA::Vec2D_Adapter::setY( moduleA::Vec2D& self, double y )
{
	self.y = y;
}

double moduleA::Vec2D_Adapter::getLength( moduleA::Vec2D& self )
{
	return sqrt( getLength2( self ) );
}

double moduleA::Vec2D_Adapter::getLength2( moduleA::Vec2D& self )
{
	return self.x * self.x + self.y * self.y;
}

void moduleA::Vec2D_Adapter::set( moduleA::Vec2D& self, double x, double y )
{
	self.x = x;
	self.y = y;
}

void moduleA::Vec2D_Adapter::get( moduleA::Vec2D& self, double& x, double& y )
{
	x = self.x;
	y = self.y;
}

double moduleA::Vec2D_Adapter::dot( moduleA::Vec2D& self, const moduleA::Vec2D& other )
{
	return self.x * other.x + self.y * other.y;
}

bool moduleA::Vec2D_Adapter::equals( moduleA::Vec2D& self, const moduleA::Vec2D& other )
{
	return self.x == other.x && self.y == other.y;
}

moduleA::Vec2D moduleA::Vec2D_Adapter::testInOut( moduleA::Vec2D& self,
		const moduleA::Vec2D& a, moduleA::Vec2D& b, moduleA::Vec2D& c )
{
	c = b;
	b = a;
	return self;
}

co::IService* moduleA::Vec2D_Adapter::dummyMethod( moduleA::Vec2D&, const co::Any& in, co::AnyValue& out )
{
	out = in;
	return NULL;
}

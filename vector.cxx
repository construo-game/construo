/*
	$Id$

	------------------------------------------------------------------------
	ClanLib, the platform independent game SDK.

	This library is distributed under the GNU LIBRARY GENERAL PUBLIC LICENSE
	version 2. See COPYING for details.

	For a total list of contributers see CREDITS.

	------------------------------------------------------------------------

	1999/06/19 Daniel Vogel
	
	totally replaced old CL_Vector with this code
*/

#include <assert.h>
#include <math.h>
#include "vector.hxx"

CL_Vector::CL_Vector(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

CL_Vector::CL_Vector(const CL_Vector &other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
}

float CL_Vector::norm() const
{
	#ifdef WIN32
	return (float)sqrt(x*x+y*y+z*z);
	#else
	return sqrt(x*x+y*y+z*z);
	#endif
}

void CL_Vector::normalize()
{
	float f = norm();
	if (f!=0)
	{
		x /= f;
		y /= f;
		z /= f;
	}
}

float CL_Vector::dot(const CL_Vector& v) const
{
	return x*v.x + y*v.y + z*v.z;  
}

float CL_Vector::angle(const CL_Vector& v) const
{
	#ifdef WIN32
	return (float)acos(dot(v)/(norm()*v.norm()));  
	#else
	return std::acos(dot(v)/(norm()*v.norm()));  
	#endif
}

CL_Vector CL_Vector::cross(const CL_Vector& v) const
{
	CL_Vector tmp = CL_Vector(y * v.z - z * v.y,
				  z * v.x - x * v.z,
				  x * v.y - y * v.x);
	return tmp;  
}

// quick hack, same as glRotatef(angle, a);
CL_Vector CL_Vector::rotate(float angle, const CL_Vector& a) const
{
	CL_Vector tmp = CL_Vector();

	#ifdef WIN32
	float s = (float)sin(angle);
	float c = (float)cos(angle);
	#else
	float s = std::sin(angle);
	float c = std::cos(angle);
	#endif

	tmp.x = x*(a.x*a.x*(1-c)+c)     + y*(a.x*a.y*(1-c)-a.z*s) + z*(a.x*a.z*(1-c)+a.y*s);
	tmp.y = x*(a.y*a.x*(1-c)+a.z*s) + y*(a.y*a.y*(1-c)+c)     + z*(a.y*a.z*(1-c)-a.x*s);
	tmp.z = x*(a.x*a.z*(1-c)-a.y*s) + y*(a.y*a.z*(1-c)+a.x*s) + z*(a.z*a.z*(1-c)+c);
	return tmp;  
}

void CL_Vector::round()
{
	x = int(x+0.5f);
	y = int(y+0.5f);
	z = int(z+0.5f);
	w = int(w+0.5f);
}

CL_Vector CL_Vector::operator * (float s) const
{
	return CL_Vector(s * x,
		 	 s * y,
			 s * z,
			 s * w);
}

CL_Vector operator * (float s, const CL_Vector& v)
{
	return CL_Vector(s * v.x,
			 s * v.y,
			 s * v.z,
			 s * v.w);
}

void CL_Vector::operator += (const CL_Vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.z;
}

void CL_Vector::operator -= (const CL_Vector& v)
{
  	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
}

void CL_Vector::operator *= (float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}

CL_Vector CL_Vector::operator + (const CL_Vector& v) const
{
	return CL_Vector(x + v.x,
			 y + v.y,
			 z + v.z,
			 w + v.w);
}

CL_Vector CL_Vector::operator - (const CL_Vector& v) const
{
	return CL_Vector(x - v.x,
			 y - v.y,
			 z - v.z,
			 w - v.z);
}

CL_Vector CL_Vector::operator - () const
{
	return CL_Vector(-x,
			 -y,
			 -z,
			 -w);
}

CL_Vector& CL_Vector::operator = (const CL_Vector& v)
{ 
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}
 
int CL_Vector::operator == (const CL_Vector& v) const
{
	return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));
}

int CL_Vector::operator != (const CL_Vector& v) const
{
	return !(operator == (v));
}

float & CL_Vector::operator [] (int n)
{
	switch (n)
	{
		case 0:	return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
	}
	assert(false);
	return x;			// dummy
}

std::ostream& operator << (std::ostream& os, const CL_Vector& v)
{
	os << v.x << " " << v.y << " " << v.z;
	return os;
}

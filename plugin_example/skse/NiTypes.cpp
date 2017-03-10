#include "NiTypes.h"

NiPoint3::NiPoint3()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

NiPoint3 NiPoint3::operator- () const
{
	return NiPoint3(-x, -y, -z);
}

NiPoint3 NiPoint3::operator+ (const NiPoint3& pt) const
{
	return NiPoint3(x + pt.x, y + pt.y, z + pt.z);
}

NiPoint3 NiPoint3::operator- (const NiPoint3& pt) const
{
	return NiPoint3(x - pt.x, y - pt.y, z - pt.z);
}

NiPoint3& NiPoint3::operator+= (const NiPoint3& pt)
{
	x += pt.x;
	y += pt.y;
	z += pt.z;
	return *this;
}
NiPoint3& NiPoint3::operator-= (const NiPoint3& pt)
{
	x -= pt.x;
	y -= pt.y;
	z -= pt.z;
	return *this;
}

// Scalar operations
NiPoint3 NiPoint3::operator* (float scalar) const
{
	return NiPoint3(scalar * x, scalar * y, scalar * z);
}
NiPoint3 NiPoint3::operator/ (float scalar) const
{
	float invScalar = 1.0f / scalar;
	return NiPoint3(invScalar * x, invScalar * y, invScalar * z);
}

NiPoint3& NiPoint3::operator*= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}
NiPoint3& NiPoint3::operator/= (float scalar)
{
	float invScalar = 1.0f / scalar;
	x *= invScalar;
	y *= invScalar;
	z *= invScalar;
	return *this;
}

void NiMatrix33::Identity()
{
	content[0][0] = 1.0f;
	content[0][1] = 0.0f;
	content[0][2] = 0.0f;
	content[1][0] = 0.0f;
	content[1][1] = 1.0f;
	content[1][2] = 0.0f;
	content[2][0] = 0.0f;
	content[2][1] = 0.0f;
	content[2][2] = 1.0f;
}

NiMatrix33 NiMatrix33::operator* (const NiMatrix33& rhs) const
{
	NiMatrix33 tmp;
	tmp.content[0][0] =
		content[0][0]*rhs.content[0][0]+
		content[0][1]*rhs.content[1][0]+
		content[0][2]*rhs.content[2][0];
	tmp.content[1][0] =
		content[1][0]*rhs.content[0][0]+
		content[1][1]*rhs.content[1][0]+
		content[1][2]*rhs.content[2][0];
	tmp.content[2][0] =
		content[2][0]*rhs.content[0][0]+
		content[2][1]*rhs.content[1][0]+
		content[2][2]*rhs.content[2][0];
	tmp.content[0][1] =
		content[0][0]*rhs.content[0][1]+
		content[0][1]*rhs.content[1][1]+
		content[0][2]*rhs.content[2][1];
	tmp.content[1][1] =
		content[1][0]*rhs.content[0][1]+
		content[1][1]*rhs.content[1][1]+
		content[1][2]*rhs.content[2][1];
	tmp.content[2][1] =
		content[2][0]*rhs.content[0][1]+
		content[2][1]*rhs.content[1][1]+
		content[2][2]*rhs.content[2][1];
	tmp.content[0][2] =
		content[0][0]*rhs.content[0][2]+
		content[0][1]*rhs.content[1][2]+
		content[0][2]*rhs.content[2][2];
	tmp.content[1][2] =
		content[1][0]*rhs.content[0][2]+
		content[1][1]*rhs.content[1][2]+
		content[1][2]*rhs.content[2][2];
	tmp.content[2][2] =
		content[2][0]*rhs.content[0][2]+
		content[2][1]*rhs.content[1][2]+
		content[2][2]*rhs.content[2][2];
	return tmp;
}

NiMatrix33 NiMatrix33::operator* (float scalar) const
{
	NiMatrix33 result;
	result.content[0][0] = content[0][0] * scalar;
	result.content[0][1] = content[0][1] * scalar;
	result.content[0][2] = content[0][2] * scalar;
	result.content[1][0] = content[1][0] * scalar;
	result.content[1][1] = content[1][1] * scalar;
	result.content[1][2] = content[1][2] * scalar;
	result.content[2][0] = content[2][0] * scalar;
	result.content[2][1] = content[2][1] * scalar;
	result.content[2][2] = content[2][2] * scalar;
	return result;
}

NiPoint3 NiMatrix33::operator* (const NiPoint3& pt) const
{
	return NiPoint3
		(
		content[0][0]*pt.x+content[0][1]*pt.y+content[0][2]*pt.z,
		content[1][0]*pt.x+content[1][1]*pt.y+content[1][2]*pt.z,
		content[2][0]*pt.x+content[2][1]*pt.y+content[2][2]*pt.z
		);
}

NiTransform::NiTransform()
{
	rot.Identity();
	scale = 1.0f;
}

NiTransform NiTransform::operator*(const NiTransform &rhs) const
{
	NiTransform tmp;
	tmp.scale = scale * rhs.scale;
	tmp.rot = rot * rhs.rot;
	tmp.pos = pos + (rot * rhs.pos) * scale;
	return tmp;
}

NiPoint3 NiTransform::operator*(const NiPoint3 & pt) const
{
	return (((rot * pt) * scale) + pos);
}

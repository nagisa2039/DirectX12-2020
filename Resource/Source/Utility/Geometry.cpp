#include <cmath>
#include "Geometry.h"
#include <algorithm>
#include <vector>
#include "Utility/Cast.h"

const Vector2i Vector2i::ZERO(0, 0);
const Vector2f Vector2f::ZERO(0.0f, 0.0f);
const Vector2f Vector2f::NIL(NAN, NAN);
const Segment Segment::NIL(Vector2f::NIL, Vector2f::NIL);

using namespace std;

float Dot(const Vector2f & lval, const Vector2f & rval)
{
	return lval.x * rval.x + lval.y * rval.y;
}

float Cross(const Vector2f & lval, const Vector2f & rval)
{
	return lval.x * rval.y - lval.y * rval.x;
}

Rect::Rect():center(Vector2i()),size(Size())
{
}

Rect::Rect(const int x, const int y, const int w, const int h) : center(Vector2i(x,y)), size(Size(w,h))
{
}

Rect::Rect(const Vector2i & pos, const Size & sz) : center(Vector2i(pos)), size(Size(sz))
{
}

const int Rect::Left() const
{
	return center.x - size.w/2;
}

const int Rect::Top() const
{
	return center.y - size.h/2;
}

const int Rect::Right() const
{
	return center.x + size.w/2;
}

const int Rect::Botton() const
{
	return center.y + size.h/2;
}

bool Rect::IsHit(const Rect& rect)const
{
	return abs(center.x - rect.center.x) <= (size.w + rect.size.w) / 2.0f &&
		abs(center.y - rect.center.y) <= (size.h + rect.size.h) / 2.0f;
}

const bool Rect::IsHit(const Rect & aRect, const Rect & bRect)
{
	return abs(aRect.center.x - bRect.center.x) < (aRect.size.w + bRect.size.w) / 2 &&
		abs(aRect.center.y - bRect.center.y) <= (aRect.size.h + bRect.size.h) / 2;
}

const Size Rect::OverlapSize(const Rect & aRect, const Rect & bRect)
{
	int w = min(aRect.Right(), bRect.Right()) - max(aRect.Left(), bRect.Left());
	int h = min(aRect.Botton(), bRect.Botton()) - max(aRect.Top(), bRect.Top());
	return Size(w,h);
}

Circle::Circle():centor(0,0),radius(0)
{
}

Circle::Circle(const float x, const float y, const float r) : centor(x, y), radius(r)
{
}

Circle::Circle(const Vector2f& centor, const float r) : centor(centor), radius(r)
{
}

bool Circle::IsHit(const Circle& c) const
{
	auto pow2 = (this->radius + c.radius) * (this->radius + c.radius);
	return pow2 >= (this->centor - c.centor).SQLength();
}

Size Size::operator-(const Size& size)
{
	return Size(-size.w, -size.h);
}

void Size::operator*=(const float scale)
{
	this->w = static_cast<int>(w * scale);
	this->h = static_cast<int>(h * scale);
}

Vector3 Vector3::operator-(void)const
{
	return Vector3(-x,-y,-z);
}

void Vector3::operator+=(const Vector3 & val)
{
	this->x += val.x;
	this->y += val.y;
	this->z += val.z;
}

void Vector3::operator-=(const Vector3 & val)
{
	this->x -= val.x;
	this->y -= val.y;
	this->z -= val.z;
}

void Vector3::operator*=(const Vector3 & val)
{
	this->x *= val.x;
	this->y *= val.y;
	this->z *= val.z;
}

void Vector3::operator/=(const Vector3 & val)
{
	this->x /= val.x;
	this->y /= val.y;
	this->z /= val.z;
}

void Vector3::operator+=(float scale)
{
	this->x += scale;
	this->y += scale;
	this->z += scale;
}
void Vector3::operator-=(float scale)
{
	this->x -= scale;
	this->y -= scale;
	this->z -= scale;
}
void Vector3::operator*=(float scale)
{
	this->x *= scale;
	this->y *= scale;
	this->z *= scale;
}
void Vector3::operator/=(float scale)
{
	this->x /= scale;
	this->y /= scale;
	this->z /= scale;
}

float Vector3::Length() const
{
	return std::hypot((const float)this->x, (const float)this->y, (const float)this->z);
}

Vector3 Vector3::Normalize()
{
	auto len = this->Length();
	this->x/=len;
	this->y/=len;
	this->z/=len;
	return *this;
}

Vector3 Vector3::Normalized() const
{
	auto len = this->Length();
	return Vector3(this->x / len, this->y / len, this->z / len);
}

float Dot(const Vector3 & lval, const Vector3 & rval)
{
	return lval.x * rval.x + lval.y * rval.y + lval.z * rval.z;
}

Vector3 Cross(const Vector3 & lval, const Vector3 & rval)
{
	return Vector3(lval.y * rval.z - lval.z * rval.y, 
		lval.z * rval.x - lval.x * rval.z, 
		lval.x * rval.y - lval.y * rval.x);
}

Vector3 ReflectionVector(const Vector3 & baseVector, const Vector3 & norm)
{
	return baseVector + norm * (2 * Dot(baseVector*-1, norm));
}

Vector3 RefractionVector(const Vector3 & baseVector, const Vector3 & norm, float parsent)
{
	float vn = Dot(baseVector, norm);
	float d = 1 - pow((1 + vn), 2) / pow(parsent,2);
	return norm * (-vn/parsent - sqrtf(d)) + baseVector * (1 / parsent);
}

Vector3 Lerp(const Vector3 & start, const Vector3 & end, const float parsent)
{
	return start + (end - start) * parsent;
}

Vector3 operator+(const Vector3 & lval, const Vector3 & rval)
{
	return Vector3(lval.x + rval.x, lval.y + rval.y, lval.z + rval.z);
}

Vector3 operator-(const Vector3 & lval, const Vector3 & rval)
{
	return Vector3(lval.x - rval.x, lval.y - rval.y, lval.z - rval.z);
}

Vector3 operator*(const Vector3 & lval, const Vector3 & rval)
{
	return Vector3(lval.x * rval.x, lval.y * rval.y, lval.z * rval.z);
}

Vector3 operator/(const Vector3 & lval, const Vector3 & rval)
{
	return Vector3(lval.x / rval.x, lval.y / rval.y, lval.z / rval.z);
}

Vector3 operator+(const Vector3 & lval, const float & rval)
{
	return Vector3(lval.x + rval, lval.y + rval, lval.z + rval);
}

Vector3 operator-(const Vector3 & lval, const float & rval)
{
	return Vector3(lval.x - rval, lval.y - rval, lval.z - rval);
}

Vector3 operator*(const Vector3 & lval, const float & rval)
{
	return Vector3(lval.x * rval, lval.y * rval, lval.z * rval);
}

Vector3 operator/(const Vector3 & lval, const float & rval)
{
	return Vector3(lval.x / rval, lval.y / rval, lval.z / rval);
}

Vector3 XMVECTORtoVec3(DirectX::XMVECTOR & vec)
{
	DirectX::XMFLOAT3 f3;
	DirectX::XMStoreFloat3(&f3, vec);
	return Vector3(f3.x, f3.y, f3.z);
}

float Lerp(const float lval, const float rval, const float parsent)
{
	return lval + (rval - lval) * parsent;
}

DirectX::XMFLOAT2 Lerp(const DirectX::XMFLOAT2 lval, const DirectX::XMFLOAT2 rval, const float parsent)
{
	return DirectX::XMFLOAT2(Lerp(lval.x, rval.y, parsent), Lerp(lval.y, rval.y, parsent));
}

float TriangleArea(const Vector3 & p0, const Vector3 & p1, const Vector3 & p2)
{
	// éOï”ÇÃí∑Ç≥ÇãÅÇﬂÇÈ
	auto e0 = (p1 - p0).Length();
	auto e1 = (p2 - p0).Length();
	auto e2 = (p2 - p1).Length();

	// ÉwÉçÉìÇÃåˆéÆÇ≈ñ êœÇåvéZ
	auto s = (e0 + e1 + e2) / 2.0f;

	auto area = sqrtf(s*(s - e0)*(s - e1)*(s - e2));
	return area;
}

Size operator+(const Size& lval, const Size& rval)
{
	return Size(lval.w + rval.w, lval.h + rval.h);
}

Size operator*(const Size& lval, const Size& rval)
{
	return Size(lval.w * rval.w, lval.h * rval.h);
}

Size operator/(const Size& lval, const Size& rval)
{
	return Size(lval.w / rval.w, lval.h / rval.h);
}

Size operator/(const Size& lval, const Vector2i& rval)
{
	return Size(lval.w / rval.x, lval.h / rval.y);
}

Size operator+(const Size& lval, const int& rval)
{
	return Size(lval.w + rval, lval.h + rval);
}

Size operator*(const Size& lval, const float& rval)
{
	return Size(static_cast<int>(lval.w * rval), static_cast<int>(lval.h * rval));
}

bool operator==(const Size& lval, const Size& rval)
{
	return lval.w == rval.w && lval.h == rval.h;
}

float Clamp(const float in, const float min, const float max)
{
	return std::clamp(in, min, max);
}

Vector3 Clamp(const Vector3 & in, const float min, const float max)
{
	return Vector3(Clamp(in.x, min, max), Clamp(in.y, min, max), Clamp(in.z, min, max));
}

bool operator==(const Range& lv, const Range& rv)
{
	return lv.min == rv.min && lv.max == rv.max;
}

Range::Range()
{
	this->min = 0;
	this->max = 0;
}

Range::Range(const int min, const int max)
{
	this->min = min;
	this->max = max;
}

bool Range::Hit(const int value) const
{
	return this->min <= value && this->max >= value;
}

Range Range::GetCriticalRange(const Range& target) const
{
	Range critical = *this;
	if (target == *this)
	{
		return Range(0,0);
	}

	bool start = false;
	for (int i = this->min; i <= this->max; i++)
	{
		if (!target.Hit(i))
		{
			start = true;
			critical.min = i;
		}
		else
		{
			if (start)
			{
				critical.max = i - 1;
				return critical;
			}
		}
	}
	return start ? critical : Range(0,0);
}

bool Capsule::IsHit(const Circle& circle) const
{
	auto startToEndN = segment.vec.Normalized();
	auto startToEndLen = segment.vec.Length();
	auto startToCircle = (circle.centor - segment.start);

	auto shaeiLen = Dot(startToCircle, startToEndN);

	// çsÇ´âﬂÇ¨ëŒçÙ
	shaeiLen = Clamp(shaeiLen, 0.0f, startToEndLen);
	auto suisenLen = (circle.centor - (segment.start + startToEndN * shaeiLen)).Length();

	return (circle.radius + radius) >= suisenLen;
}

Vector2f Capsule::End() const
{
	return segment.start + segment.vec;
}

Vector2f Segment::End() const
{
	return Vector2f(start + vec);
}

bool Segment::IsNil() const
{
	return isnan(vec.x) || isnan(vec.y);
}

float Segment::GetY(const float x) const
{
	if (IsNil())return NAN;
	return start.y + vec.y * (x - start.x) / vec.x;
}

float Segment::GetX(const float y) const
{
	if (IsNil())return NAN;
	return start.x + vec.x * (y - start.y) / vec.y;
}
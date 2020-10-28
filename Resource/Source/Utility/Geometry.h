#pragma once
#include <DirectXMath.h>
#include <cmath>

struct Size;

template<typename T>
struct Vector2
{
	T x, y;
	Vector2() : x(0), y(0) {}
	Vector2(T x, T y): x(x), y(y) {}	

	inline Vector2<T> operator-()
	{
		return Vector2<T>(-x, -y);
	}
	inline void operator*=(const float scale)
	{
		x *= scale;
		y *= scale;
	}
	inline void operator=(const Vector2<T>& value)
	{
		x = value.x;
		y = value.y;
	}
	inline void operator+=(const Vector2<T>& value)
	{
		x += value.x;
		y += value.y;
	}
	inline void operator-=(const Vector2<T>& value)
	{
		x -= value.x;
		y -= value.y;
	}
	inline void operator*=(const Vector2<T>& value)
	{
		x *= value.x;
		y *= value.y;
	}
	inline void operator/=(const Vector2<T>& value)
	{
		x /= value.x;
		y /= value.y;
	}
	inline float Length()const
	{
		if (x == 0 && y == 0)
		{
			return 0.0f;
		}
		return std::hypot(x, y);
	}
	// ベクトルの長さの2乗
	inline float SQLength()const
	{
		return x * x + y * y;
	}
	inline void Normal()
	{
		auto length = Length();
		x /= length;
		y /= length;
	}
	inline Vector2<T> Normalized()const
	{
		if (x == 0 && y == 0)
		{
			return Vector2<T>();
		}
		auto length = Length();
		return Vector2<T>(x / length, y / length);
	}
	template<typename T2>
	inline Vector2<T2> ToVector2()const
	{
		return Vector2<T2>(static_cast<T2>(x), static_cast<T2>(y));
	}
	inline Vector2<T>Rotate(const float rot)const
	{
		return Vector2<T>(x * cos(rot) - y * sin(rot), x * sin(rot) + y * cos(rot));
	}

	static const Vector2<T> ZERO;
	static const Vector2<T> NIL;

	inline bool IsNil()const
	{
		return isnan(x) || isnan(y);
	}
};

template<typename T>
inline Vector2<T> operator+(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return Vector2<T>(lval.x + rval.x, lval.y + rval.y);
}
template<typename T>
inline Vector2<T> operator-(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return Vector2<T>(lval.x - rval.x, lval.y - rval.y);
}
template<typename T>
inline Vector2<T> operator*(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return Vector2<T>(lval.x * rval.x, lval.y * rval.y);
}
template<typename T>
inline Vector2<T> operator/(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return Vector2<T>(lval.x / rval.x, lval.y / rval.y);
}
template<typename T>
inline Vector2<T> operator%(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return Vector2<T>(lval.x % rval.x, lval.y % rval.y);
}
template<typename T>
inline Vector2<T> operator*(const Vector2<T>& lval, const float rval)
{
	return Vector2<T>(static_cast<T>(lval.x * rval), static_cast<T>(lval.y * rval));
}
template<typename T>
inline bool operator == (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x == rval.x && lval.y == rval.y;
}
template<typename T>
inline bool operator != (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x != rval.x || lval.y != rval.y;
}
template<typename T>
inline bool operator < (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x < rval.x && lval.y < rval.y;
}
template<typename T>
inline bool operator > (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x > rval.x && lval.y > rval.y;
}
template<typename T>
inline bool operator <= (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x <= rval.x && lval.y <= rval.y;
}
template<typename T>
inline bool operator >= (const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x >= rval.x && lval.y >= rval.y;
}
template<typename T>
Vector2<T> Lerp(const Vector2<T>& start, const Vector2<T>& end, const float parsent)
{
	return start + (end - start) * parsent;
}
template<typename T>
float Dot(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x * rval.x + lval.y * rval.y;
}
template<typename T>
float Cross(const Vector2<T>& lval, const Vector2<T>& rval)
{
	return lval.x * rval.y - lval.y * rval.x;
}

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;

struct Vector3
{
	float x, y, z;
	Vector3():x(0),y(0),z(0) {}
	Vector3(const float x, const float y, const float z) :x(x), y(y), z(z) {}
	Vector3(const DirectX::XMFLOAT3& xyz):x(xyz.x), y(xyz.y), z(xyz.z) {}
	Vector3(const DirectX::XMFLOAT4& xyzw) :x(xyzw.x), y(xyzw.y), z(xyzw.z) {}
	Vector3(const Vector2f& vec2): x(vec2.x), y(vec2.y), z(0) {}

	Vector3 operator-(void)const;

	void operator+=(const Vector3& val);
	void operator-=(const Vector3& val);
	void operator*=(const Vector3& val);
	void operator/=(const Vector3& val);

	void operator+=(float scale);
	void operator-=(float scale);
	void operator*=(float scale);
	void operator/=(float scale);

	float Length()const;

	Vector3 Normalize();
	Vector3 Normalized()const;

	DirectX::XMFLOAT3 ToXMFloat3()const;
};

float Dot(const Vector3 &lval, const Vector3& rval);
Vector3 Cross(const Vector3 &lval, const Vector3& rval);

// 反射ベクトルを作る
Vector3 ReflectionVector(const Vector3& baseVector, const Vector3& norm);

// 屈折ベクトルを作る
Vector3 RefractionVector(const Vector3& baseVector, const Vector3& norm, float parsent);

// 線形補完
Vector3 Lerp(const Vector3& start, const Vector3& end, const float parsent);

Vector3 operator+(const Vector3 &lval, const Vector3& rval);
Vector3 operator-(const Vector3 &lval, const Vector3& rval);
Vector3 operator*(const Vector3 &lval, const Vector3& rval);
Vector3 operator/(const Vector3 &lval, const Vector3& rval);

Vector3 operator+(const Vector3 &lval, const float& rval);
Vector3 operator-(const Vector3 &lval, const float& rval);
Vector3 operator*(const Vector3 &lval, const float& rval);
Vector3 operator/(const Vector3 &lval, const float& rval);

float Lerp(const float lval, const float rval, const float parsent);
DirectX::XMFLOAT2 Lerp(const DirectX::XMFLOAT2 lval, const DirectX::XMFLOAT2 rval, const float parsent);

// 三角形の面積の算出
float TriangleArea(const Vector3& p0, const Vector3& p1, const Vector3& p2);

struct Vector4
{
	float x, y, z, w;
	Vector4() :x(0), y(0), z(0), w(0) {};
	Vector4(const float x, const float y, const float z, const float w) :x(x), y(y), z(z), w(w){}
};

struct Size
{
	int w, h;
	Size() :w(0), h(0) {}
	Size(const int w, const int h) :w(w), h(h) {}
	Size operator-(const Size& size);
	void operator*=(const float scale);

	template<typename T>
	Vector2<T> ToVector2()const
	{
		return Vector2<T>(static_cast<T>(w), static_cast<T>(h));
	}
};

Size operator+(const Size& lval, const Size& rval);
Size operator*(const Size& lval, const Size& rval);
Size operator/(const Size& lval, const Size& rval);
Size operator/(const Size& lval, const Vector2i& rval);

Size operator+(const Size& lval, const int& rval);
Size operator*(const Size& lval, const float& rval);

bool operator==(const Size& lval, const Size& rval);

struct Rect 
{
	Vector2i center;
	Size size;
	Rect();
	Rect(const int x, const int y, const int w, const int h);
	Rect(const Vector2i& pos, const Size& sz);

	const int Left()const;
	const int Top() const;
	const int Right()const;
	const int Botton()const;
	const int Width()const { return size.w; }
	const int Height()const{ return size.h; }

	void Draw(const unsigned int color = 0xffffff, const bool fill = true)const;
	void Draw(const Vector2i& offset, const int color = 0xffffff, const bool fill = true)const;
	void DrawGraph(const int graphH, const Vector2i& offset = Vector2i(0,0))const;
	void DrawRectGraph(const Vector2i& leftup, const Size& rectSize, const int graphH, const Vector2i& offset = Vector2i(0, 0))const;
	void DrawExtendGraph(const Vector2i& leftup, const Vector2i& rightdown, const int graphH, const Vector2i& offset = Vector2i(0, 0))const;

	bool IsHit(const Rect& rect)const;
	static const bool IsHit(const Rect& aRect, const Rect& bRect);
	static const Size OverlapSize(const Rect& aRect, const Rect& bRect);
};

struct Circle
{
	Vector2f centor;
	float radius;
	Circle();
	Circle(const float x, const float y, const float r);
	Circle(const Vector2f& centor, const float r);

	void Draw(int color = 0xffffff, const Vector2i offset = Vector2i());
	bool IsHit(const Circle& c)const;
};

struct Segment
{
	Vector2f start;
	Vector2f vec;

	Vector2f End()const;
	Segment(Vector2f start, Vector2f end) :start(start), vec(end - start) {}

	static const Segment NIL;
	bool IsNil()const;

	float GetY(const float x)const;
	float GetX(const float y)const;
};

struct Capsule
{
	Segment segment;
	float radius;
	Capsule(Vector2f start, Vector2f end, float radius)
		:segment(start, end), radius(radius){};
	Capsule(Segment seg, float radius)
		:segment(seg), radius(radius) {};

	bool IsHit(const Circle& circle)const;
	Vector2f End()const;
};

// 値を最大値と最小値を考慮した値に加工する
float Clamp(const float in, const float min = 0.0f, const float max = 1.0f);

Vector3 Clamp(const Vector3& in, const float min = 0.0f, const float max = 1.0f);

struct Range
{
	int min;
	int max;

	Range();
	Range(const int min, const int max);

	bool Hit(const int value)const;
	Range GetCriticalRange(const Range& target)const;
};
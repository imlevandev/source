#include <cmath>
struct vec2
{
	float x, y;
};

float ToMeters(double x)
{
	return static_cast<float>(x / 39.62);
}

struct Rotator
{
	double Pitch, Yaw, Roll;
};

struct Vector3
{
	double x, y, z;
	Vector3 operator-(Vector3 ape)
	{
		return { x - ape.x, y - ape.y, z - ape.z };
	}

	Vector3 operator+(Vector3 ape)
	{
		return { x + ape.x, y + ape.y, z + ape.z };
	}

	Vector3 operator*(double ape)
	{
		return { x * ape, y * ape, z * ape };
	}

	Vector3 operator/(double ape)
	{
		return { x / ape, y / ape, z / ape };
	}

	Vector3 operator/=(double ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	Vector3 operator+=(Vector3 ape)
	{
		return { x += ape.x, y += ape.y, z += ape.z };
	}

	Vector3 operator-=(Vector3 ape)
	{
		return { x -= ape.x, y -= ape.y, z -= ape.z };
	}

	double Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	double Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	double DistTo(Vector3 ape)
	{
		return (*this - ape).Length();
	}

	double Dist2D(Vector3 ape)
	{
		return (*this - ape).Length2D();
	}

	double Dot(Vector3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
};
struct Vector4
{
	double w, x, y, z;
};

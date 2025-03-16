#pragma once
#include <cmath>
#include <cstring>

#define M_PI 3.14159265358979323846

inline float rad2deg(float rad)
{
	return rad * 180.0f / 3.1415927f;
}

struct Vector2
{
	float x = 0.f, y = 0.f;

	Vector2 operator-(Vector2 ape)
	{
		return {x - ape.x, y - ape.y};
	}

	Vector2 operator+(Vector2 ape)
	{
		return {x + ape.x, y + ape.y};
	}

	Vector2 operator/(Vector2 ape)
	{
		return {x / ape.x, y / ape.y};
	}

	Vector2 operator*(float ape)
	{
		return {x * ape, y * ape};
	}

	Vector2 operator/(float ape)
	{
		return {x / ape, y / ape};
	}

	Vector2 operator/=(float ape)
	{
		x /= ape;
		y /= ape;

		return *this;
	}

	Vector2 operator+=(Vector2 ape)
	{
		return {x += ape.x, y += ape.y};
	}

	Vector2 operator-=(Vector2 ape)
	{
		return {x -= ape.x, y -= ape.y};
	}
};

struct Vector
{
	float x = 0.f, y = 0.f, z = 0.f;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(x, y, z);
	}

	Vector(void)
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector(float X, float Y, float Z)
	{
		x = X;
		y = Y;
		z = Z;
	}

	bool empty()
	{
		if (x == 0 && y == 0 && z == 0) return true;
		return false;
	}

	Vector cross(const Vector& other) const {
		return Vector(y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	Vector operator-(Vector ape)
	{
		return {x - ape.x, y - ape.y, z - ape.z};
	}

	Vector operator+(Vector ape)
	{
		return {x + ape.x, y + ape.y, z + ape.z};
	}

	Vector operator*(float scalar)
	{
		return {x * scalar, y * scalar, z * scalar};
	}

	// This friend function allows for commutative multiplication
	friend Vector operator*(float scalar, Vector vec);

	Vector operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;

		return *this;
	}

	Vector operator/(float ape)
	{
		return {x / ape, y / ape, z / ape};
	}

	Vector operator/=(float ape)
	{
		x /= ape;
		y /= ape;
		z /= ape;

		return *this;
	}

	bool operator==(Vector ape)
	{
		if (x == ape.x && y == ape.y && z == ape.z) return true;

		return false;
	}

	Vector operator+=(Vector ape)
	{
		return {x += ape.x, y += ape.y, z += ape.z};
	}

	Vector operator*(Vector ape)
	{
		return {x * ape.x, y * ape.y, z * ape.z};
	}

	Vector operator-=(Vector ape)
	{
		return {x -= ape.x, y -= ape.y, z -= ape.z};
	}

	Vector CalcAngle(const Vector& dst)
	{
		Vector angle = Vector();
		Vector delta = Vector((x - dst.x), (y - dst.y), (z - dst.z));

		float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
		angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
		angle.z = 0;
		if (delta.x >= 0.0) angle.y += 180.0f;

		return angle;
	}

	void Normalize()
	{
		while (x > 89.0f)
			x -= 180.f;

		while (x < -89.0f)
			x += 180.f;

		while (y > 180.f)
			y -= 360.f;

		while (y < -180.f)
			y += 360.f;
	}

	void Clamp();

	float Length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}

	float Length2D()
	{
		return sqrt((x * x) + (y * y));
	}

	float DistTo(Vector ape)
	{
		return (*this - ape).Length();
	}

	float Dist2D(Vector ape)
	{
		return (*this - ape).Length2D();
	}

	float Dot(const Vector& vector)
	{
		return (x * vector.x + y * vector.y + z * vector.z);
	}

	float Distance(const Vector& vector)
	{
		return sqrt(pow(vector.x - x, 2) + pow(vector.y - y, 2) + pow(vector.z - z, 2));
	}

	inline static float distance(Vector lhs, Vector rhs)
	{
		Vector delta = Vector {rhs.x - lhs.x, rhs.y - lhs.y, rhs.z - lhs.z};
		return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}

	inline float magnitude() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vector normalized() const
	{
		float mag = magnitude();
		if (mag == 0) return {0, 0, 0}; // Handle the zero vector edge case
		return {x / mag, y / mag, z / mag};
	}

	inline Vector ToAngle() const
	{
		return Vector {std::atan2(-z, std::hypot(x, y)) * (180.0f / (float)M_PI), std::atan2(y, x) * (180.0f / (float)M_PI), 0.0f};
	}

	inline Vector to_angles() const
	{
		float yaw, pitch;
		if (y == 0.0f && x == 0.0f)
		{
			yaw = 0.0f;
			pitch = z > 0.0f ? 270.0f : 90.0f;
		}
		else
		{
			yaw = rad2deg(atan2(y, x));
			const float tmp = sqrt(x * x + y * y);
			pitch = rad2deg(atan2(-z, tmp));
		}
		return Vector {pitch, yaw, 0.0f};
	}

	inline Vector norm_angles() const
	{
		const float pitch = x < -90.0f ? -90.0f : (x > 90.0f ? 90.0f : x);
		float yaw = y;
		while (yaw <= -180.0f) yaw += 360.0f;
		while (yaw > 180.0f) yaw -= 360.0f;
		return Vector {pitch, yaw, 0.0f};
	}
};

class VMatrix
{
public:
	VMatrix()
		: m {
			{0.f, 0.f, 0.f, 0.f},
			{0.f, 0.f, 0.f, 0.f},
			{0.f, 0.f, 0.f, 0.f},
			{0.f, 0.f, 0.f, 0.f}
		}
	{
	}

	VMatrix(const VMatrix&) = default;

	VMatrix transpose()
	{
		VMatrix m;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				m.m[i][j] = this->m[j][i];

		return m;
	}

	void matrix_identity()
	{
		memset(this, 0.f, sizeof(VMatrix));
		m[0][0] = 1.0f;
		m[1][1] = 1.0f;
		m[2][2] = 1.0f;
		m[3][3] = 1.0f;
	}

	Vector operator*(const Vector& vec)
	{
		VMatrix m;

		m[3][0] = vec.x;
		m[3][1] = vec.y;
		m[3][2] = vec.z;

		m[0][0] = 1.f;
		m[1][1] = 1.f;
		m[2][2] = 1.f;

		m[0][3] = 0.0f;
		m[1][3] = 0.0f;
		m[2][3] = 0.0f;
		m[3][3] = 1.0f;

		auto result = m * (*this);

		return Vector {result[3][0], result[3][1], result[3][2]};
	}

	VMatrix operator*(const VMatrix& _m2)
	{
		auto _m = *this;

		VMatrix out;
		out[0][0] = _m[0][0] * _m2[0][0] + _m[0][1] * _m2[1][0] + _m[0][2] * _m2[2][0] + _m[0][3] * _m2[3][0];
		out[0][1] = _m[0][0] * _m2[0][1] + _m[0][1] * _m2[1][1] + _m[0][2] * _m2[2][1] + _m[0][3] * _m2[3][1];
		out[0][2] = _m[0][0] * _m2[0][2] + _m[0][1] * _m2[1][2] + _m[0][2] * _m2[2][2] + _m[0][3] * _m2[3][2];
		out[0][3] = _m[0][0] * _m2[0][3] + _m[0][1] * _m2[1][3] + _m[0][2] * _m2[2][3] + _m[0][3] * _m2[3][3];
		out[1][0] = _m[1][0] * _m2[0][0] + _m[1][1] * _m2[1][0] + _m[1][2] * _m2[2][0] + _m[1][3] * _m2[3][0];
		out[1][1] = _m[1][0] * _m2[0][1] + _m[1][1] * _m2[1][1] + _m[1][2] * _m2[2][1] + _m[1][3] * _m2[3][1];
		out[1][2] = _m[1][0] * _m2[0][2] + _m[1][1] * _m2[1][2] + _m[1][2] * _m2[2][2] + _m[1][3] * _m2[3][2];
		out[1][3] = _m[1][0] * _m2[0][3] + _m[1][1] * _m2[1][3] + _m[1][2] * _m2[2][3] + _m[1][3] * _m2[3][3];
		out[2][0] = _m[2][0] * _m2[0][0] + _m[2][1] * _m2[1][0] + _m[2][2] * _m2[2][0] + _m[2][3] * _m2[3][0];
		out[2][1] = _m[2][0] * _m2[0][1] + _m[2][1] * _m2[1][1] + _m[2][2] * _m2[2][1] + _m[2][3] * _m2[3][1];
		out[2][2] = _m[2][0] * _m2[0][2] + _m[2][1] * _m2[1][2] + _m[2][2] * _m2[2][2] + _m[2][3] * _m2[3][2];
		out[2][3] = _m[2][0] * _m2[0][3] + _m[2][1] * _m2[1][3] + _m[2][2] * _m2[2][3] + _m[2][3] * _m2[3][3];
		out[3][0] = _m[3][0] * _m2[0][0] + _m[3][1] * _m2[1][0] + _m[3][2] * _m2[2][0] + _m[3][3] * _m2[3][0];
		out[3][1] = _m[3][0] * _m2[0][1] + _m[3][1] * _m2[1][1] + _m[3][2] * _m2[2][1] + _m[3][3] * _m2[3][1];
		out[3][2] = _m[3][0] * _m2[0][2] + _m[3][1] * _m2[1][2] + _m[3][2] * _m2[2][2] + _m[3][3] * _m2[3][2];
		out[3][3] = _m[3][0] * _m2[0][3] + _m[3][1] * _m2[1][3] + _m[3][2] * _m2[2][3] + _m[3][3] * _m2[3][3];

		return out;
	}

	float* operator[](size_t i) { return m[i]; }
	const float* operator[](size_t i) const { return m[i]; }

	union
	{
		struct
		{
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};

		float m[4][4];
	};
};

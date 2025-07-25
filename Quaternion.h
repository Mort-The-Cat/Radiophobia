#ifndef VISION_QUATERNION
#define VISION_QUATERNION

#include "OpenGL_Declarations.h"

glm::mat4 Model_Rotation_Calculate(glm::vec3 Position, glm::vec3 Orientation, glm::vec3 Up_Vector)
{
	glm::vec3 Front = Orientation;
	glm::vec3 Right = glm::normalize(Fast::Cross(Up_Vector, Front));
	glm::vec3 Up = Fast::Cross(Front, Right);

	/*return glm::mat4(Right.x, Up.x, Front.x, Position.x,
		Right.y, Up.y, Front.y, Position.y,
		Right.z, Up.z, Front.z, Position.z,
		0, 0, 0, 1);*/

	return glm::mat4(Right.x, Right.y, Right.z, 0.f,
		Up.x, Up.y, Up.z, 0.f,
		Front.x, Front.y, Front.z, 0.f,
		Position.x, Position.y, Position.z, 1.f);
}

glm::vec3 Calculate_Orientation_Vector(glm::vec3 Euler_Angles)
{
	//glm::vec3 Return = Forward_Vector;

	float Angle_X = -DTR * Euler_Angles.x;
	float Angle_Y = -DTR * Euler_Angles.y;

	float Sin_X = sinf(Angle_X);
	float Cos_X = cosf(Angle_X);

	float Sin_Y = sinf(Angle_Y);
	float Cos_Y = cosf(Angle_Y);

	glm::vec3 Return = glm::vec3(Sin_X, Sin_Y, Cos_X);
	Return.x *= Cos_Y;
	Return.z *= Cos_Y;

	return Return;
}

namespace Quaternion
{
	class Quaternion
	{
	public:
		float W, X, Y, Z;
		Quaternion() {}
		Quaternion(float Wp, float Xp, float Yp, float Zp)
		{
			W = Wp;
			X = Xp;
			Y = Yp;
			Z = Zp;
		}

		void Normalise()
		{
			float Inverse_Magnitude = 1.0f / sqrtf(W * W + X * X + Y * Y + Z * Z);
			W *= Inverse_Magnitude;
			X *= Inverse_Magnitude;
			Y *= Inverse_Magnitude;
			Z *= Inverse_Magnitude;
		}

		glm::mat4 Get_Rotation_Matrix()
		{
			// Need to write this function

			// return glm::mat4(1.0f);

			glm::quat Quat(W, X, Y, Z);
			
			return glm::mat4_cast(Quat);
		}

		Quaternion operator* (float Value)
		{
			return Quaternion(W * Value, X * Value, Y * Value, Z * Value);
		}
		Quaternion operator- (Quaternion Other)
		{
			return Quaternion(W - Other.W, X - Other.X, Y - Other.Y, Z - Other.Z);
		}
		Quaternion operator+ (Quaternion Other)
		{
			return Quaternion(W + Other.W, X + Other.X, Y + Other.Y, Z + Other.Z);
		}

		void operator*= (float Value)
		{
			W *= Value;
			X *= Value;
			Y *= Value;
			Z *= Value;
		}
		void operator+= (Quaternion Other)
		{
			W += Other.W;
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
		}
	};

	Quaternion Interpolate(Quaternion A, Quaternion B, float Value)
	{
		Quaternion C;
		C = A * (1.f - Value);
		C += B * Value;
		return C;
	}

	Quaternion Sphere_Interpolate(Quaternion A, Quaternion B, float Value)
	{
		float Dot = Fast::Dot(glm::vec4(A.X, A.Y, A.Z, A.W), glm::vec4(B.X, B.Y, B.Z, B.W));

		//const float Dot_Threshold = 0.9995f;
		//if (Dot > Dot_Threshold)
		//	return Interpolate(A, B, Value);

		glm::clamp(Dot, -1.0f, 1.0f);
		float Theta_0 = acosf(Dot);
		float Theta = Theta_0 * Value;

		Quaternion C = B - A * Value;
		return A * cosf(Theta) + C * sinf(Theta);
	}

	Quaternion Rotate_Quaternion(Quaternion A, Quaternion B)
	{
		glm::quat A_Q = glm::quat(A.W, A.X, A.Y, A.Z);
		glm::quat B_Q = glm::quat(B.W, B.X, B.Y, B.Z);

		glm::quat C = A_Q * B_Q;

		return Quaternion(C.w, C.x, C.y, C.z);
	}

	glm::vec3 Rotate(Quaternion Rotation, glm::vec3 Vector) // This is a black box xddd
	{
		glm::vec3 Quaternion_Vector(Rotation.X, Rotation.Y, Rotation.Z);

		const float Scalar = Rotation.W;

		return (2.0f * Fast::Dot(Quaternion_Vector, Vector)) * Quaternion_Vector
			+ (Scalar * Scalar - Fast::Dot(Quaternion_Vector, Quaternion_Vector)) * Vector
			+ (2.0f * Scalar) * Fast::Cross(Quaternion_Vector, Vector);
	}

	glm::vec3 Get_Tangent(glm::vec3 Angle_Axis, glm::vec3 Point)
	{
		//std::swap(Angle_Axis.x, Angle_Axis.y);

		//Angle_Axis.y = -Angle_Axis.y;

		Angle_Axis = glm::normalize(Fast::Cross(Angle_Axis, Point));
		if (std::isnan(Angle_Axis.x) || std::isnan(Angle_Axis.y) || std::isnan(Angle_Axis.z))
			Angle_Axis = glm::vec3(0, 0, 1);
		return Angle_Axis;
	}

	float Get_Angle(Quaternion Rotation, glm::vec3 Point) // This returns radians
	{
		return acosf(Fast::Dot(Rotate(Rotation, Point), Point));
	}

	glm::vec4 Get_Axis_Angle(Quaternion Rotation, glm::vec3 Point) // angle is in radians
	{
		glm::vec3 A = Rotate(Rotation, Point);

		float Dot_Product = acosf(Fast::Dot(A, Point)); // This can be simplified haha

		//float Sign = A.x; // This will be used for the sign bit of the sine lol

		A = glm::normalize(Fast::Cross(A, Point));

		if (std::isnan(A.x))
			A = glm::vec3(1, 0, 0);

		return glm::vec4(A.x, A.y, A.z, Dot_Product); // the angle is in radians

		//return glm::vec4(Fast::cross(A, glm::vec3(0, 0, 1)), acosf(Dot_Product));
	}

	Quaternion Rotation_Vector_To_Quaternion(glm::vec3 Rotation)
	{
		Quaternion Return;
		float Length = glm::length(Rotation);

		float Angle = Length * -0.5f;

		Return.W = cos(Angle);

		Return.X = Rotation.x;
		Return.Y = Rotation.y;
		Return.Z = Rotation.z;

		Angle = sin(Angle) / Length;

		Return.X *= Angle;
		Return.Y *= Angle;
		Return.Z *= Angle;

		if (std::isnan(Return.X))
			Return = Quaternion(1, 0, 0, 0);

		return Return;
	}

	Quaternion Angle_Axis_To_Quaternion(glm::vec3 Axis, float Angle)
	{
		Quaternion Return;
		Angle *= -0.5 * DTR;
		Return.W = cos(Angle);

		Angle = sin(Angle);

		Return.X = Axis.x * Angle;
		Return.Y = Axis.y * Angle;
		Return.Z = Axis.z * Angle;

		if (std::isnan(Return.X))
			Return = Quaternion(1, 0, 0, 0);

		return Return;
	}
}

#endif

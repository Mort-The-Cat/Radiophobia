#pragma once

#ifndef FAST_MATHS_H
#define FAST_MATHS_H

#include <cstdint>
#include <math.h>
#include <random>
#include <glm/glm.hpp>

const float One = 1.0f;

constexpr const float DTR = 3.14159f / 180.0f;

constexpr const float Inv_Rand = 1.0f / (float)RAND_MAX;

float RNG()
{
	return static_cast<float>(rand()) * Inv_Rand;
}

template<typename Type>
std::vector<Type> operator+(std::vector<Type> A, std::vector<Type> B)
{
	size_t Iterator = A.size();

	A.resize(A.size() + B.size());

	for (size_t W = 0; W < B.size(); W++)
	{
		A[Iterator] = B[W];
		Iterator++;
	}

	return A;
}

template<typename Type>
std::vector<Type> Splice_Vector(std::vector<Type>& A, size_t Start, size_t End)
{
	size_t Delta = End - Start;

	std::vector<Type> New_Vector(Delta);
	for (size_t I = 0, J = Start; J < End; I++, J++)
	{
		New_Vector[I] = A[J];
	}

	return New_Vector;
}

namespace Fast
{

#define Delta_Constant 0x1FBD3F7Du
#define Epsilon_Constant 0x7EF4FDF4u
#define Zeta_Constant 1064992510u

#define DTR_Constant 48934896u

	float Dot(glm::vec4 A, glm::vec4 B)
	{
		alignas(16) float Result_Vector[4]; // final float here is just padding but it's required with these functions

		_mm_store_ps(Result_Vector,
			_mm_mul_ps(
				_mm_loadu_ps(reinterpret_cast<float*>(&A)),
				_mm_loadu_ps(reinterpret_cast<float*>(&B))
			)
		);

		return (Result_Vector[0] + Result_Vector[1]) + (Result_Vector[2] + Result_Vector[3]);
	}

	float Dot(const glm::vec3 A, const glm::vec3 B)
	{
		/*
		
			| a |	| d |	
			| b |	| e | =	ad + be + cf + 0g
			| c |	| f |
		
		*/

		alignas(16) float Result_Vector[4]; // final float here is just padding but it's required with these functions

		_mm_store_ps(Result_Vector,
			_mm_mul_ps(
				_mm_loadu_ps(reinterpret_cast<const float*>(&A)),
				_mm_loadu_ps(reinterpret_cast<const float*>(&B))
			)
		);

		return Result_Vector[0] + Result_Vector[1] + Result_Vector[2];
	}

	glm::vec3 Cross(glm::vec3 A, glm::vec3 B) 
	{				// These are usually inlined by the compiler, so the compiler is able to remove any locally redundant code here if possible!!
		__m128 A_Xmm = _mm_loadu_ps(reinterpret_cast<float*>(&A));
		__m128 B_Xmm = _mm_loadu_ps(reinterpret_cast<float*>(&B));

		alignas(16) glm::vec4 Result_Vector;

		_mm_store_ps(reinterpret_cast<float*>(&Result_Vector),
			_mm_sub_ps(
				_mm_mul_ps(
					_mm_permute_ps(A_Xmm, 0b001001u), // y, z, x
					_mm_permute_ps(B_Xmm, 0b010010u) // z, x, y
				),
				_mm_mul_ps(
					_mm_permute_ps(A_Xmm, 0b010010u), // z, x, y
					_mm_permute_ps(B_Xmm, 0b001001u) // y, z, x
				)
			)
		);

		return Result_Vector;
	}

	/*glm::vec3 Cross_Old(glm::vec3 A, glm::vec3 B)
	{
		
		//const float Column_0[] = { A.y, A.z, A.x };
		alignas (16) const float Column_1[] = { B.z, B.x, B.y };
		alignas (16) const float Column_2[] = { A.z, A.x, A.y };
		//const float Column_3[] = { B.y, B.z, B.x };

		__m128 Column_1_Xmm = _mm_load_ps(Column_1);
		__m128 Column_2_Xmm = _mm_load_ps(Column_2);

		alignas(16) glm::vec4 Result_Vector;

		_mm_store_ps(reinterpret_cast<float*>(&Result_Vector),
			_mm_sub_ps(
				_mm_mul_ps(
					_mm_permute_ps(Column_2_Xmm, 0b010010u),
					Column_1_Xmm
				),
				_mm_mul_ps(
					Column_2_Xmm,
					_mm_permute_ps(Column_1_Xmm, 0b010010u)
				)
			)
		);

		return *reinterpret_cast<glm::vec3*>(&Result_Vector);
	}*/

	glm::vec4 Matrix_Vector_Multiplication(const glm::mat4x4& Matrix, const glm::vec4& Vector)
	{
		/*
		
			| a, b, c, d,|	| q |		| x |
			| e, f, g, h,|	| r |		| y |
			| i, j, k, l,|	| s |	=	| z |
			| m, n, o, p |	| t |		| w |

		*/

		const float First_Row[]		= { Matrix[0][0], Matrix[1][0], Matrix[2][0], Matrix[3][0] };
		const float Second_Row[]	= { Matrix[0][1], Matrix[1][1], Matrix[2][1], Matrix[3][1] };
		const float Third_Row[]		= { Matrix[0][2], Matrix[1][2], Matrix[2][2], Matrix[3][2] };
		const float Fourth_Row[]	= { Matrix[0][3], Matrix[1][3], Matrix[2][3], Matrix[3][3] };

		__m128 Vector_Xmm = _mm_loadu_ps(reinterpret_cast<const float*>(&Vector));

		__m128 Result_Vector_Xmm =
			_mm_hadd_ps(
				_mm_hadd_ps(
					_mm_mul_ps(Vector_Xmm, _mm_loadu_ps(First_Row)),
					_mm_mul_ps(Vector_Xmm, _mm_loadu_ps(Second_Row))
				),
				_mm_hadd_ps(
					_mm_mul_ps(Vector_Xmm, _mm_loadu_ps(Third_Row)),
					_mm_mul_ps(Vector_Xmm, _mm_loadu_ps(Fourth_Row))
				)
			);

		glm::vec4 Result_Vector;

		_mm_storeu_ps(reinterpret_cast<float*>(&Result_Vector), Result_Vector_Xmm);

		return Result_Vector;
	}

	float Approach(float Value, float Target, float Delta)
	{
		float Difference = Target - Value;
		float Deltas[] = { Difference, copysignf(Delta, Difference) };

		return Value + Deltas[fabsf(Difference) > Delta];
	}

	float Add_Epsilon(float Value, int Epsilon)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long += Epsilon;

		return *(float*)&Long;
	}

	float Inverse_Sqrt(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long >>= 1;

		Long += Delta_Constant;

		Long = Epsilon_Constant - Long;

		return *(float*)&Long;
	}

	float Sqrt(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long >>= 1;

		Long += Delta_Constant;

		return *(float*)&Long;
	}

	template<typename Type>
	Type Square(Type A)
	{
		return A * A;
	}

	float Division(float Value, float Denominator)
	{
		uint32_t Long = *(uint32_t*)&Value;

		uint32_t I = *(uint32_t*)&Denominator;

		Long -= I;

		Long += Zeta_Constant;

		return *(float*)&Long;
	}

	float Inverse(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;
		Long = Epsilon_Constant - Long;
		return *(float*)&Long;
	}

	float Old_Pow(float Value, float Exponent)
	{
		uint32_t Long = (*(uint32_t*)&Value) - Zeta_Constant;

		Long *= Exponent;

		Long += Zeta_Constant;

		return *(float*)&Long;
	}

	float Log_Two(float Value)
	{
		int64_t Long = *(uint32_t*)&Value;
		float Exponent = (Long >> 23) - 128;
		uint32_t Mantissa = (Long & 8388607) | 1065353216;
		Exponent += *(float*)&Mantissa;
		return Exponent;
	}

	float Pow_2(float Exponent)
	{
		uint32_t Long = 127;
		Long += Exponent;
		Long <<= 23;

		Exponent -= floorf(Exponent) - 1;
		uint32_t Mantissa = *(uint32_t*)&Exponent;

		Long |= Mantissa & 8388607u;

		return *(float*)&Long;
	}

	float Pow(float Value, float Exponent)
	{
		Value = Log_Two(Value);

		return Pow_2(Exponent * Value);
	}

	float Sigmoid(float Value)
	{
		uint32_t Long = 11762006;

		Long *= -Value; // float * int (not ideal)

		Long += Zeta_Constant; // integre

		float Denomenator = 1.0f + *(float*)&Long; //float

		Long = *(uint32_t*)&Denomenator;

		Long = Epsilon_Constant - Long; // integre

		return *(float*)&Long;
	}

	float Sigmoid_Derivative(float Value)
	{
		float Sigmoid_X = Sigmoid(Value);

		return Sigmoid_X * (1 - Sigmoid_X);
	}

	bool Is_Decimal(float Value) // This returns whether or not the input is a whole number
	{
		uint32_t Long = *(uint32_t*)&Value;

		int8_t Exponent = (Long >> 23) + 1; // This should get the transformed exponent value with simple ALU instructions

		//Exponent -= 127;

		uint32_t Mantissa_Mask = 8388607u; //((1 << 23) - 1);

		return !(Long & (Mantissa_Mask >> Exponent)) && Exponent < 0;
	}

	float Floor(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		int8_t Exponent = (Long >> 23) - 127;

		uint32_t Float_Mask = 8388607u >> Exponent; // Creates a mask for the float, unsetting the bits which contribute to the part of the number

		Long &= ~(Float_Mask); // Applies the mask

		uint8_t Negative_Exp = (Exponent < 0) << 1;

		Long >>= Negative_Exp; // If the exponent is negative (and thereby the value is between -1 and 1) then the float is basically set to zero

		return *(float*)&Long;
	}

	float To_Radians(float Value)
	{
		uint32_t Long = *(uint32_t*)&Value;

		Long -= DTR_Constant; // Equivalent to / (3.14159 / 180)

		return *(float*)&Long;
	}

#undef Delta_Constant
#undef Zeta_Constant
#undef Epsilon_Constant
#undef DTR_Constant

}

#endif

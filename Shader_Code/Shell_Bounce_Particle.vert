#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

uniform mat4 Projection_Matrix;

uniform float Particle_Data[450]; // We know that the smoke particle has 8 floats in it total

uniform vec2 Inv_Window_Dimensions;

int Particle_Index = gl_InstanceID * 18;

vec3 Initial_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);

float Particle_Age = Particle_Data[Particle_Index + 3];

vec3 Velocity = vec3(Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5], Particle_Data[Particle_Index + 6]);

float Initial_X_Direction = Particle_Data[Particle_Index + 7];

float E = Particle_Data[Particle_Index + 8];

float Delta = Particle_Data[Particle_Index + 9];
float Q = Particle_Data[Particle_Index + 10];

float Height = Particle_Data[Particle_Index + 11];
float Ratio = Particle_Data[Particle_Index + 12];

float Floor_Delta = Particle_Data[Particle_Index + 13];
float Inverse_Delta = Particle_Data[Particle_Index + 14];

float L = Particle_Data[Particle_Index + 15];
float Inverse_L = Particle_Data[Particle_Index + 16];

float Random_Value = Particle_Data[Particle_Index + 17];

float Smoothed_Particle_Age = log(1 + Particle_Age);

float S(float X)
{
	X = 2 * mod(X + 10, 1) - 1;
	X *= X;
	return 1 - X;
}

float Bounce()
{
	return Height * pow(Ratio, -L) * S(pow(E, 1 - L) * ((Particle_Age - Q - Delta * Inverse_L) * Inverse_Delta));
}

vec3 Rotated_Point(vec3 Point)
{
	float X_Direction = Initial_X_Direction + (Smoothed_Particle_Age * 3) * sin(Random_Value * 10);
	float Y_Direction = exp(-L) * (Smoothed_Particle_Age * 3 * cos(Random_Value * 15));

	float X_Cos = cos(X_Direction);
	float X_Sin = sin(X_Direction);

	float Y_Cos = cos(Y_Direction);
	float Y_Sin = sin(Y_Direction);

	//

	vec3 First_Pass;

	First_Pass.x = X_Cos * Point.x + X_Sin * Point.z;
	First_Pass.z = X_Cos * Point.z - X_Sin * Point.x;

	First_Pass.y = Point.y;

	vec3 Second_Pass;

	Second_Pass.y = Y_Cos * First_Pass.y + Y_Sin * First_Pass.z;
	Second_Pass.z = Y_Cos * First_Pass.z - Y_Sin * First_Pass.y;

	Second_Pass.x = First_Pass.x;

	return Second_Pass;
}

out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;
	
	float Vertex_Transparency;
} data_out;

void main()
{
	vec3 New_In_Position = 1.5f * Rotated_Point(In_Position);

	New_In_Position += vec3(Initial_Position.x, 0, Initial_Position.z);

	New_In_Position.xz += Smoothed_Particle_Age * Velocity.xz;

	New_In_Position.y += Bounce() + Floor_Delta;

	gl_Position = vec4(New_In_Position, 1.0f);

	data_out.Normal = In_Normal;
	data_out.Projection_Matrix = Projection_Matrix;
	data_out.UV = In_UV;

	data_out.Vertex_Transparency = 1.0f;
	data_out.Lighting_Transparency = 1.0f;
}
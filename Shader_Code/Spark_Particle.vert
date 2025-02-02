#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

uniform mat4 Projection_Matrix;

out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;

	float Vertex_Transparency;
} data_out;

out vec2 UV;
out float Vertex_Transparency;

uniform float Particle_Data[600]; // We know that the spark particle has 8 floats in it total

uniform vec3 Camera_Position;
uniform vec3 Camera_Up_Direction;
uniform vec3 Camera_Direction;

vec3 Camera_Right_Direction = cross(Camera_Direction, Camera_Up_Direction);

int Particle_Index = gl_InstanceID * 8;

vec3 Particle_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);
vec3 Particle_Orientation = vec3(Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5], Particle_Data[Particle_Index + 6]);

float Particle_Age = Particle_Data[Particle_Index + 3];

float Natural_Delta_Time = log(Particle_Age + 1);

float Random_Number = Particle_Data[Particle_Index + 7];

vec2 Rotated_Position()
{
	float Sin = sin(Particle_Age + Random_Number);
	float Cos = cos(Particle_Age + Random_Number);

	vec2 Rotated;

	float Temp = In_Position.x;

	Rotated.x = In_Position.x * Cos + In_Position.y * Sin;
	Rotated.y = In_Position.y * Cos - Temp * Sin;

	return Rotated;
}

void main()
{
	vec4 Transformed_Position = vec4(Particle_Position, 1);

	// We want to apply the transformations on this in order to point it in the right direction

	//

	Transformed_Position.xyz += Particle_Orientation * Particle_Age * (4.0f * Random_Number + 1.0f);

	Transformed_Position.y += 4.0f * Particle_Age * Particle_Age;

	vec2 Rotated = Rotated_Position();

	Transformed_Position.xyz -= Camera_Right_Direction * Rotated.x;
	Transformed_Position.xyz += Camera_Up_Direction * Rotated.y;

	gl_Position = Projection_Matrix * Transformed_Position;

	//data_out.Normal = normalize(Camera_Position - Transformed_Position.xyz);
	UV = In_UV;
	//data_out.Projection_Matrix = Projection_Matrix;

	//data_out.Lighting_Transparency = 0.5;
	
	Vertex_Transparency = 3.0f - Particle_Age; //0.5f * max(0.0f, 0.5f - 0.0142857142857f * (Particle_Age * Particle_Age));
}
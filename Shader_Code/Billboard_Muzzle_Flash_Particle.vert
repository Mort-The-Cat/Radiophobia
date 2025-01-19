#version 440

layout(location = 0) in vec2 In_Position;
layout(location = 1) in vec2 In_UV;

uniform mat4 Projection_Matrix;

out vec3 Normal;
out vec4 Position;

out vec3 UV_Tangent;
out vec3 UV_Bitangent;

out vec2 UV;
out float Lighting_Transparency;
out float Vertex_Transparency;

uniform float Particle_Data[300]; // muzzle flash has 5 floats total

uniform vec3 Camera_Position;
uniform vec3 Camera_Up_Direction;
uniform vec3 Camera_Direction;

vec3 Camera_Right_Direction = cross(Camera_Direction, Camera_Up_Direction);

int Particle_Index = gl_InstanceID * 5;

vec3 Particle_Position = vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]);

float Particle_Age = Particle_Data[Particle_Index + 3];

float Particle_Angle = (6 + 10 * Particle_Age) * Particle_Data[Particle_Index + 4];

vec2 Rotated_Position()
{
	float Sin = sin(Particle_Angle);
	float Cos = cos(Particle_Angle);

	vec2 Rotated;

	float Temp = In_Position.x;

	Rotated.x = In_Position.x * Cos + In_Position.y * Sin;
	Rotated.y = In_Position.y * Cos - Temp * Sin;

	return Rotated;
}

void main()
{
	vec4 Transformed_Position = vec4(Particle_Position, 1);

	vec2 Rotated = Rotated_Position() * vec2(10 * Particle_Age); // * vec2(log(Particle_Age * 0.5 + 1) + 1);

	Transformed_Position.xyz -= Camera_Right_Direction * Rotated.x;
	Transformed_Position.xyz += Camera_Up_Direction * Rotated.y;

	Position = Transformed_Position;

	gl_Position = Projection_Matrix * Transformed_Position;

	Normal = normalize(Camera_Position - Transformed_Position.xyz);
	UV = In_UV;

	Lighting_Transparency = 0.5;

	Particle_Age *= Particle_Age; // x^2
	Particle_Age *= Particle_Age; // x^4
	
	Vertex_Transparency = 0.35 - 512 * Particle_Age; // The flash lasts a very short time
}
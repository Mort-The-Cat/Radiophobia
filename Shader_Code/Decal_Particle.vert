#version 440

layout(location = 0) in uint Index;

uniform mat4 Projection_Matrix;

uniform float Particle_Data[1200]; // Decal has 24 floats

int Particle_Index = gl_InstanceID * 24;

out vec4 Position;
out vec2 UV;

vec3 Vertices[3] = vec3[](
	vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]),
	vec3(Particle_Data[Particle_Index + 3], Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5]),
	vec3(Particle_Data[Particle_Index + 6], Particle_Data[Particle_Index + 7], Particle_Data[Particle_Index + 8])
);

vec3 In_Normal = vec3(Particle_Data[Particle_Index + 9], Particle_Data[Particle_Index + 10], Particle_Data[Particle_Index + 11]);
vec3 In_Bitangent = vec3(Particle_Data[Particle_Index + 12], Particle_Data[Particle_Index + 13], Particle_Data[Particle_Index + 14]);
vec3 In_Tangent = vec3(Particle_Data[Particle_Index + 15], Particle_Data[Particle_Index + 16], Particle_Data[Particle_Index + 17]);

vec2 UVs[3] = vec2[](
	vec2( Particle_Data[Particle_Index + 18], Particle_Data[Particle_Index + 19] ),
	vec2( Particle_Data[Particle_Index + 20], Particle_Data[Particle_Index + 21] ),
	vec2( Particle_Data[Particle_Index + 22], Particle_Data[Particle_Index + 23] )
);

out vec3 Normal;
out vec3 UV_Tangent;
out vec3 UV_Bitangent;

void main()
{
	vec4 Transformed_Position = vec4(Vertices[Index], 1);

	Position = Transformed_Position;

	Transformed_Position = Projection_Matrix * Transformed_Position;

	gl_Position = Transformed_Position;

	UV = UVs[Index];

	Normal = In_Normal;
	UV_Bitangent = In_Bitangent;
	UV_Tangent = In_Tangent;
}

#version 440

layout(location = 0) in uint Index;

uniform mat4 Projection_Matrix;

uniform float Particle_Data[375]; // Decal has 15 floats

int Particle_Index = gl_InstanceID * 15;

out vec4 Position;
out vec2 UV;

vec3 Vertices[3] = vec3[](
	vec3(Particle_Data[Particle_Index], Particle_Data[Particle_Index + 1], Particle_Data[Particle_Index + 2]),
	vec3(Particle_Data[Particle_Index + 3], Particle_Data[Particle_Index + 4], Particle_Data[Particle_Index + 5]),
	vec3(Particle_Data[Particle_Index + 6], Particle_Data[Particle_Index + 7], Particle_Data[Particle_Index + 8])
);

vec2 UVs[3] = vec2[](
	vec2( Particle_Data[Particle_Index + 9], Particle_Data[Particle_Index + 10] ),
	vec2( Particle_Data[Particle_Index + 11], Particle_Data[Particle_Index + 12] ),
	vec2( Particle_Data[Particle_Index + 13], Particle_Data[Particle_Index + 14] )
);

void main()
{
	vec4 Transformed_Position = vec4(Vertices[Index], 1);

	Position = Transformed_Position;

	Transformed_Position = Projection_Matrix * Transformed_Position;

	gl_Position = Transformed_Position;

	UV = UVs[Index];
}

#version 440

layout(location = 0) out vec4 Out_Colour;

layout(location = 1) out vec4 Position_Out;

// We don't need to store the position of the decal

layout(location = 2) out vec4 Normal_Out;
layout(location = 3) out vec4 Material_Out;

uniform sampler2D Albedo;
uniform sampler2D Material;

in vec2 UV;
in vec4 Position;

in vec3 Normal;
in vec3 UV_Tangent;
in vec3 UV_Bitangent;

in vec3 Calculated_Vertices[3];

mat3 TBN(vec3 P_Normal)
{
	vec3 Tangent = UV_Tangent;

	vec3 Bitangent = UV_Bitangent;

	mat3 Matrix = (mat3(Tangent, Normal, Bitangent));

	return Matrix;
}

vec2 Calculated_UV;

vec3 Normal_Map_Read()
{
	vec3 New_Values;

	New_Values.z = -(texture(Material, Calculated_UV).z * 2 - 1);
	New_Values.x = -(texture(Material, Calculated_UV).w * 2 - 1);

	New_Values.y = sqrt(1 - (New_Values.x * New_Values.x + New_Values.z * New_Values.z));

	return New_Values;
}

vec3 Final_Normal;

float Get_Transparency(vec2 UV)
{
	
	float T = atan(UV.x / UV.y);

	return length(UV) < 0.15 + 0.0625 * cos( cos(3 * T) * 4 * sin(8 * T) ) ? 1.0f : 0.0f;
}

void main()
{
	
	float Transparency = Get_Transparency(UV - vec2(0.5f)); //length(UV - vec2(0.5f)) < 0.5f ? texture(Albedo, UV).r : 0.0f;

	if(Transparency < 0.1f)
		discard;

	Calculated_UV = vec2(dot(UV_Tangent, Position.xyz), dot(UV_Bitangent, Position.xyz));

	Final_Normal = normalize(Normal);

	mat3 TBN = TBN(Final_Normal);
	
	Final_Normal = normalize(TBN * Normal_Map_Read());

	Normal_Out = vec4(Final_Normal, 0);

	Material_Out = vec4(texture(Material, Calculated_UV).xy, 0, 0);

	Out_Colour = texture(Albedo, Calculated_UV); // * vec4(1, 1, 1, 0.5f);
}
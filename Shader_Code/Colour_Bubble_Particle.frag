#version 440

layout(location = 0) out vec4 Out_Colour;
layout(location = 3) out vec4 Material_Out;

in vec2 Refracted_Vector;
in vec3 Position;
in vec3 Normal;

uniform vec2 Inv_Window_Dimensions;

uniform samplerCube Cubemap;

uniform vec3 Camera_Position;
uniform vec3 Camera_Direction;

float Get_Colour_Of_Bubble_Edge()
{
	vec3 Normalised_To_Fragment_Vector = normalize(Position - Camera_Position);
	float Cos_Theta = dot(Normalised_To_Fragment_Vector, Normal);

	Cos_Theta = (1 - Cos_Theta * Cos_Theta); // "Cos_Theta" = sin(x)^2

	Cos_Theta = Cos_Theta * Cos_Theta;

	// this gives us "Cos_Theta" = sin(x)^4

	return Cos_Theta * 0.5f;
}

void main()
{
	vec2 Fragment_Vector = gl_FragCoord.xy * Inv_Window_Dimensions * vec2(2.0f) - vec2(1.0f);

	vec2 Tangential_Difference = Refracted_Vector.xy - Fragment_Vector.xy;

	Tangential_Difference /= (vec2(1.0f) + Refracted_Vector.xy * Fragment_Vector.xy);

	// Material_Out.zw = Tangential_Difference;

	Out_Colour = vec4(Get_Colour_Of_Bubble_Edge());
}
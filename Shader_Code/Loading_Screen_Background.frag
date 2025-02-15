#version 440

layout(location = 0) out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Albedo;

uniform vec4 Colour;

uniform float Time;

vec2 New_UV()
{
	vec2 New = vec2(UV.x + 0.075f * Time, UV.y);

	return New;
}

vec4 Get_Colour(vec2 Coord)
{
	vec4 Colour[] = 
	{
		vec4(1 - Coord.y) * 0.25 + vec4(0.043, 0.2, 0.136, 1.0),
		vec4(1 - Coord.y) * 0.25 + vec4(0.066, 0.33, 0.165, 1.0),
	};

	Coord.xy *= vec2(16, 10);

	return Colour[((int(Coord.y) + int(Coord.x)) % 2)];
}

void main()
{
	vec4 Sampled_Colour = Get_Colour(New_UV()) * vec4(0.75, 0.75, 0.75, 1.0);

	Out_Colour = Sampled_Colour;
	
}
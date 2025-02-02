#version 440

layout(location = 0) out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Albedo;

uniform vec4 Colour;

uniform float Time;

vec2 New_UV()
{
	vec2 New = vec2(UV.x, UV.y + sin(UV.y * 2.0f + Time) + Time);

	New.y *= 0.5f;

	// if(int(New.y) % 2 == 0)
	// 	New.x = -New.x;

	return New;
}

void main()
{
	// vec2 Position = New_UV();

	// gl_FragColor = vec4(vec3(1.0f), exp(-20 * abs( Position.x - 0.25f * (1.0f + sin(Position.y)) )));

	vec4 Sampled_Colour = texture(Albedo, New_UV());

	Sampled_Colour *= Sampled_Colour;

	Sampled_Colour *= Colour;

	//if(abs(Sampled_Colour.x - 0.27450980392156865f) < 0.01f)	// If background colour,
	//	discard;

	Out_Colour = Sampled_Colour; //texture(Albedo, New_UV()); // Very sweet little function

	// gl_FragColor = vec4(1, 1, 1, 1);
}
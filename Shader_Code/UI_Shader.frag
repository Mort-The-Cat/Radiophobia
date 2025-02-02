#version 440

layout(location = 0) out vec4 Out_Colour;

in vec2 UV;

uniform sampler2D Albedo;

uniform vec4 Colour;

void main()
{
	Out_Colour = Colour * texture(Albedo, UV); // Very sweet little function

	// gl_FragColor = vec4(1, 1, 1, 1);
}
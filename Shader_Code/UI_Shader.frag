#version 440

in vec2 UV;

uniform sampler2D Albedo;

uniform vec4 Colour;

void main()
{
	gl_FragColor = Colour * texture(Albedo, UV); // Very sweet little function

	// gl_FragColor = vec4(1, 1, 1, 1);
}
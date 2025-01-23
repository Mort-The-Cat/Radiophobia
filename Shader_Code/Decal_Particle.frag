#version 440

layout(location = 0) out vec4 Out_Colour;

uniform sampler2D Albedo;

in vec2 UV;
in vec4 Position;

void main()
{
	Out_Colour = texture(Albedo, UV);
}
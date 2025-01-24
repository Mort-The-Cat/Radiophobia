#version 440

layout(location = 0) out vec4 Out_Colour;

uniform sampler2D Albedo;

in vec2 UV;
in vec4 Position;

void main()
{
	if(texture(Albedo, UV).w < 0.1f)
		discard;

	Out_Colour = texture(Albedo, UV); // * vec4(1, 1, 1, 0.5f);
}
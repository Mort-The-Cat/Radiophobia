#version 440

in vec2 UV;

flat in uint Index;

uniform sampler2D Albedo;

uniform uint Character_Indices[128];

uniform vec4 Colour;

void main()
{
	uint X = Character_Indices[Index] & 15u;
	uint Y = Character_Indices[Index] >> 4u;

	vec4 Texture_Colour = texture(Albedo, UV + vec2( float(X) / 16.0f, float(Y) / 5.0f ) );

	if(Texture_Colour.a < 0.5f)
		discard;

	gl_FragColor = Colour * Texture_Colour; // Very sweet little function

	// gl_FragColor = vec4(0.01f * Character_Indices[Index], 1 - 0.01f * Character_Indices[Index], 0, 1);
}
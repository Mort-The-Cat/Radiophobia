#version 440

layout(location = 0) out vec4 Colour;

in vec3 Position;

uniform sampler2D Screen_Texture;
uniform sampler2D Position_Texture;

uniform vec2 Screen_Dimensions;

uniform vec3 Camera_Position;

flat in float Volume_Density_Out;
flat in float C_Out;
flat in float Cosine_Theta_Squared_Out;
flat in float Dot_Source_Direction_Out;

flat in vec3 Cone_Origin_Out;
flat in vec3 Cone_Direction_Out;
flat in vec3 Colour_Out;

void main()
{
	vec3 To_Fragment = normalize(Camera_Position - Position);

	float Dot_A_Delta = dot(To_Fragment, Cone_Direction_Out);
	float Dot_A_Source = dot(To_Fragment, (Camera_Position - Cone_Origin_Out));

	// Dot_A_A is always equal to 1 because it's a normalized vector- we don't have to worry about it

	float A = Dot_A_Delta * Dot_A_Delta - Cosine_Theta_Squared_Out; // * Dot_A_A
	float B = 2 * (Dot_A_Delta * Dot_Source_Direction_Out - Cosine_Theta_Squared_Out * Dot_A_Source);

	float Discriminant = B * B - 4 * A * C_Out;

	float Delta = abs(sqrt(Discriminant) / A);

	float Delta_2 = length(texture(Position_Texture, gl_FragCoord.xy * Screen_Dimensions).xyz - Position);

	if(Discriminant < 0 || Delta_2 < Delta || Dot_Source_Direction_Out == -1.0f)
		Delta = Delta_2;

	float Transparency = 0.95f - exp(-Volume_Density_Out * Delta);

	Colour = vec4(Colour_Out, Transparency);
}
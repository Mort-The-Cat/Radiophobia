#version 440

layout(location = 0) in vec3 In_Position;
layout(location = 1) in vec3 In_Normal;
layout(location = 2) in vec2 In_UV;

// layout(location = 4) in float In_Occlusion; // We don't worry about this so much at the moment

uniform mat4 Model_Matrix;
uniform mat4 Projection_Matrix;
uniform vec3 Model_Position;

out DATA
{
	vec3 Normal;
	mat4 Projection_Matrix;
	vec2 UV;
	float Lighting_Transparency;
	
	float Vertex_Transparency;
} data_out;

// out vec3 Position;
// out vec3 Normal;
// out vec2 UV;

void main()
{	
	vec4 Transformed_Position = Model_Matrix * vec4(In_Position, 1.0f);
	
	gl_Position = Transformed_Position;	// The projection matrix is applied in the geometry shader, so we just want the transformed position for now.
	
	data_out.Normal = normalize((Model_Matrix * vec4(In_Normal, 1.0f)).xyz - Model_Position);
	data_out.UV = In_UV;
	data_out.Projection_Matrix = Projection_Matrix;
	
	data_out.Lighting_Transparency = 0;
	
	data_out.Vertex_Transparency = 1;
	
	//UV_Tangent = (Model_Matrix * vec4(In_UV_Tangent, 1)).xyz - Model_Position;
}
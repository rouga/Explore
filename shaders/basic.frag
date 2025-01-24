#version 460

struct Light
{
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	float Intensity;
	int isDirectional;
	int IsPoint;
	int isSpot;
};

layout (binding=0) readonly uniform FrameUB
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	Light[8] LightData;
	int LightCount;
} in_FrameUB;

layout (set=1, binding=4) readonly uniform ObjectUB
{
	mat4 ModelMatrix;
	mat4 NormalMatrix;
	int HasUV;
} in_ObjectUB;

layout (set=1, binding=5) uniform sampler2D AlbedoTex;

layout(location = 0) out vec4 out_Color;

layout(location = 0) in vec2 in_UVs;
layout(location = 1) in vec3 in_Normals;

void main()
{
	out_Color = vec4(0.0, 0.0, 0.0, 1.0);
	vec3 Color = vec3(1.0, 0.0, 0.0);
	
	if(in_ObjectUB.HasUV != 0)
	{
		Color = vec3(texture(AlbedoTex, in_UVs));
	}
	
	vec3 normal = normalize(in_Normals);
	vec3 lightDir = vec3(0.0, 0.0, 0.0);
	
	for(int i=0; i < in_FrameUB.LightCount; i++)
	{
		if(in_FrameUB.LightData[i].isDirectional != 0)
		{
			lightDir = in_FrameUB.LightData[i].Direction;
		}
		
		float diff = max(dot(normal, lightDir), 0.0);
		out_Color += vec4(vec3(in_FrameUB.LightData[i].Color * (Color * diff * in_FrameUB.LightData[i].Intensity)), 0);
	}
}
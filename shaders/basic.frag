#version 460

layout(location = 0) out vec4 out_Color;

layout(location = 0) in vec2 in_UVs;

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

layout (set=1, binding=3) readonly uniform ObjectUB
{
	mat4 ModelMatrix;
	int HasUV;
} in_ObjectUB;

layout (set=1, binding=4) uniform sampler2D AlbedoTex;

void main()
{
	if(in_ObjectUB.HasUV != 0)
	{
		out_Color = texture(AlbedoTex, in_UVs);
	}
	else
	{
		out_Color = vec4(1.0, 0.0, 0.0, 1.0);
	}
}
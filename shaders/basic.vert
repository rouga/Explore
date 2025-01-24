#version 460

struct Vertex
{
	float x, y, z;
};

struct Normal
{
	float x, y, z;
};

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

layout (set=1,binding=0) readonly buffer Vertices 
{ 
	Vertex data[]; 
} in_Vertices;

layout (set=1,binding=1) readonly buffer Indices 
{ 
	uint data[]; 
} in_Indices;

layout (set=1,binding=2) readonly buffer Normals 
{ 
	Normal data[]; 
} in_Normals;

layout (set=1,binding=3) readonly buffer UVs 
{ 
	vec2 data[]; 
} in_UVs;

layout (set=1, binding=4) readonly uniform ObjectUB
{
	mat4 ModelMatrix;
	mat4 NormalMatrix;
	int HasUV;
} in_ObjectUB;

layout(location = 0) out vec2 out_UVs;
layout(location = 1) out vec3 out_Normal;

void main() 
{
	uint index = in_Indices.data[gl_VertexIndex];	
	Vertex vertex = in_Vertices.data[index];
	Normal normal = in_Normals.data[index];
	out_Normal = normalize(mat3(in_ObjectUB.NormalMatrix) * vec3(normal.x, normal.y, normal.z));
	
	gl_Position = in_FrameUB.ProjectionMatrix * in_FrameUB.ViewMatrix  * in_ObjectUB.ModelMatrix * vec4(vertex.x, vertex.y, vertex.z, 1.0);
	
	out_UVs = vec2(0.0, 0.0);
	if(in_ObjectUB.HasUV != 0)
	{
		out_UVs = vec2(in_UVs.data[index]);
	}
}

#version 460

struct PositionData
{
	float x, y ,z;
};

layout (binding=0) readonly uniform FrameUB
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
} in_FrameUB;

layout (set=1,binding=0) readonly buffer Vertices 
{ 
	PositionData data[]; 
} in_Vertices;

layout (set=1,binding=1) readonly buffer Indices 
{ 
	uint data[]; 
} in_Indices;

layout (set=1,binding=2) readonly buffer UVs 
{ 
	vec2 data[]; 
} in_UVs;

layout (set=1, binding=3) readonly uniform ObjectUB
{
	mat4 ModelMatrix;
	int HasUV;
} in_ObjectUB;

layout(location = 0) out vec2 out_UVs;

void main() 
{
	uint index = in_Indices.data[gl_VertexIndex];	
	PositionData vertex = in_Vertices.data[index];
    gl_Position = in_FrameUB.ProjectionMatrix * in_FrameUB.ViewMatrix  * in_ObjectUB.ModelMatrix * vec4(vertex.x, vertex.y, vertex.z, 1.0);
	if(in_ObjectUB.HasUV != 0)
	{
		out_UVs = vec2(in_UVs.data[index]);
	}
	else
	{
		out_UVs = vec2(0.0, 0.0);
	}
}

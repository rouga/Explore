#version 460

struct VertexData
{
	float x, y ,z;
	float u, v;
};

layout (binding=0) readonly uniform FrameUB
{
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
} in_FrameUB;

layout (set=1,binding=0) readonly buffer Vertices 
{ 
	VertexData data[]; 
} in_Vertices;

void main() 
{
	VertexData vertex = in_Vertices.data[gl_VertexIndex];
    gl_Position = in_FrameUB.ProjectionMatrix * in_FrameUB.ViewMatrix  * vec4(vertex.x, vertex.y, vertex.z, 1.0) ;
}

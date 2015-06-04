#version 330 

uniform mat4 mvpMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalModelViewMatrix;

in vec3 vert;

smooth out vec3 o_Position;
smooth out vec3 o_Normal;

void main()
{	
    // Transform the position to view space
    o_Position = vec3(modelViewMatrix * vec4(vert, 1));

    // For a sphere, the normals are in the same direction as the vertex
    // Thus, simply normalize the vertex and transform it to view space (remove translations)
    o_Normal = normalize(normalModelViewMatrix * vert);

    gl_Position = mvpMatrix * vec4(vert, 1.0);
    // gl_Position = ftransform();
}

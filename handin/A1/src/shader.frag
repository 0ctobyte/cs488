#version 330

uniform int cindex;

out vec4 finalColor;

vec3 colours[] = vec3[]( 
  vec3(1.0f, 0.4f, 0.4f),
  vec3(1.0f, 0.7f, 0.4f),
  vec3(1.0f, 1.0f, 0.4f),
  vec3(0.7f, 1.0f, 0.4f),
  vec3(0.4f, 1.0f, 0.4f),
  vec3(0.4f, 1.0f, 0.7f),
  vec3(0.4f, 1.0f, 1.0f),
  vec3(0.4f, 0.7f, 1.0f),
  vec3(0.4f, 0.4f, 0.4f),
  vec3(1.0f, 0.0f, 0.0f)
);


void main()
{
    finalColor = vec4(colours[cindex], 1.0);
    // gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}


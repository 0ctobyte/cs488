#version 330

struct Material {
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Camera {
  vec3 position;
};

struct LightSource {
  vec3 position;
  vec3 intensity;
};

uniform Material material = Material(vec3(0.7, 0.7, 0.7), vec3(1.0, 0.0, 0.0), 80.0);
uniform Camera camera = Camera(vec3(0.0, 0.0, 0.0));
uniform LightSource lightSource = LightSource(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

// The normals and positions are interpolated for each pixel
smooth in vec3 o_Position;
smooth in vec3 o_Normal;

out vec4 finalColor;

void main()
{
    // Calculate vector from surface position to light source
    vec3 surf_to_light = lightSource.position - o_Position;

    // Calculate the cosine of the angle of incidence (brightness)
    // (no need to divide the dot product by the product of the lengths of the vectors since they have been normalized)
    // Brightness must be clamped between 0 and 1 (anything less than 0 means 0 brightness)
    float brightness = max(0.0, dot(o_Normal, normalize(surf_to_light)));

    // Calculate the diffuse component
    vec3 diffuse = brightness * material.diffuse * lightSource.intensity;

    // Calculate the angle of reflectance.
    // The surf_to_light needs to go in the opposite direction in order to represent the angle of incidence
    vec3 incidence = normalize(-surf_to_light);
    vec3 reflection = reflect(incidence, o_Normal);
    vec3 surf_to_cam = normalize(camera.position - o_Position);
    float specular_brightness = max(0.0, dot(surf_to_cam, reflection));
    float specular_coefficient = (brightness > 0.0) ? pow(specular_brightness, material.shininess) : 0.0;

    // Calculate the specular component
    vec3 specular = specular_coefficient * material.specular * lightSource.intensity;

    // Calculate the final color based on
    // 1. The diffuse component
    // 2. The specular component
    finalColor = vec4(diffuse+specular, 1.0);
}


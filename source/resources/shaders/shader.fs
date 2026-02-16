#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 vColor;
out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
  // Normalize normal
  vec3 norm = normalize(Normal);

  // Lambertian diffuse
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);

  vec3 diffuse = diff * vColor;

  // Ambient term
  vec3 ambient = 0.2 * vColor;

  FragColor = vec4(ambient + diffuse, 1.0);
}

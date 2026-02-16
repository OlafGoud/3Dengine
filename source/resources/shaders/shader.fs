#version 330 core
in vec3 FragColor;
in vec3 FragPos;
in vec3 Normal;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
  // simple Phong lighting
  vec3 ambient = 0.1 * FragColor;

  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * FragColor;

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = vec3(0.02) * spec;

  vec3 result = ambient + diffuse + specular;
  color = vec4(result, 1.0);
}

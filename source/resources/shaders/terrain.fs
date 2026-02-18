#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

out vec4 FragColor;

void main() {
  vec3 lightDir = normalize(vec3(-1.0,2.0,1.0));
  float diff = max(dot(normalize(Normal),lightDir),0.0);
  vec3 lighting = (0.2 + diff) * Color;
  FragColor = vec4(lighting,1.0);
}

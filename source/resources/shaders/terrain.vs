#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  FragPos = vec3(model * vec4(aPos,1.0));
  Normal = mat3(transpose(inverse(model))) * aNormal;


  if(aPos.y > 5) {
    Color = vec3(0.9, 0.9, 0.9);
  } else if(aPos.y > 2) {
    Color = vec3(0.4, 0.4, 0.4);
  } else if(aPos.y > 1) {
    Color = vec3(aColor.r + 0.05, aColor.g + 0.05, aColor.b + 0.05);
  } else if (aPos.y < -2.5) {
    Color = vec3(0.1, 0.1, 0.8);
  } else if(aPos.y < -1.3) {
    Color = vec3(aColor.r - 0.05, aColor.g - 0.05, aColor.b - 0.05);
  } else {
    Color = aColor;
  }

  float diff = (aPos.y + 30) / 60;

  Color = aColor;


  gl_Position = projection * view * vec4(FragPos,1.0);
}
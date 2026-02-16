#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 vColor;

uniform mat4 uMVP;
uniform mat4 uModel;

void main() {
  gl_Position = uMVP * vec4(aPos, 1.0);
  FragPos = vec3(uModel * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(uModel))) * aNormal; // normalize inside fragment shader
  vColor = aColor;
}

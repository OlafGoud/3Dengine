#version 330 core
in vec3 worldPos;
out vec4 FragColor;

uniform float pctR;
uniform float pctG;
uniform float pctB;
uniform float noiseScale;

float hash(vec3 p){
  return fract(sin(dot(p,vec3(12.9898,78.233,54.53)))*43758.5453);
}

void main()
{
  float n = hash(worldPos * noiseScale);
  n = clamp(n,0.0,1.0);

  vec3 base = vec3(
    n * pctR,
    n * pctG,
    n * pctB
  );

  FragColor = vec4(1.0);
}
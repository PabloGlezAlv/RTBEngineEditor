#version 430 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
out vec3 vNormal;
out vec3 vFragPos;
uniform mat4 uModel;
layout(std140, binding = 1) uniform CameraData { mat4 view; mat4 projection; vec3 viewPos; };
void main() {
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    gl_Position = projection * view * worldPos;
    vFragPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
}

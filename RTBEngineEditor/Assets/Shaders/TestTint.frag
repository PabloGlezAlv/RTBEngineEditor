#version 430 core
in vec3 vNormal;
in vec3 vFragPos;
uniform vec4 uColor;
out vec4 FragColor;
void main() {
    vec3 lit = normalize(vNormal) * 0.5 + 0.5;
    FragColor = vec4(lit * vec3(1.0, 0.25, 0.25) * uColor.rgb, uColor.a);
}

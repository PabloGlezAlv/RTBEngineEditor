#version 430 core

in vec3 vNormal;
in vec3 vFragPos;
in vec3 vObjectPos;

uniform vec4 uColor;
uniform float uTime;
uniform float uPulseSpeed;
uniform float uGlowIntensity;

layout(std140, binding = 1) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

out vec4 FragColor;

float emberHash(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 37.719))) * 43758.5453);
}

void main() {
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vFragPos);
    float nDotV = max(dot(normal, viewDir), 0.0);

    float outerFresnel = pow(1.0 - nDotV, 2.2);
    float hotRim = pow(1.0 - nDotV, 5.0);
    float coreMask = pow(nDotV, 0.35);

    vec3 sphereDir = normalize(vObjectPos);
    float longitude = atan(sphereDir.z, sphereDir.x);
    float latitude = sphereDir.y;
    float time = uTime * uPulseSpeed;

    // Sharp heat cracks — unlike the smooth mana swirls on MagicOrb.
    float cracks = sin(longitude * 11.0 - time * 3.5) * sin(latitude * 18.0 + time * 2.2);
    cracks = smoothstep(0.15, 0.85, cracks * 0.5 + 0.5);

    float sparks = emberHash(sphereDir * 9.0 + floor(time * 2.0));
    sparks = smoothstep(0.82, 1.0, sparks);

    float rings = sin(longitude * 8.0 - time * 5.0) * 0.5 + 0.5;
    float heat = mix(cracks, rings, 0.4) + sparks * 0.35;

    // Erratic flicker (campfire / ember bolt) vs smooth arcane pulse.
    float flicker = 0.78
        + 0.14 * step(0.5, fract(sin(time * 4.7) * 43758.5453))
        + 0.08 * sin(time * 6.3 + latitude * 5.0);

    vec3 tint = uColor.rgb;
    vec3 charredShell = tint * vec3(0.22, 0.05, 0.02);
    vec3 moltenCrack = mix(vec3(1.0, 0.38, 0.04), tint * 1.9, 0.45);
    vec3 hotCore = mix(vec3(1.0, 0.82, 0.35), tint * 2.0, 0.25);
    vec3 outerFlare = mix(vec3(1.0, 0.45, 0.08), tint * 1.3, 0.4);

    vec3 color = vec3(0.0);
    color += charredShell * (1.0 - coreMask) * 0.55;
    color += moltenCrack * heat * coreMask * flicker;
    color += hotCore * coreMask * coreMask * (0.65 + 0.35 * heat) * flicker;
    color += outerFlare * outerFresnel * uGlowIntensity * flicker;
    color += vec3(1.0, 0.95, 0.7) * hotRim * 0.35 * flicker;

    color = color / (color + vec3(0.28));

    FragColor = vec4(color, uColor.a);
}

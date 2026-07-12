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

// Cheap procedural turbulence — common in stylized magic FX (swirling mana veins).
float magicTurbulence(vec3 p, float time) {
    float waveA = sin(dot(p, vec3(3.1, 2.7, 2.3)) + time * 1.7);
    float waveB = sin(p.x * 5.3 + p.y * 4.1 - time * 2.1);
    float waveC = sin(p.z * 6.0 + time * 1.3 + p.y * 2.5);
    return waveA * 0.4 + waveB * 0.35 + waveC * 0.25;
}

void main() {
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(viewPos - vFragPos);
    float nDotV = max(dot(normal, viewDir), 0.0);

    // Layered Fresnel: soft outer halo + tight hot rim (Overwatch / ARPG orb style).
    float outerFresnel = pow(1.0 - nDotV, 1.6);
    float innerRim = pow(1.0 - nDotV, 4.5);

    // Bright core when looking into the sphere center.
    float coreMask = pow(nDotV, 0.45);

    // Surface energy swirl in object space (reads well on a small projectile).
    vec3 sphereDir = normalize(vObjectPos);
    float longitude = atan(sphereDir.z, sphereDir.x);
    float latitude = sphereDir.y;
    float time = uTime * uPulseSpeed;

    float spiral = sin(longitude * 6.0 + latitude * 8.0 + time * 2.0) * 0.5 + 0.5;
    float bands = sin(latitude * 14.0 - time * 2.8) * 0.5 + 0.5;
    float turbulence = magicTurbulence(sphereDir * 2.5, time) * 0.5 + 0.5;
    float energy = mix(spiral, bands, 0.55) * (0.65 + 0.35 * turbulence);

    // Multi-harmonic pulse — less mechanical than a single sine.
    float pulse = 0.90
        + 0.08 * sin(time)
        + 0.05 * sin(time * 2.37 + 1.2)
        + 0.03 * sin(time * 3.91);

    // Purple palette: deep violet body, magenta veins, lavender rim (tinted by uColor).
    vec3 tint = uColor.rgb;
    vec3 deepBody = tint * vec3(0.45, 0.08, 0.72);
    vec3 energyVein = mix(tint * 1.35, vec3(0.95, 0.45, 1.0), 0.55);
    vec3 outerGlow = mix(tint * 1.1, vec3(0.82, 0.55, 1.0), 0.45);
    vec3 hotCore = mix(vec3(1.0, 0.88, 1.0), tint * 1.6, 0.35);

    vec3 color = vec3(0.0);
    color += deepBody * coreMask * (0.35 + 0.25 * pulse);
    color += energyVein * energy * coreMask * (0.55 + 0.45 * pulse);
    color += hotCore * coreMask * energy * 0.45 * pulse;
    color += outerGlow * outerFresnel * uGlowIntensity * pulse;
    color += vec3(1.0, 0.92, 1.0) * innerRim * 0.42 * pulse;

    // Soft highlight compression — keeps the orb bright without blowing out.
    color = color / (color + vec3(0.35));

    FragColor = vec4(color, uColor.a);
}

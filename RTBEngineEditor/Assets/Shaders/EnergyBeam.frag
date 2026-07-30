#version 330 core

in vec4 vColor;
in vec2 vUV;
in vec3 vNormal;
in vec3 vWorldPos;
in float vShell;

uniform vec4 uBeamColor;
uniform vec4 uCoreColor;
uniform float uEmissionStrength;
uniform float uNoiseScale;
uniform float uNoiseSpeed;
uniform float uDistortionStrength;
uniform float uFresnelPower;
uniform float uGlowIntensity;
uniform float uPulseSpeed;
uniform float uPulseAmount;
uniform float uSoftEdge;
uniform float uTime;
uniform vec3 uViewPos;

out vec4 FragColor;

float hash21(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float valueNoise(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash21(i);
    float b = hash21(i + vec2(1.0, 0.0));
    float c = hash21(i + vec2(0.0, 1.0));
    float d = hash21(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p)
{
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < 4; ++i) {
        v += a * valueNoise(p);
        p = p * 2.03 + vec2(17.1, 9.3);
        a *= 0.5;
    }
    return v;
}

void main()
{
    float t = uTime * uNoiseSpeed;
    vec2 nUV = vec2(vUV.x * uNoiseScale - t, vUV.y * uNoiseScale * 1.35);
    float n1 = fbm(nUV);
    float n2 = fbm(nUV * 1.7 + vec2(3.1, t * 0.35));
    float noise = mix(n1, n2, 0.45);
    float flow = fbm(vec2(vUV.x * (uNoiseScale * 0.65) - t * 1.4, vUV.y * 2.0 + noise * uDistortionStrength));

    vec3 N = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vWorldPos);
    float ndotv = clamp(abs(dot(N, viewDir)), 0.0, 1.0);
    float fresnel = pow(1.0 - ndotv, max(uFresnelPower, 0.001));
    float soft = max(uSoftEdge, 0.0001);
    float face = pow(clamp(ndotv, 0.0, 1.0), soft * 0.25);

    float pulse = 1.0 + sin(uTime * uPulseSpeed + vUV.x * 6.0) * uPulseAmount * 0.5;

    float isCore = 1.0 - step(0.5, vShell);
    float isInner = step(0.5, vShell) * (1.0 - step(1.5, vShell));
    float isOuter = step(1.5, vShell);

    // Keep hue: modulate brightness in a narrow range, avoid whitening.
    vec3 tint = mix(uBeamColor.rgb, uCoreColor.rgb, isCore * 0.85);
    float energy = mix(0.72, 1.08, noise) * mix(0.85, 1.12, flow);
    float rim = fresnel * uGlowIntensity;

    vec3 rgb = tint * energy;
    rgb += tint * rim * mix(0.2, 0.55, isOuter + isInner * 0.35);
    rgb += uCoreColor.rgb * isCore * (0.25 + flow * 0.15);
    // Mild emission: bloom handles the glow, shader keeps readable color.
    rgb *= (0.65 + uEmissionStrength * 0.45) * pulse;

    float alpha = vColor.a;
    alpha *= mix(mix(uBeamColor.a, uBeamColor.a * 0.85, isInner), uCoreColor.a, isCore);
    alpha *= mix(0.35, 0.85, isOuter);   // outer aura softer
    alpha *= mix(0.75, 1.05, isInner);
    alpha *= mix(0.9, 1.15, isCore);
    alpha *= mix(0.55, 1.0, face);
    alpha *= mix(0.8, 1.1, noise);
    alpha += rim * 0.12 * (0.35 + 0.65 * isOuter);
    alpha = clamp(alpha, 0.0, 1.0);

    FragColor = vec4(rgb * vColor.rgb, alpha);
}

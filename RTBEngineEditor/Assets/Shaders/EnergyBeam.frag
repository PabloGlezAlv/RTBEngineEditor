#version 330 core

in vec4 vColor;
in vec2 vUV;
in float vSide;
in vec3 vWorldPos;

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
    float absSide = abs(vSide);
    float radial = max(1.0 - absSide, 0.0);

    float soft = max(uSoftEdge, 0.0001);
    float edgeAlpha = pow(radial, soft);

    float t = uTime * uNoiseSpeed;
    vec2 nUV = vec2(vUV.x * uNoiseScale - t, vUV.y * 2.0);
    float n1 = fbm(nUV);
    float n2 = fbm(nUV * 1.7 + vec2(3.1, t * 0.35));
    float noise = mix(n1, n2, 0.45);

    float distort = (noise - 0.5) * uDistortionStrength;
    float radialNoise = max(1.0 - abs(absSide + distort * 0.35), 0.0);

    float coreMask = smoothstep(0.42, 0.0, absSide);
    float innerMask = pow(radialNoise, 1.15);
    float outerMask = pow(radial, mix(1.8, 0.9, clamp(uGlowIntensity * 0.35, 0.0, 1.0)));

    float pulse = 1.0 + sin(uTime * uPulseSpeed + vUV.x * 6.0) * uPulseAmount;

    vec3 viewDir = normalize(uViewPos - vWorldPos);
    float fresnel = pow(1.0 - clamp(abs(dot(viewDir, vec3(0.0, 1.0, 0.0))), 0.0, 1.0), max(uFresnelPower, 0.001));

    vec3 core = uCoreColor.rgb * (1.15 + noise * 0.35);
    vec3 body = uBeamColor.rgb * (0.75 + noise * 0.55);
    vec3 rgb = mix(body * outerMask, body * innerMask, 0.55);
    rgb = mix(rgb, core, coreMask);
    rgb += body * fresnel * 0.35 * uGlowIntensity;
    rgb *= uEmissionStrength * pulse * uGlowIntensity;

    float alpha = vColor.a * uBeamColor.a * max(edgeAlpha, coreMask * 0.95);
    alpha *= mix(0.75, 1.15, noise);
    alpha = clamp(alpha, 0.0, 1.0);

    FragColor = vec4(rgb * vColor.rgb, alpha);
}

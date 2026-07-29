#version 330 core

in vec4 vColor;
in vec2 vUV;
in float vSide;

uniform float uSoftEdge;
uniform int uHasTexture;
uniform sampler2D uDiffuse;
uniform float uLaserCore;
uniform float uLaserNoise;

out vec4 FragColor;

void main()
{
    float absSide = abs(vSide);
    float radial = max(1.0 - absSide, 0.0);

    float edgeAlpha = 1.0;
    if (uSoftEdge > 0.0001) {
        float soft = max(uSoftEdge, 0.0001);
        edgeAlpha = pow(radial, soft);
    }

    vec4 texColor = vec4(1.0);
    if (uHasTexture != 0) {
        texColor = texture(uDiffuse, vUV);
        float lum = max(texColor.r, max(texColor.g, texColor.b));
        texColor.a = max(texColor.a, lum);
    }

    // Default path keeps normal aim trails unchanged.
    if (uLaserCore <= 0.0001) {
        vec4 color = vColor * texColor;
        color.a *= edgeAlpha;
        FragColor = color;
        return;
    }

    // Stylized laser: hot white core + colored soft bloom + light energy shimmer.
    float coreWidth = mix(0.28, 0.10, clamp(uLaserCore, 0.0, 1.0));
    float coreMask = smoothstep(coreWidth, 0.0, absSide);
    float glowMask = pow(radial, mix(1.6, 0.75, clamp(uLaserCore, 0.0, 1.0)));

    float shimmer = 1.0;
    if (uLaserNoise > 0.0001) {
        float wave = sin(vUV.x * 14.0) * 0.5 + 0.5;
        shimmer = mix(1.0, 0.78 + 0.40 * wave, clamp(uLaserNoise, 0.0, 1.0));
    }

    vec3 tint = vColor.rgb * texColor.rgb;
    vec3 coreColor = mix(tint, vec3(1.0, 0.98, 0.94), coreMask * 0.95);
    vec3 rgb = mix(tint * glowMask, coreColor, coreMask) * shimmer;

    float alpha = vColor.a * texColor.a * max(edgeAlpha, coreMask);
    FragColor = vec4(rgb, alpha);
}

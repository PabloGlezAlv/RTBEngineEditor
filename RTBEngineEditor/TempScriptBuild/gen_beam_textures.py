from PIL import Image
import math
import random
import os

out_dir = r"C:\Users\pablo\Desktop\Proyectos\RTBEngine\RTBEngineEditor\RTBEngineEditor\Assets\Textures\VFX\Beam"
os.makedirs(out_dir, exist_ok=True)

W, H = 512, 128


def soft_profile(y, h, power=2.2):
    t = abs((y + 0.5) / h - 0.5) * 2.0
    return max(0.0, 1.0 - t) ** power


def make_beam(path, rgb, power, streak_amp, streak_freq, seed, edge_boost=0.0):
    rnd = random.Random(seed)
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    px = img.load()

    # Continuous flow: high-frequency low-amplitude longitudinal modulation
    wave = [0.0] * W
    for x in range(W):
        u = x / W
        wave[x] = 1.0
        wave[x] += streak_amp * 0.55 * math.sin(u * math.pi * 2 * streak_freq + seed)
        wave[x] += streak_amp * 0.30 * math.sin(u * math.pi * 2 * (streak_freq * 2.3) + seed * 1.9)
        wave[x] += streak_amp * 0.15 * math.sin(u * math.pi * 2 * (streak_freq * 5.1) + seed * 0.7)

    for y in range(H):
        base = soft_profile(y, H, power=power)
        # slight vertical micro-noise for plasma feel
        for x in range(W):
            a = base * wave[x]
            a += (rnd.random() - 0.5) * 0.015 * base
            if edge_boost > 0.0 and base > 0.2:
                a = min(1.0, a + edge_boost * (base ** 2))
            a = max(0.0, min(1.0, a))
            if a <= 0.002:
                px[x, y] = (0, 0, 0, 0)
                continue
            px[x, y] = (
                int(rgb[0] * 255),
                int(rgb[1] * 255),
                int(rgb[2] * 255),
                int(a * 255),
            )

    # Seamless U wrap blend
    blend = 12
    for y in range(H):
        for i in range(blend):
            t = (i + 1) / (blend + 1)
            left = px[i, y]
            right = px[W - 1 - i, y]
            px[i, y] = tuple(int(left[c] * (1 - t) + right[c] * t) for c in range(4))
            px[W - 1 - i, y] = tuple(int(right[c] * (1 - t) + left[c] * t) for c in range(4))

    img.save(path, "PNG")
    print("wrote", path)


# Continuous DBZ-like ribbons
make_beam(os.path.join(out_dir, "BeamCore.png"), (1.00, 1.00, 0.96), 4.2, 0.08, 6.0, 11, edge_boost=0.12)
make_beam(os.path.join(out_dir, "BeamAura.png"), (0.40, 0.88, 1.00), 2.15, 0.12, 4.5, 22)
make_beam(os.path.join(out_dir, "BeamHalo.png"), (0.20, 0.42, 1.00), 1.45, 0.07, 2.5, 33)
make_beam(os.path.join(out_dir, "BeamPreview.png"), (0.88, 0.96, 1.00), 2.6, 0.05, 3.0, 44)

# QA: stacked additive-ish preview
bg = Image.new("RGBA", (512, 360), (10, 10, 14, 255))
y = 30
for name, tint in [
    ("BeamHalo.png", 1.0),
    ("BeamAura.png", 1.0),
    ("BeamCore.png", 1.0),
]:
    im = Image.open(os.path.join(out_dir, name)).convert("RGBA")
    layer = Image.new("RGBA", bg.size, (0, 0, 0, 0))
    layer.paste(im, (0, y), im)
    bg = Image.alpha_composite(bg, layer)
    y += 100

# Also one stacked single beam simulation (same Y)
stack = Image.new("RGBA", (512, 160), (10, 10, 14, 255))
for name in ["BeamHalo.png", "BeamAura.png", "BeamCore.png"]:
    im = Image.open(os.path.join(out_dir, name)).convert("RGBA")
    layer = Image.new("RGBA", stack.size, (0, 0, 0, 0))
    layer.paste(im, (0, 16), im)
    stack = Image.alpha_composite(stack, layer)

bg.convert("RGB").save(os.path.join(out_dir, "_qa_layers.png"))
stack.convert("RGB").save(os.path.join(out_dir, "_qa_stacked.png"))
print("qa done")

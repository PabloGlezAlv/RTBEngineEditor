import re
from pathlib import Path

PLAYER_VISUAL_UUID = "5200AC9B-039A-416E-82AE-FD5FE1754C95"


def extract_balanced_block(text: str, start_index: int):
    depth = 0
    i = start_index
    while i < len(text):
        ch = text[i]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                return text[start_index : i + 1], i + 1
        i += 1
    raise ValueError("unbalanced")


text = Path("Assets/Prefabs/Player Ranger.prefab").read_text(encoding="utf-8")
match = re.search(r"return\s*\{", text)
base, _ = extract_balanced_block(text, match.end() - 1)
marker = f'uuid = "{PLAYER_VISUAL_UUID}"'
idx = base.index(marker)
brace_start = base.rfind("{", 0, idx)
block, end = extract_balanced_block(base, brace_start)
print("block len", len(block))
print("after end:", repr(base[end : end + 150]))

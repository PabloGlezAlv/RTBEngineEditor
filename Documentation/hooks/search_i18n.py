"""Copy the search index under /en/search.

English pages sometimes request that path. The i18n plugin only writes
the combined index at the site root, and the search worker reads that
root index for both languages.
"""

import shutil
from pathlib import Path

from mkdocs import plugins


@plugins.event_priority(-150)
def on_post_build(config, **kwargs):
    site = Path(config["site_dir"])
    source = site / "search" / "search_index.json"
    if not source.exists():
        return

    target_dir = site / "en" / "search"
    target_dir.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(source, target_dir / "search_index.json")

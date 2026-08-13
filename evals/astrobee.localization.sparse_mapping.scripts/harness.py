"""Harness for astrobee.localization.sparse_mapping.scripts.

reorder_directories(directory) renames a map-partition directory's numeric
subdirectories so they are sequential from 0. It takes a path and acts on the
filesystem, so a fixture row can't call it directly: build the directory
layout the row describes in a temp dir (outside the repo root — the effect
probe ignores writes there), run the unit, and return the resulting listing.
"""

import os
import shutil
import tempfile


def reorder_in_tempdir(row, ctx):
    base = tempfile.mkdtemp(prefix="ent-sparse-mapping-")
    try:
        for name in row["input"]["dirs"]:
            os.mkdir(os.path.join(base, name))
        ctx.entrypoint(base)
        return sorted(os.listdir(base))
    finally:
        shutil.rmtree(base, ignore_errors=True)

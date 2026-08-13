"""Harness for astrobee.localization.sparse_mapping.tools.

parse_localization_log_str(log_str) is the unit's pure core: it extracts
(image names, error values) from localization log text. It returns a tuple
of two lists; fixture rows compare JSON, so hand back [images, errors].
"""


def parse_log_as_lists(row, ctx):
    return list(ctx.entrypoint(row["input"]))

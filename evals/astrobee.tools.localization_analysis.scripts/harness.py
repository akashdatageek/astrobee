"""Harness for astrobee.tools.localization_analysis.scripts.

check_and_fill_line(value_map, config_file_line) is the unit's ROS-free core:
it rewrites a config line when its first token is a swept parameter. Two
arguments, so a fixture row can't call it directly — unpack the row.
"""


def fill_config_line(row, ctx):
    return ctx.entrypoint(row["input"]["valueMap"], row["input"]["line"])

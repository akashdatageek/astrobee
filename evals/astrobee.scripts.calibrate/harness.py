"""Harness for astrobee.scripts.calibrate.

lua_find_transform(text, transform_name) locates `name = transform(...)` in a
lua config and returns (matched_text, open_idx, close_idx). Two arguments and
a tuple result, so adapt: unpack the row, return just the matched text — the
part a caller consumes; None when the transform isn't present.
"""


def match_lua_transform(row, ctx):
    return ctx.entrypoint(row["input"]["text"], row["input"]["name"])[0]

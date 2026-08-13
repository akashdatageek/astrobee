#!/bin/bash
set -euo pipefail

# Only needed in Claude Code on the web, where each session starts in a
# fresh container.
if [ "${CLAUDE_CODE_REMOTE:-}" != "true" ]; then
  exit 0
fi

# The container's Debian-packaged PyJWT has no RECORD file (pip cannot
# upgrade it in place), and its cryptography package is broken (missing
# _cffi_backend), so replace all three in /usr/local ahead of time.
pip install --quiet --ignore-installed pyjwt cffi cryptography

# entiendo 0.2.0 uses the mcp.server.fastmcp API, which mcp 2.x removed,
# so hold mcp at 1.x.
pip install --quiet "entiendo[mcp]==0.2.0" "mcp>=1.0,<2"

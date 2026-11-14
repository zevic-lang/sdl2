#!/usr/bin/env bash
# Runs a command; when it fails, the end of its output is also written as a GitHub error annotation, so the
# reason is on the summary page of the run and not only in the log (which needs a sign-in to read).
#
#   scripts/annotate.sh "title" command args...
set -uo pipefail
title="$1"
shift
log="$(mktemp)"
"$@" 2>&1 | tee "$log"
code="${PIPESTATUS[0]}"
if [ "$code" -ne 0 ]; then
    # annotations are one line: % \r and \n have to be escaped
    # the lines that say what went wrong first, then the end of the output
    { grep -E 'FAIL|error|Error|fatal|panic' "$log" | head -n 15; echo "--- end of the output ---"; tail -n 12 "$log"; } \
        | tr -d '\r' | sed -e 's/%/%25/g' | awk '{printf "%s%%0A", $0}' > "$log.body"
    body="$(cat "$log.body")"
    rm -f "$log.body"
    echo "::error title=${title} (exit ${code})::${body}"
fi
rm -f "$log"
exit "$code"

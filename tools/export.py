"""Regenerate the offline demo's trace data from the compiled C++ executable."""
import csv
import io
import json
from pathlib import Path
import subprocess
import sys

traces = {}
for mode in ("drop", "clean"):
    output = subprocess.check_output([sys.argv[1], "--" + mode], text=True)
    rows = list(csv.DictReader(io.StringIO(output)))
    for row in rows:
        for key in row.keys() - {"event"}:
            row[key] = int(row[key]) if row[key] else None
    traces[mode] = rows

target = Path(__file__).resolve().parents[1] / "docs" / "traces.js"
target.write_bytes(("const traces = " + json.dumps(traces, indent=2) + ";\n").encode())

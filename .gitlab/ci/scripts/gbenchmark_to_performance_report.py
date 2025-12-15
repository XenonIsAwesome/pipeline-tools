import json
import sys

with open(sys.argv[1]) as f:
    data = json.load(f)

perf = []
for bm in data.get("benchmarks", []):
    name = bm["name"]
    unit = bm["time_unit"]

    metrics = []
    for t in ["real_time", "cpu_time"]:
        if t in bm:
            metrics.append({"name": f"{t}_{unit}", "value": bm[t], "unit": unit})
    perf.append({"subject": name, "metrics": metrics})

json.dump(perf, sys.stdout, indent=2)

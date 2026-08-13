"""Harness for astrobee.scripts.postprocessing.coverage_analysis.

The unit's ROS-free core is Coverage_StatsReporter: it bins per-cube ML
feature counts into a five-bin coverage distribution per ISS wall. The
entrypoint resolves to the class; construct one, feed it the row's counts,
and return the resulting distribution
["", %bin1..%bin5, total features] for exact comparison.
"""


def drive_wall_coverage(row, ctx):
    reporter = ctx.entrypoint()
    for count in row["input"]["featuresPerCube"]:
        reporter.analyze_wall_coverage(count)
    return list(reporter.coverage_per_wall)

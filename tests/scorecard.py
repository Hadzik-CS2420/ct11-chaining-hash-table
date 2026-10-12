#!/usr/bin/env python3
"""Print your CT 11: Chaining Hash Tables score.

    python3 tests/scorecard.py

Run it from the top of your repository -- the folder that holds CMakeLists.txt,
not from inside tests/. It builds if it needs to, runs the Google Test suite,
and prints a scored breakdown that matches what the GitHub autograder will
award when the grader is run.

Points are per test, exactly as .github/workflows/classroom.yml awards them.
The "n of m" column shows how many tests in a section pass; unlike CT 01 there
is partial credit inside a section, because each test is graded on its own.

The identity in the header is taken from the repository, never typed in: the
git remote carries the GitHub username the Repo Creator assigned you, and
user.name / user.email are the identity stamped on your commits.
"""

import datetime
import json
import os
import subprocess
import sys
import tempfile

sys.stdout.reconfigure(encoding="utf-8")

WIDTH = 72
TASK = "ct10-hash-tables"

# ---------------------------------------------------------------------------
# Section table -- the single source of truth for how CT 11 is scored here.
#
# Every entry is ("Suite.TestName", points). These MUST stay in step with
# .github/workflows/classroom.yml; if you change one, change the other.
# Tests are keyed by suite AND name because names repeat across suites.
# ---------------------------------------------------------------------------
SECTIONS = [
    ("HashTable.cpp             hash()",
     [("HashFunction.ConsistentResults", 2),
      ("HashFunction.WithinRange", 2)]),
    ("HashTable.cpp             insert()",
     [("ChainingInsert.BasicInsert", 3),
      ("ChainingInsert.UpdateExistingKey", 2),
      ("ChainingInsert.HandlesCollisions", 3)]),
    ("HashTable.cpp             search()",
     [("ChainingSearch.FindsExistingKey", 2),
      ("ChainingSearch.ReturnsNullForMissing", 2),
      ("ChainingSearch.EmptyTable", 2)]),
    ("HashTable.cpp             remove()",
     [("ChainingRemove.RemovesExistingKey", 2),
      ("ChainingRemove.ReturnsFalseForMissing", 2),
      ("ChainingRemove.RemoveFromCollisionChain", 2)]),
    ("HashTable.cpp             resize()",
     [("ChainingResize.TriggersOnHighLoadFactor", 3),
      ("ChainingResize.AllEntriesSurvive", 3)]),
]

TOTAL = sum(pts for _, tests in SECTIONS for _, pts in tests)

GREEN, RED, YELLOW, RESET = "\033[32m", "\033[31m", "\033[33m", "\033[0m"


def use_color():
    return sys.stdout.isatty() and os.environ.get("TERM", "") != "dumb"


def _git(*args):
    try:
        out = subprocess.run(("git",) + args, capture_output=True, text=True)
        return out.stdout.strip() if out.returncode == 0 else ""
    except OSError:
        return ""


def identity():
    """Who ran this and where, for the header of the scorecard."""
    remote = _git("remote", "get-url", "origin")
    repo = ""
    if remote:
        repo = remote.rstrip("/").rsplit("/", 1)[-1]
        if repo.endswith(".git"):
            repo = repo[:-4]

    # Repos are named "<task>-<github username>". Strip the known task prefix
    # rather than splitting on the last "-": GitHub usernames may contain
    # hyphens, so rsplit would turn "scott-sbctraining" into "sbctraining" and
    # quietly name the wrong student.
    user = repo[len(TASK) + 1:] if repo.startswith(TASK + "-") else ""

    who = " ".join(x for x in (_git("config", "--get", "user.name"),
                               _git("config", "--get", "user.email")) if x)
    return {
        "user": user or "(unknown -- see Repository below)",
        "repo": repo or "(no git remote -- did you clone, or download a ZIP?)",
        "who": who or "(git identity not set -- see Module 0 setup)",
    }


def run_suite():
    """Build if needed, run the tests, return gtest's JSON result."""
    exe = os.path.join("build", "run_tests")
    if not os.path.exists(exe):
        print("No build/run_tests yet -- building. This may take a minute the "
              "first time while Google Test downloads.\n")
        if subprocess.run(["cmake", "-B", "build"]).returncode != 0:
            sys.exit("cmake -B build failed. Fix the configure error, then "
                     "run this again.")
        if subprocess.run(["cmake", "--build", "build"]).returncode != 0:
            sys.exit("The build failed, so there is nothing to score yet. "
                     "Fix the compile errors and run this again.")

    out = os.path.join(tempfile.gettempdir(), "ct10-hash-tables_results.json")
    # A non-zero exit just means tests failed, which is expected while you are
    # still working -- so the return code is deliberately ignored here.
    subprocess.run([exe, "--gtest_output=json:" + out],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    if not os.path.exists(out):
        sys.exit("run_tests produced no results file.")
    with open(out, encoding="utf-8") as fh:
        return json.load(fh)


def tally(results):
    """Return {"Suite.TestName": True/False} -- True when the test passed."""
    passed = {}
    for suite in results.get("testsuites", []):
        # Parameterized suites are reported as "Prefix/SuiteName", so keep only
        # the part after the slash to match the table above.
        name = suite.get("name", "").split("/")[-1]
        for case in suite.get("testsuite", []):
            key = name + "." + case.get("name", "").split("/")[0]
            passed[key] = not case.get("failures")
    return passed


def main():
    if not os.path.exists("CMakeLists.txt"):
        sys.exit("Run this from the top of your repository -- the folder that "
                 "holds CMakeLists.txt, not from inside tests/.")

    passed = tally(run_suite())
    color = use_color()

    def line(text, state=None):
        if color and state is not None:
            text = ({"all": GREEN, "some": YELLOW, "none": RED}[state]
                    + text + RESET)
        print(text)

    who = identity()
    print("")
    print("=" * WIDTH)
    print("YOUR SCORE -- CT 07: Dynamic Arrays".center(WIDTH))
    print("=" * WIDTH)
    print("  %-13s %s" % ("GitHub user", who["user"]))
    print("  %-13s %s" % ("Repository", who["repo"]))
    print("  %-13s %s" % ("Submitted by", who["who"]))
    print("  %-13s %s" % ("Run at",
                          datetime.datetime.now().strftime("%Y-%m-%d %H:%M")))
    print("-" * WIDTH)

    missing = [k for _, tests in SECTIONS for k, _ in tests if k not in passed]
    if len(missing) == sum(len(t) for _, t in SECTIONS):
        line("  No tests ran at all -- the build is broken. Fix that first.",
             "none")
        print("-" * WIDTH)

    earned = 0
    for label, tests in SECTIONS:
        got = sum(pts for k, pts in tests if passed.get(k))
        possible = sum(pts for _, pts in tests)
        n = sum(1 for k, _ in tests if passed.get(k))
        earned += got
        state = "all" if n == len(tests) else ("none" if n == 0 else "some")
        line("  %-42s %2d of %-2d  %2d/%-2d" % (label, n, len(tests),
                                                got, possible), state)

    print("-" * WIDTH)
    pct = (earned * 100.0 / TOTAL) if TOTAL else 0.0
    line("  %-42s %8s %3d/%-3d (%.0f%%)" % ("TOTAL", "", earned, TOTAL, pct),
         "all" if earned == TOTAL else ("none" if earned == 0 else "some"))
    print("=" * WIDTH)

    if earned < TOTAL:
        print("")
        print("  To see which tests are still failing:")
        print("")
        print("      ./build/run_tests --gtest_brief=1")
        print("")
        print("  To work on one section at a time:")
        print("")
        print("      ./build/run_tests --gtest_filter=DynamicArraysTest.*")
        print("")
    else:
        print("")
        print("  Full marks. Commit, push, and submit your repository URL")
        print("  in Canvas.")
        print("")

    # Exit 0 always. This is a report, not a gate; a non-zero exit here would
    # make it look like the script itself had broken.
    return 0


if __name__ == "__main__":
    sys.exit(main())

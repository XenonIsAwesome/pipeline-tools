import os
import subprocess
import glob
import argparse
from pathlib import Path

def run_gcov(search_dir):
    # Find all .gcda files
    gcda_files = glob.glob(os.path.join(search_dir, "**/*.gcda"), recursive=True)
    if not gcda_files:
        print(f"No .gcda files found in {search_dir}. Did you run the tests?")
        return

    print(f"Found {len(gcda_files)} .gcda files. Running gcov...")
    
    for gcda in gcda_files:
        # Run gcov in the directory of the gcda file
        # We use -p to preserve the directory structure in gcov filenames
        # and -l to handle long filenames (optional but helpful)
        cwd = os.path.dirname(gcda)
        subprocess.run(["gcov", "-p", "-l", os.path.basename(gcda)], cwd=cwd, capture_output=True)

def parse_gcov_files(search_dir):
    gcov_files = glob.glob(os.path.join(search_dir, "**/*.gcov"), recursive=True)
    if not gcov_files:
        print("No .gcov files generated.")
        return

    stats = {}
    
    for gcov in gcov_files:
        try:
            with open(gcov, 'r', errors='ignore') as f:
                total_executable = 0
                covered = 0
                source_file = None
                
                for line in f:
                    # Format:  hits:  line:code
                    parts = line.split(':', 2)
                    if len(parts) < 3:
                        continue
                    
                    hits = parts[0].strip()
                    line_no = parts[1].strip()
                    
                    if line_no == "0" and "Source" in parts[2]:
                        # Metadata line: 0:Source:path/to/source
                        source_file = parts[2].split(':', 1)[1].strip()
                        continue
                    
                    if hits == "-": # Not an executable line
                        continue
                    
                    if line_no == "0": # Other metadata
                        continue

                    total_executable += 1
                    # hits can be a number (executed) or ##### / ==== (not executed)
                    if hits.isdigit() and int(hits) > 0:
                        covered += 1
                
                if source_file and total_executable > 0:
                    # Merge results if we see the same source file multiple times (due to multiple object files)
                    if source_file in stats:
                        old_covered, old_total = stats[source_file]
                        stats[source_file] = (old_covered + covered, old_total + total_executable)
                    else:
                        stats[source_file] = (covered, total_executable)
        except Exception as e:
            print(f"Error parsing {gcov}: {e}")

    return stats

def main():
    parser = argparse.ArgumentParser(description="Mock SonarQube Coverage Parser")
    parser.add_argument("--build-dir", default="build", help="Directory to search for .gcda and .gcov files")
    parser.add_argument("--silent", action="store_true")

    # TODO: Currently not used...
    parser.add_argument("--source-dir", default=Path(), help="Directory to filter coverage for")

    args = parser.parse_args()

    # Step 1: Run gcov to generate reports from data files
    run_gcov(args.search_dir)
    
    # Step 2: Parse the generated .gcov files
    stats = parse_gcov_files(args.search_dir)
    
    if not stats:
        print("No coverage data found. Ensure you built with ENABLE_COVERAGE=ON and ran the tests.")
        return

    if args.silent:
        return

    print("\n--- Coverage Summary (Mock SonarQube) ---")
    print(f"{'Source File':<60} | {'Coverage':<10}")
    print("-" * 75)
    
    total_hits = 0
    total_lines = 0
    
    # Clean up paths to make them more readable (relative to project root if possible)
    # We'll just show the tail if it's too long
    for source in sorted(stats.keys()):
        hits, lines = stats[source]
        percentage = (hits / lines) * 100
        
        display_name = source
        if len(display_name) > 60:
            display_name = "..." + display_name[-57:]
            
        print(f"{display_name:<60} | {percentage:>6.2f}% ({hits}/{lines})")
        total_hits += hits
        total_lines += lines
        
    if total_lines > 0:
        total_percentage = (total_hits / total_lines) * 100
        print("-" * 75)
        print(f"{'TOTAL':<60} | {total_percentage:>6.2f}% ({total_hits}/{total_lines})")

if __name__ == "__main__":
    main()

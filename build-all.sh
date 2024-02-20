#!/usr/bin/env bash

# Exit the script if any command fails
set -e

c_compiler=""
clang_format_name="clang-format"
clang_tidy_name="clang-tidy"
cppcheck_name="cppcheck"

# Function to display script usage
usage()
{
    echo "Usage: $0 [-f <clang-format>] [-t <clang-tidy>] [-k <cppcheck>]"
    echo "  -f clang-format   Specify the clang-format name (e.g. clang-tidy or clang-tidy-17)"
    echo "  -t clang-tidy     Specify the clang-tidy name (e.g. clang-tidy or clang-tidy-17)"
    echo "  -k cppcheck       Specify the cppcheck name (e.g. cppcheck)"
    exit 1
}

# Parse command-line options using getopt
while getopts ":f:t:k:" opt; do
  case $opt in
    f)
      clang_format_name="$OPTARG"
      ;;
    t)
      clang_tidy_name="$OPTARG"
      ;;
    k)
      cppcheck_name="$OPTARG"
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      usage
      ;;
  esac
done

# Read the C compilers from the file into an array
c_compilers=()
while IFS= read -r line; do
    c_compilers+=("$line")
done < "supported_c_compilers.txt"

# Loop through the C compilers array
for c_compiler in "${c_compilers[@]}"; do
    echo "$c_compiler"

    ./generate-cmakelists.sh
    ./change-compiler.sh -c "$c_compiler" -f "$clang_format_name" -t "$clang_tidy_name" -k "$cppcheck_name"
    ./build.sh
done

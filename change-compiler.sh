#!/usr/bin/env bash

c_compiler=""
clang_format_name="clang-format"
clang_tidy_name="clang-tidy"
cppcheck_name="cppcheck"
sanitizers="address,leak,pointer_overflow,undefined"

# Function to display script usage
usage()
{
    echo "Usage: $0 -c <c compiler> [-f <clang-format>] [-t <clang-tidy>] [-k <cppcheck>] [-s <sanitizers>]"
    echo "  -c c compiler   Specify the c++ compiler name (e.g. gcc or clang)"
    echo "  -f clang-format   Specify the clang-format name (e.g. clang-tidy or clang-tidy-17)"
    echo "  -t clang-tidy     Specify the clang-tidy name (e.g. clang-tidy or clang-tidy-17)"
    echo "  -k cppcheck       Specify the cppcheck name (e.g. cppcheck)"
    echo "  -s sanitizers     Specify the sanitiers to use name (e.g. address,undefined)"
    exit 1
}

# Parse command-line options using getopt
while getopts ":c:f:t:k:s:" opt; do
  case $opt in
    c)
      c_compiler="$OPTARG"
      ;;
    f)
      clang_format_name="$OPTARG"
      ;;
    t)
      clang_tidy_name="$OPTARG"
      ;;
    k)
      cppcheck_name="$OPTARG"
      ;;
    s)
      sanitizers="$OPTARG"
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

# Check if the compiler argument is provided
if [ -z "$c_compiler" ]; then
  echo "Error: c compiler argument (-c) is required."
  usage
fi

./check-env.sh -c "$c_compiler" -f "$clang_format_name" -t "$clang_tidy_name" -k "$cppcheck_name"

if [ ! -f "supported_c_compilers.txt" ] || ! grep -Fxq "$c_compiler" supported_c_compilers.txt; then
   ./check-compilers.sh
fi

if [ ! -d "./.flags/$c_compiler" ]; then
    ./generate-flags.sh
fi

# Split the sanitizers string and construct flags
IFS=',' read -ra SANITIZERS <<< "$sanitizers"
for sanitizer in "${SANITIZERS[@]}"; do
    sanitizer_flags+="-DSANITIZER_${sanitizer}=ON "
done

echo "$sanitizer_flags"
cmake -S . -B build -DCMAKE_C_COMPILER="$c_compiler" -DCLANG_FORMAT_NAME="$clang_format_name" -DCLANG_TIDY_NAME="$clang_tidy_name" -DCPPCHECK_NAME="$cppcheck_name" $sanitizer_flags -DCMAKE_BUILD_TYPE=Debug

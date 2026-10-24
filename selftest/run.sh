#!/usr/bin/env bash
# Tests the tester itself; no libft needed. `make selftest` runs it.
#
#  1. Compiles the framework, the runner and every test with -Werror, the
#     tests against selftest/libft/libft.h (only the subject's prototypes).
#  2. Runs selftest/framework_test.c, whose cases each do one thing the
#     framework must catch, in both output modes, and compares the output
#     with selftest/expected.txt.
#
# UPDATE=1 rewrites expected.txt: read the diff before committing it.
# The expected output has glibc's signal names, so step 2 is for Linux.
set -euo pipefail
cd "$(dirname "$0")/.."

CC="${CC:-cc}"
out=build/selftest
flags=(-Wall -Wextra -Werror -g)
track=(-Dmalloc=tester_malloc -Dfree=tester_free)
rm -rf "$out"
mkdir -p "$out"

echo "selftest: compiling everything with $CC -Werror"
for f in framework/*.c; do
	"$CC" "${flags[@]}" -c "$f" -o "$out/fw_$(basename "$f" .c).o"
done
for f in runner/*.c; do
	"$CC" "${flags[@]}" -c "$f" -o "$out/runner_$(basename "$f" .c).o"
done
"$CC" "$out"/runner_*.o -o "$out/runner"
"$out/runner" -h > /dev/null
for f in tests/*_test.c; do
	"$CC" "${flags[@]}" "${track[@]}" -Iframework -Iselftest/libft \
		-c "$f" -o "$out/test.o"
done

echo "selftest: checking what the framework catches"
"$CC" "${flags[@]}" "${track[@]}" -Iframework -c selftest/framework_test.c \
	-o "$out/framework_test.o"
"$CC" "$out/framework_test.o" "$out"/fw_*.o -o "$out/framework_test"
status=0
{
	NO_COLOR=1 LIBT_TIMEOUT=1 "$out/framework_test" || status=$?
	NO_COLOR=1 LIBT_TIMEOUT=1 "$out/framework_test" -q || true
} > "$out/output.txt" 2>&1
if [ "$status" -ne 1 ]; then
	echo "selftest: FAILED, the suite exited with $status instead of 1"
	exit 1
fi
if [ "${UPDATE:-0}" = 1 ]; then
	cp "$out/output.txt" selftest/expected.txt
	echo "selftest: expected.txt updated"
elif diff -u selftest/expected.txt "$out/output.txt"; then
	echo "selftest: OK"
else
	echo "selftest: FAILED, the output differs from selftest/expected.txt"
	exit 1
fi

#!/usr/bin/env bash
# libt: builds the tests against the libft in the current directory, then
# runs them. Arguments go to the runner (see libt -h), except --asan.

# Where the tester lives, even when this script is reached through a symlink
script="${BASH_SOURCE[0]}"
while [ -L "$script" ]; do
	dir="$(cd -P "$(dirname "$script")" && pwd)"
	script="$(readlink "$script")"
	[[ $script == /* ]] || script="$dir/$script"
done
TESTER_DIR="$(cd -P "$(dirname "$script")" && pwd)"
USER_DIR="$PWD"

args=()
make_args=()
for arg in "$@"; do
	if [ "$arg" = "--asan" ]; then
		make_args+=(SANITIZE=1)
		export ASAN_OPTIONS="detect_leaks=0:exitcode=86"
		export UBSAN_OPTIONS="print_stacktrace=1:exitcode=86"
	else
		args+=("$arg")
	fi
done

shopt -s nullglob
sources=("$USER_DIR"/ft_*.c)
shopt -u nullglob
if [ ${#sources[@]} -eq 0 ]; then
	echo "❌ No ft_*.c files in $USER_DIR"
	echo "💡 Run libt from your libft directory."
	exit 1
fi
if [[ $USER_DIR == *[[:space:]]* ]]; then
	echo "❌ make can't handle spaces in paths: $USER_DIR"
	exit 1
fi

jobs="$(getconf _NPROCESSORS_ONLN 2> /dev/null || echo 4)"
echo "⚙️  Building the tests for $USER_DIR"
if ! make -s -j"$jobs" -C "$TESTER_DIR" SRC_DIR="$USER_DIR" "${make_args[@]}"; then
	echo "❌ The tester itself failed to build."
	exit 1
fi

LIBT_SRC="$USER_DIR" LIBT_BUILD="$TESTER_DIR/build" \
	exec "$TESTER_DIR/build/runner" "${args[@]}"

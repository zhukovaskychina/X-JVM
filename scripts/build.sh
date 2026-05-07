#!/usr/bin/env bash
# X-JVM — Unix/macOS 构建脚本（在仓库根目录执行 cmake 与编译）
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

BUILD_DIR="${BUILD_DIR:-build}"
BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
CLEAN=0
JOBS=""
GENERATOR=""
VERBOSE=0

usage() {
    echo "Usage: $0 [options]"
    echo "  -h          帮助"
    echo "  -c          清理 build 目录后重新配置"
    echo "  -t TYPE     CMAKE_BUILD_TYPE（默认 Release）"
    echo "  -j N        并行编译任务数"
    echo "  -g GEN      CMake -G 生成器（如 Ninja）"
    echo "  -v          cmake --build 详细输出"
    echo "  --no-test   跳过 ctest"
    exit "${1:-0}"
}

RUN_TEST=1
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help) usage 0 ;;
        -c) CLEAN=1; shift ;;
        -t) BUILD_TYPE="$2"; shift 2 ;;
        -j) JOBS="$2"; shift 2 ;;
        -g) GENERATOR="$2"; shift 2 ;;
        -v) VERBOSE=1; shift ;;
        --no-test) RUN_TEST=0; shift ;;
        *) echo "Unknown option: $1"; usage 1 ;;
    esac
done

if [[ "$CLEAN" -eq 1 && -d "$BUILD_DIR" ]]; then
    echo "[INFO] Removing $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

CMAKE_ARGS=(-S "$ROOT" -B "$BUILD_DIR" "-DCMAKE_BUILD_TYPE=$BUILD_TYPE")
if [[ -n "$GENERATOR" ]]; then
    CMAKE_ARGS+=(-G "$GENERATOR")
fi

echo "[INFO] Configure: cmake ${CMAKE_ARGS[*]}"
cmake "${CMAKE_ARGS[@]}"

BUILD_ARGS=(--build "$BUILD_DIR" --config "$BUILD_TYPE")
if [[ -n "$JOBS" ]]; then
    BUILD_ARGS+=(--parallel "$JOBS")
fi
if [[ "$VERBOSE" -eq 1 ]]; then
    BUILD_ARGS+=(--verbose)
fi

echo "[INFO] Build: cmake ${BUILD_ARGS[*]}"
cmake "${BUILD_ARGS[@]}"

if [[ "$RUN_TEST" -eq 1 ]]; then
    echo "[INFO] Test: ctest --test-dir $BUILD_DIR --output-on-failure"
    ctest --test-dir "$BUILD_DIR" --output-on-failure || true
fi

echo "[SUCCESS] Done. Artifacts under $BUILD_DIR/"

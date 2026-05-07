#!/usr/bin/env bash
# X-JVM 示例运行脚本（macOS/Linux）
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

JVM_EXE=""
for d in "$ROOT/build" "$ROOT/build/Debug" "$ROOT/build/Release"; do
    if [[ -x "$d/jvm" ]]; then
        JVM_EXE="$d/jvm"
        break
    fi
done

if [[ -z "$JVM_EXE" ]]; then
    echo "[ERROR] jvm executable not found. Run scripts/build.sh first."
    exit 1
fi

# 自动检测 JRE
JRE="${JRE:-}"
if [[ -z "$JRE" ]]; then
    if command -v /usr/libexec/java_home &>/dev/null; then
        JRE="$(/usr/libexec/java_home 2>/dev/null)"
    fi
fi

if [[ $# -eq 0 ]]; then
    echo "Usage: $0 <class-file> [classpath] [jre-path]"
    echo "Example: $0 examples/HelloWorld.class examples"
    echo "         $0 Hello.class . /usr/lib/jvm/java-11-openjdk"
    "$JVM_EXE" --help
    exit 0
fi

CLASS_FILE="$1"
CP="${2:-.}"
JRE="${3:-$JRE}"

if [[ ! -f "$CLASS_FILE" ]]; then
    echo "[ERROR] Class file not found: $CLASS_FILE"
    exit 1
fi

if [[ -z "$JRE" ]] || [[ ! -d "$JRE" ]]; then
    echo "[ERROR] JRE not found. Set JRE env or pass as 3rd arg."
    exit 1
fi

# 标准化类名：foo/bar/Baz.class -> foo/bar/Baz.class
CLASS_ARG="$CLASS_FILE"
CLASS_ARG="${CLASS_ARG#./}"

echo "[INFO] Running: $JVM_EXE --xjre $JRE --classpath $CP --javaclass $CLASS_ARG"
"$JVM_EXE" --xjre "$JRE" --classpath "$CP" --javaclass "$CLASS_ARG"

#!/usr/bin/env bash
set -euo pipefail

BASE="$HOME/Du/tram-thoi-tiet-host"
SOURCE="$BASE/qt"
BUILD="$BASE/build-host"
QT_HOST="$HOME/Qt6Cross/qt6/host"

rm -rf "$BUILD"

cmake -S "$SOURCE" -B "$BUILD" \
-GNinja \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_PREFIX_PATH="$QT_HOST"

cmake --build "$BUILD" --parallel 2

echo
echo "===== BUILD HOST THANH CONG ====="
file "$BUILD/tram_thoi_tiet_qt"

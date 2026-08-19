#!/usr/bin/env bash
set -euo pipefail

BASE="$HOME/Du/tram-thoi-tiet-host"
SOURCE="$BASE/qt"
BUILD="$BASE/build-arm64"

QT_PI="$HOME/Qt6Cross/qt6/pi"
TOOLCHAIN="$HOME/Qt6Cross/qt6/pi-build/toolchain.cmake"

rm -rf "$BUILD"

cmake -S "$SOURCE" -B "$BUILD" \
-GNinja \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
-DQt6_DIR="$QT_PI/lib/cmake/Qt6" \
-DQt6Core_DIR="$QT_PI/lib/cmake/Qt6Core" \
-DQt6Gui_DIR="$QT_PI/lib/cmake/Qt6Gui" \
-DQt6Widgets_DIR="$QT_PI/lib/cmake/Qt6Widgets" \
-DQt6Sql_DIR="$QT_PI/lib/cmake/Qt6Sql"

cmake --build "$BUILD" --parallel 2

echo
echo "===== BUILD ARM64 THANH CONG ====="
file "$BUILD/tram_thoi_tiet_qt"

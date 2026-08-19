#!/usr/bin/env bash
set -euo pipefail

APP_DIR="$(cd "$(dirname "$0")/.." && pwd)"
QT_DIR="/usr/local/qt6"

export LD_LIBRARY_PATH="$QT_DIR/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="$QT_DIR/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="$QT_DIR/plugins/platforms"
export QT_QPA_PLATFORM=xcb

cd "$APP_DIR"

exec "$APP_DIR/bin/tram_thoi_tiet_qt"

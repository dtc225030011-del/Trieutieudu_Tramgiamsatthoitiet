#!/usr/bin/env bash
set -euo pipefail

BASE="$HOME/Du/tram-thoi-tiet-host"
APP="$BASE/build-host/tram_thoi_tiet_qt"
QT_HOST="$HOME/Qt6Cross/qt6/host"

if [ ! -x "$APP" ]; then
    "$BASE/scripts/build_host.sh"
fi

export LD_LIBRARY_PATH="$QT_HOST/lib:${LD_LIBRARY_PATH:-}"
export QT_PLUGIN_PATH="$QT_HOST/plugins"
export QT_QPA_PLATFORM_PLUGIN_PATH="$QT_HOST/plugins/platforms"
export QT_QPA_PLATFORM=xcb

exec "$APP"

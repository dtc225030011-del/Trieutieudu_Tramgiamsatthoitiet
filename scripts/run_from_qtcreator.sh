#!/bin/bash
set -e

BASE="$HOME/Du/tram-thoi-tiet-host"
PACKAGE="$BASE/package/tram-thoi-tiet"
PI_IP="192.168.137.227"

echo "===== COPY ARM64 VAO PACKAGE ====="

cp -f \
"$BASE/build-arm64/tram_thoi_tiet_qt" \
"$PACKAGE/bin/tram_thoi_tiet_qt"

chmod +x \
"$PACKAGE/bin/tram_thoi_tiet_qt"

echo "===== DONG GOI ====="

rm -f \
"$BASE/package/tram-thoi-tiet-arm64.tar.gz"

tar -C "$BASE/package" \
-czf "$BASE/package/tram-thoi-tiet-arm64.tar.gz" \
"tram-thoi-tiet"

echo "===== DEPLOY LEN RASPBERRY PI ====="

"$BASE/scripts/deploy_pi.sh" "$PI_IP"

echo "===== CHAY GIAO DIEN TREN PI ====="

ssh pi@"$PI_IP" '
pkill -x tram_thoi_tiet_qt 2>/dev/null || true
sleep 1

rm -f /tmp/tram_thoi_tiet_qt.lock
mkdir -p /home/pi/Du/tram-thoi-tiet/logs

nohup env \
DISPLAY=:0 \
QT_QPA_PLATFORM=xcb \
/home/pi/Du/tram-thoi-tiet/scripts/run_pi.sh \
> /home/pi/Du/tram-thoi-tiet/logs/app.log 2>&1 &

sleep 3

echo "===== APP TREN PI ====="
ps -ef | grep "[t]ram_thoi_tiet_qt" || true
'

echo
echo "======================================"
echo " PI DA CHAY GIAO DIEN MOI"
echo "======================================"

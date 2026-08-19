#!/usr/bin/env bash
set -euo pipefail

# ==========================================
# CẤU HÌNH RASPBERRY PI
# Có thể truyền IP khi chạy:
# ./deploy_pi.sh 192.168.137.227
# ==========================================
PI_IP="${1:-192.168.137.227}"
PI_USER="${PI_USER:-pi}"

LOCAL_PACKAGE="$HOME/Du/tram-thoi-tiet-host/package/tram-thoi-tiet-arm64.tar.gz"
REMOTE_BASE="/home/pi/Du"
REMOTE_APP="$REMOTE_BASE/tram-thoi-tiet"
REMOTE_ARCHIVE="/tmp/tram-thoi-tiet-arm64.tar.gz"

echo "=========================================="
echo "TRIEN KHA TRAM THOI TIET"
echo "Raspberry Pi : ${PI_USER}@${PI_IP}"
echo "Thu muc dich : ${REMOTE_APP}"
echo "=========================================="
echo

if [ ! -f "$LOCAL_PACKAGE" ]; then
    echo "LOI: Khong tim thay goi:"
    echo "$LOCAL_PACKAGE"
    echo
    echo "Hay tao goi ARM64 truoc."
    exit 1
fi

echo "[1/5] Kiem tra ket noi SSH..."

ssh \
    -o ConnectTimeout=8 \
    "${PI_USER}@${PI_IP}" \
    "echo 'SSH Raspberry Pi: OK'"

echo
echo "[2/5] Sao chep goi ARM64..."

scp \
    "$LOCAL_PACKAGE" \
    "${PI_USER}@${PI_IP}:${REMOTE_ARCHIVE}"

echo
echo "[3/5] Sao luu phien ban cu neu co..."

ssh "${PI_USER}@${PI_IP}" bash <<'REMOTE_SCRIPT'
set -euo pipefail

REMOTE_BASE="/home/pi/Du"
REMOTE_APP="$REMOTE_BASE/tram-thoi-tiet"
BACKUP_ROOT="$REMOTE_BASE/backup-tram-thoi-tiet"

mkdir -p "$REMOTE_BASE"

if [ -d "$REMOTE_APP" ]; then
    mkdir -p "$BACKUP_ROOT"

    THOI_GIAN="$(date +%Y%m%d_%H%M%S)"
    BACKUP_DIR="$BACKUP_ROOT/tram-thoi-tiet_$THOI_GIAN"

    cp -a "$REMOTE_APP" "$BACKUP_DIR"

    echo "Da sao luu ban cu tai:"
    echo "$BACKUP_DIR"
else
    echo "Chua co phien ban cu, bo qua sao luu."
fi
REMOTE_SCRIPT

echo
echo "[4/5] Giai nen va cai dat..."

ssh "${PI_USER}@${PI_IP}" bash <<'REMOTE_SCRIPT'
set -euo pipefail

REMOTE_BASE="/home/pi/Du"
REMOTE_APP="$REMOTE_BASE/tram-thoi-tiet"
REMOTE_ARCHIVE="/tmp/tram-thoi-tiet-arm64.tar.gz"
TEMP_DIR="/tmp/tram-thoi-tiet-deploy"

rm -rf "$TEMP_DIR"
mkdir -p "$TEMP_DIR"

tar -xzf "$REMOTE_ARCHIVE" -C "$TEMP_DIR"

if [ ! -f "$TEMP_DIR/tram-thoi-tiet/bin/tram_thoi_tiet_qt" ]; then
    echo "LOI: Goi trien khai khong hop le."
    exit 1
fi

mkdir -p "$REMOTE_APP"

# Giữ lại dữ liệu SQLite và cấu hình hiện tại nếu đã tồn tại.
if [ -f "$REMOTE_APP/config/cau_hinh.ini" ]; then
    cp \
        "$REMOTE_APP/config/cau_hinh.ini" \
        "/tmp/cau_hinh_tram_thoi_tiet.ini"
fi

if [ -d "$REMOTE_APP/data" ]; then
    rm -rf "/tmp/data_tram_thoi_tiet"
    cp -a \
        "$REMOTE_APP/data" \
        "/tmp/data_tram_thoi_tiet"
fi

rm -rf "$REMOTE_APP"
cp -a \
    "$TEMP_DIR/tram-thoi-tiet" \
    "$REMOTE_APP"

if [ -f "/tmp/cau_hinh_tram_thoi_tiet.ini" ]; then
    cp \
        "/tmp/cau_hinh_tram_thoi_tiet.ini" \
        "$REMOTE_APP/config/cau_hinh.ini"
fi

if [ -d "/tmp/data_tram_thoi_tiet" ]; then
    rm -rf "$REMOTE_APP/data"
    cp -a \
        "/tmp/data_tram_thoi_tiet" \
        "$REMOTE_APP/data"
fi

mkdir -p \
    "$REMOTE_APP/config" \
    "$REMOTE_APP/data" \
    "$REMOTE_APP/logs"

chmod +x \
    "$REMOTE_APP/bin/tram_thoi_tiet_qt" \
    "$REMOTE_APP/scripts/run_pi.sh"

rm -rf "$TEMP_DIR"
rm -f "$REMOTE_ARCHIVE"

echo "Da cai dat tai:"
echo "$REMOTE_APP"
REMOTE_SCRIPT

echo
echo "[5/5] Kiem tra file ARM64 tren Raspberry Pi..."

ssh "${PI_USER}@${PI_IP}" \
    "file '$REMOTE_APP/bin/tram_thoi_tiet_qt' && \
     ls -lh '$REMOTE_APP/bin/tram_thoi_tiet_qt' && \
     test -x '$REMOTE_APP/scripts/run_pi.sh'"

echo
echo "=========================================="
echo "TRIEN KHA THANH CONG"
echo
echo "Lenh chay tren Raspberry Pi:"
echo "$REMOTE_APP/scripts/run_pi.sh"
echo "=========================================="

#!/usr/bin/env bash
set -euo pipefail

TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$TEST_DIR"

echo "======================================================================"
echo "  [ESP32] BUILD & UNITY TEST - TRAM GIAM SAT THOI TIET"
echo "======================================================================"

make clean
make
echo
./run_esp_tests | tee test_results.log
python3 generate_report.py

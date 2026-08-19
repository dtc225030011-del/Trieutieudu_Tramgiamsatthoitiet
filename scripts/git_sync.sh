#!/usr/bin/env bash
set -e

cd /home/pi/Du/tram-thoi-tiet-host

# Ensure user config
git config user.name "blynkapp04-ui" || true
git config user.email "blynkapp04-ui@users.noreply.github.com" || true

# Add all files
git add -A

# Commit
git commit -m "docs & ci: add professional engineering repository standard files" || true

# Push
git push origin main

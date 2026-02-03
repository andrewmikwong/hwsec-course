#!/bin/bash
set -euo pipefail

echo "=== System + Browser Report (macOS) ==="
echo

# --- OS ---
echo "[OS]"
sw_vers
echo

# --- CPU ---
echo "[CPU]"
if sysctl -n machdep.cpu.brand_string >/dev/null 2>&1; then
  echo "CPU: $(sysctl -n machdep.cpu.brand_string)"
else
  # Fallback for some Apple Silicon setups
  echo "CPU: $(sysctl -n hw.model 2>/dev/null || echo "Unknown")"
fi
echo

# --- RAM ---
echo "[RAM]"
MEM_HUMAN="$(system_profiler SPHardwareDataType 2>/dev/null | awk -F': ' '/Memory:/ {print $2; exit}')"
if [[ -n "${MEM_HUMAN:-}" ]]; then
  echo "RAM: ${MEM_HUMAN}"
else
  # Fallback in bytes -> GB
  BYTES="$(sysctl -n hw.memsize)"
  GB=$(( BYTES / 1024 / 1024 / 1024 ))
  echo "RAM: ${GB} GB"
fi
echo

# --- Cache line size ---
echo "[Cache line size]"
if sysctl -n hw.cachelinesize >/dev/null 2>&1; then
  echo "Cache line size: $(sysctl -n hw.cachelinesize) bytes"
else
  echo "Cache line size: Unknown"
fi
echo

# --- Cache sizes (classic sysctl keys) ---
echo "[Cache sizes]"
get_cache() {
  local key="$1"
  if sysctl -n "$key" >/dev/null 2>&1; then
    local v
    v="$(sysctl -n "$key")"
    if [[ "$v" -gt 0 ]]; then
      echo "$key: $v bytes"
    else
      echo "$key: (not reported)"
    fi
  else
    echo "$key: (not available)"
  fi
}

get_cache hw.l1icachesize
get_cache hw.l1dcachesize
get_cache hw.l2cachesize
get_cache hw.l3cachesize

# --- Apple Silicon perf level L2 caches (often present) ---
if sysctl -n hw.perflevel0.l2cachesize >/dev/null 2>&1 || sysctl -n hw.perflevel1.l2cachesize >/dev/null 2>&1; then
  echo
  echo "[Apple Silicon perf-level L2 caches]"
  get_cache hw.perflevel0.l2cachesize
  get_cache hw.perflevel1.l2cachesize
fi
echo

# --- Browsers ---
echo "[Browser versions]"

# Chrome
CHROME_BIN="/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
if [[ -x "$CHROME_BIN" ]]; then
  echo "Chrome: $("$CHROME_BIN" --version)"
else
  echo "Chrome: Not found"
fi

# Firefox
FIREFOX_BIN="/Applications/Firefox.app/Contents/MacOS/firefox"
if [[ -x "$FIREFOX_BIN" ]]; then
  echo "Firefox: $("$FIREFOX_BIN" --version)"
else
  echo "Firefox: Not found"
fi

# Safari (bundle version)
if [[ -d "/Applications/Safari.app" ]]; then
  SAF_VER="$(defaults read /Applications/Safari.app/Contents/Info CFBundleShortVersionString 2>/dev/null || true)"
  if [[ -n "${SAF_VER:-}" ]]; then
    echo "Safari: $SAF_VER"
  else
    echo "Safari: Found, version not readable"
  fi
else
  echo "Safari: Not found"
fi

echo
echo "=== Done ==="


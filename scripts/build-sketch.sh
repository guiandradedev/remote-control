#!/usr/bin/env bash
# Compila um sketch ESP32 com PlatformIO, em uma pasta temporária.
#
# Uso: scripts/build-sketch.sh esp-code/Q0962-Experimento
#
# Variáveis opcionais:
#   PIO           caminho do executável do PlatformIO
#   IRREMOTE_DIR  pasta da biblioteca IRremote

set -euo pipefail

PIO="${PIO:-$HOME/.platformio/penv/bin/platformio}"
IRREMOTE_DIR="${IRREMOTE_DIR:-$HOME/Arduino/libraries/IRremote}"

if [ $# -ne 1 ] || [ ! -d "$1" ]; then
    echo "Uso: $0 <pasta-do-sketch>" >&2
    exit 1
fi

ORIGEM="$(realpath "$1")"
NOME="$(basename "$ORIGEM")"
TRABALHO="$(mktemp -d)"
trap 'rm -rf "$TRABALHO"' EXIT

mkdir -p "$TRABALHO/src"
cp "$ORIGEM"/*.h "$TRABALHO/src/"
cp "$ORIGEM/$NOME.ino" "$TRABALHO/src/main.ino"

printf '[env:esp32dev]\nplatform = espressif32\nboard = esp32dev\nframework = arduino\nlib_deps = file://%s\n' \
    "$IRREMOTE_DIR" > "$TRABALHO/platformio.ini"

cd "$TRABALHO"
"$PIO" run

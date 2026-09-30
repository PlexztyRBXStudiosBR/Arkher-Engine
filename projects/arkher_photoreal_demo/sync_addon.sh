#!/bin/sh
# Sincroniza o addon a partir do repo da engine.
# Use quando o addons/arkher_quality do repo mudar.
set -e
cd "$(dirname "$0")"
if [ ! -d ../../addons/arkher_quality ]; then
  echo "Falha: este projeto deve estar dentro do repo da engine (projects/arkher_photoreal_demo)." >&2
  exit 1
fi
cp -r ../../addons/arkher_quality/. addons/arkher_quality/
echo "Addon sincronizado a partir de addons/arkher_quality."

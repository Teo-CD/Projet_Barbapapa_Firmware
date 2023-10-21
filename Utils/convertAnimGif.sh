#!/bin/bash

set -euo pipefail

if [[ $# -eq 0 ]]; then
  echo "This script expects the name of the files to convert as arguments"
  exit 1
fi

if [[ ! $(command -v convert) ]]; then
  echo "Could not find 'convert' utility, cannot proceed."
  exit 1
fi

# Get the absolute path to the source of the script
scriptSrcDir="$(dirname -- "$0")"
pushd "$scriptSrcDir"
scriptSrcDir="$(pwd)"
popd

if [[ $(command -v bitmap_helper) ]]; then
  bmp_help="$(command -v bitmap_helper)"
elif [[ $(command -v "$scriptSrcDir"/bitmap_helper/target/debug/bitmap_helper) ]]; then
  bmp_help="$scriptSrcDir"/bitmap_helper/target/debug/bitmap_helper
elif [[ $(command -v "$scriptSrcDir"/bitmap_helper) ]]; then
  bmp_help="$scriptSrcDir"/bitmap_helper
else
  echo "Could not find 'bitmap_helper'."
  echo "Have you compiled it or placed it in the same directory as the script ?"
  exit 1
fi

for gifToConvert in "$@"; do
  if [[ -z "$(echo "$gifToConvert" | grep -e 'gif')" ]]; then
    echo "Cannot convert $gifToConvert : not a gif."
    continue
  fi

  animDir=ANIM_"${gifToConvert//.gif//}"
  mkdir "$animDir"
  pushd "$animDir"

  convert -resize 84x48 -coalesce ../"$gifToConvert" -threshold 50% %02d.mono
  "$bmp_help" ./*.mono
  # Clean up and remove the extension
  rm -f ./*.mono
  for f in ./*.bin; do
    mv "$f" "${f//.bin/}"
  done

  popd
done

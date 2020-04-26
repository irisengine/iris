#!/bin/bash

show_help()
{
    echo 'Usage: ./build_macos.sh --graphics_api <metal|opengl>'
    echo 'Where:'
    echo '    graphics_api: which graphics api to use (default is metal)'
}

graphics_api="metal"

while (( "$#" )); do
  case "$1" in
    --graphics_api)
      graphics_api=$2
      shift 2
      ;;
  esac

  case "$1" in
    -h|--help)
      show_help
      exit -1
      ;;
  esac
done

if [ -z "$graphics_api" ]; then
    show_help
    exit -1
fi

build_dir=build/macos/$graphics_api

mkdir -p $build_dir

root=$(pwd)

pushd $build_dir

api_upper=$(echo "$graphics_api" | tr a-z A-Z)

cmake $root \
    -DIRIS_PLATFORM=MACOS \
    -DIRIS_GRAPHICS_API="$api_upper" \

popd


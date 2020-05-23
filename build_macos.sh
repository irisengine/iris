#!/bin/bash

show_help()
{
    echo 'Usage: ./build_macos.sh --graphics_api <metal|opengl> --jobs_api <fibers|threads>'
    echo 'Where:'
    echo '    graphics_api: which graphics api to use (default is metal)'
    echo '    jobs_api: which jobs api to use (default is fibers)'
}

graphics_api="metal"
jobs_api="fibers"

while (( "$#" )); do
  case "$1" in
      --graphics_api)
      graphics_api=$2
      shift 2
      ;;

      --jobs_api)
      jobs_api=$2
      shift 2
      ;;

    -h|--help)
      show_help
      exit -1
      ;;

    *)
      echo "unknown argument: $1"
      show_help
      exit -1
      ;;
  esac
done

build_dir=build/macos/$graphics_api

mkdir -p $build_dir

root=$(pwd)

pushd $build_dir

graphics_api_upper=$(echo "$graphics_api" | tr a-z A-Z)
jobs_api_upper=$(echo "$jobs_api" | tr a-z A-Z)

cmake $root \
    -DIRIS_PLATFORM=MACOS \
    -DIRIS_GRAPHICS_API="$graphics_api_upper" \
    -DIRIS_JOBS_API="$jobs_api_upper" \

popd


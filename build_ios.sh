#!/bin/bash

show_help()
{
    echo 'Usage: ./build_ios.sh --bundle_identifier <value> --development_team <value>'
    echo 'Where:'
    echo '    bundle_identifier: unique string for identifying apps, will be prepended to sample names'
    echo '    development_team: unique id of ios development team' 
}

bundle_identifier=""
development_team=""

while (( "$#" )); do
  case "$1" in
    --bundle_identifier)
      bundle_identifier=$2
      shift 2
      ;;
  esac

  case "$1" in
    --development_team)
      development_team=$2
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

if [ -z "$bundle_identifier" ] ||
   [ -z "$development_team" ]; then
    show_help
    exit -1
fi

build_dir=build/ios/metal

mkdir -p $build_dir

root=$(pwd)

pushd $build_dir

cmake $root \
    -G Xcode \
    -DCMAKE_TOOLCHAIN_FILE=$root/toolchains/ios.toolchain.cmake \
    -DPLATFORM=OS64COMBINED \
    -DSDK_VERSION=13.3 \
    -DDEPLOYMENT_TARGET=13.3 \
    -DENABLE_ARC=1 \
    -DENABLE_VISIBILITY=1 \
    -DIRIS_PLATFORM=IOS \
    -DIRIS_BUNDLE_IDENTIFIER=$bundle_identifier \
    -DIRIS_DEVELOPMENT_TEAM=$development_team \

popd


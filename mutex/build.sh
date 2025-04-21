#!/bin/bash

git config --global credential.helper store
git config --global core.fileMode false

set -e

test -x /usr/bin/git || exit 0

usage() {
    echo "Don't directely exec this script, Please call make command with tab show all target"
}

pushd() {
    command pushd "$@" >/dev/null
}

# shellcheck disable=SC2120
popd() {
    command popd "$@" >/dev/null
}

project_dir=${PWD}

### --------------------------------------------------------------------
### Build
### --------------------------------------------------------------------
project_build() {
    # git config --global --add safe.directory /kdrm210_main
    # project_gen_common $1
    # for dir in $(find /kdrm210_main/modules/ -type d); do
    #     git config --global --add safe.directory "$dir"
    # done
    # echo "build with $1 platform"
    if [[ $1 == "arm" ]]; then
        cmake -S "${project_dir}" -DCMAKE_BUILD_TYPE=Debug \
            -B "${project_dir}"/build/$1 -G "Unix Makefiles" && cmake --build "${project_dir}"/build/$1 -j4
    fi
}

### --------------------------------------------------------------------
### Depends
### --------------------------------------------------------------------
project_depends() {
    sudo apt-get install -y libgpiod-dev libzmq3-dev libopencv-dev qtbase5-dev qttools5-dev-tools
}

### --------------------------------------------------------------------
### Run
### --------------------------------------------------------------------
project_run() {
    sudo mutex
}

### --------------------------------------------------------------------
### Install
### --------------------------------------------------------------------
project_install() {
    sudo cp -rf ./lib/3rd/arm/lib* /usr/lib
    sudo cp -rf ./lib/product/arm/lib* /usr/lib
    sudo cp -rf ./lib/product/arm/bin/mutex /usr/bin
}

### --------------------------------------------------------------------
### Scirpt params
### --------------------------------------------------------------------
case "$1" in
arm)
    project_build $1
    ;;
install)
    project_install
    ;;
run)
    project_run
    ;;
depends)
    project_depends
    ;;
*)
    usage
    exit 1
    ;;
esac

exit 0

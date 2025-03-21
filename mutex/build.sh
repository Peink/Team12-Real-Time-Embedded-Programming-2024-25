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
### Scirpt params
### --------------------------------------------------------------------
case "$1" in
arm)
    project_build $1
    ;;
*)
    usage
    exit 1
    ;;
esac

exit 0

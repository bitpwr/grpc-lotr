#!/usr/bin/env bash

set -eu

dir=deps

if [[ -d ${dir} ]]; then
    rm -r ${dir}
fi

mkdir ${dir}
conan install . --build missing --output-folder ${dir}

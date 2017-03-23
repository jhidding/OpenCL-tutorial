#!/bin/bash

root=$(pwd)

create_meson_file() {
    cc_files=$(find . -name '*.cc' -printf "'%p',")
    d=$(pwd)
    id=$(basename $d | sed -e "s/-/_/")
    echo "${id}_files = files(${cc_files})" > meson.build
}

cd src
echo -n "" > meson.build

folders=$(find . -mindepth 1 -maxdepth 1 -type d)
for d in ${folders}
do
    echo "subdir('${d}')" >> meson.build
    cd ${d}
    create_meson_file
    cd ..
done


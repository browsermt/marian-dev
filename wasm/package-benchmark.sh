#!/bin/bash

if [[ ! -e ../models ]]; then
    cd ..
    mkdir -p models
    if [[ ! -e students ]]; then
        echo "Cloning https://github.com/browsermt/students)"
        git clone --depth 1 --branch main --single-branch https://github.com/browsermt/students
    fi
    
    echo "Download files"
    cd students/esen/
    ./download-models.sh
    cd esen.student.tiny11

    echo "Copying files to models folder"
    cp vocab.esen* model* lex.s2t* ../../../models/
    sacrebleu -t wmt13 -l es-en --echo src > ../../../models/newstest2013.es
    cd ../../../
    head -n300 models/newstest2013.es > models/newstest2013.es.top300lines
    gunzip models/*
    cd build-wasm
else
    echo "models directory already exists in root folder; Using it to package files without downloading anything"
fi

echo "Packaging files for wasm binary"
$EMSDK_PYTHON $EMSDK/upstream/emscripten/tools/file_packager.py model-files.data --preload ../models/@ --js-output=model-files.js

echo "Enabling wormhole via APIs that compile and instantiate wasm module"
sed -i.bak 's/var result = WebAssembly.instantiateStreaming(response, info);/var result = WebAssembly.instantiateStreaming(response, info, {simdWormhole:true});/g' marian-decoder.js
sed -i.bak 's/return WebAssembly.instantiate(binary, info);/return WebAssembly.instantiate(binary, info, {simdWormhole:true});/g' marian-decoder.js
sed -i.bak 's/var module = new WebAssembly.Module(bytes);/var module = new WebAssembly.Module(bytes, {simdWormhole:true});/g' marian-decoder.js
echo "SUCCESS"

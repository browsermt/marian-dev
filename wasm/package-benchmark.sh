#!/bin/bash

if [[ ! -e ../models ]]; then
    mkdir -p ../models
    if [[ ! -e ../students ]]; then
        echo "Cloning https://github.com/browsermt/students)"
        git clone --depth 1 --branch main --single-branch https://github.com/browsermt/students ../
    fi
    
    echo "Downloading files"
    ../students/esen/download-models.sh

    echo "Copying downloaded files to models folder"
    cp ../students/esen/esen.student.tiny11/vocab.esen* ../students/esen/esen.student.tiny11/model* ../students/esen/esen.student.tiny11/lex.s2t* ../models/
    sacrebleu -t wmt13 -l es-en --echo src > ../models/newstest2013.es
    head -n300 ../models/newstest2013.es > ../models/newstest2013.es.top300lines
    gunzip ../models/*
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

echo "Import integer (8-bit) gemm from within the main module"
#sed -i.bak 's/asmLibraryArg,/asmLibraryArg ,"wasm_gemm":{"mmul_int8_shifted": (...a) => Module.asm.mmul_int8_shifted_ext(...a),"mmul_int8": (...a) => Module.asm.mmul_int8_ext(...a),"prepare_a_int8": (...a) => Module.asm.prepare_a_int8_ext(...a),"prepare_a_int8_shifted": (...a) => Module.asm.prepare_a_int8_shifted_ext(...a),"prepare_b_int8": (...a) => Module.asm.prepare_b_int8_ext(...a), "prepare_bias_int8_shifted": (...a) => Module.asm.prepare_bias_int8_shifted_ext(...a), "prepare_qt_int8": (...a) => Module.asm.prepare_qt_int8_ext(...a), },/g' marian-decoder.js
sed -i.bak 's/asmLibraryArg,/asmLibraryArg,"wasm_gemm":{"int8_prepare_a": (...a) => Module["asm"].int8_prepare_a_fallback(...a) },/g' marian-decoder.js
echo "SUCCESS"

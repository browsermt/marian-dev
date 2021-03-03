# -*- mode: makefile-gmake; indent-tabs-mode: true; tab-width: 4 -*-
SHELL   		= bash
PWD     		= $(shell pwd)
NATIVE_IMAGE    = local/marian-build-native
WASM_IMAGE	    = local/marian-build-wasm

all: native-image wasm-image

# Build the Docker image for native builds
native-image:
	docker build -t local/marian-build-native ./native/

# Build the Docker image for WASM builds
wasm-image:
	docker build -t local/marian-build-wasm ./wasm/

# Commands for compilation:
cmake_cmd  = cmake --debug-output -Wno-dev
cmake_cmd += -DUSE_STATIC_LIBS=on
cmake_cmd += -DUSE_SENTENCEPIECE=on
cmake_cmd += -DCOMPILE_CUDA=off
cmake_cmd += -DUSE_DOXYGEN=off
cmake_cmd += -DUSE_FBGEMM=off
cmake_cmd += -DCOMPILE_LIBRARY_ONLY=off
cmake_cmd += -DUSE_MKL=off
cmake_cmd += -DCOMPILE_CPU=on
cmake_cmd += -DCOMPILE_WITHOUT_EXCEPTIONS=on
cmake_cmd += -DUSE_WASM_COMPATIBLE_BLAS=on

native_decoder_cmake_cmd = ${cmake_cmd}
native_decoder_cmake_cmd += -DCOMPILE_DECODER_ONLY=on

wasm_cmake_cmd = ${native_decoder_cmake_cmd}
wasm_cmake_cmd += -DCOMPILE_WASM=on
wasm_cmake_cmd += -DWORMHOLE=on
#wasm_cmake_cmd += -DProtobuf_INCLUDE_DIR=/usr/opt/protobuf-wasm-lib/dist/include
#wasm_cmake_cmd += -DProtobuf_LIBRARY=/usr/opt/protobuf-wasm-lib/dist/lib/libprotobuf.a

wasm_without_pthreads_cmake_cmd = ${wasm_cmake_cmd}
wasm_without_pthreads_cmake_cmd += -DCOMPILE_WITH_PTHREADS=off

make_cmd  = make -j3
#make_cmd += VERBOSE=1

# ... and running things on Docker
docker_mounts  = ${PWD}/..:/repo
docker_mounts += ${HOME}/.ccache:/.ccache
run_on_docker  = docker run --rm
run_on_docker += $(addprefix -v, ${docker_mounts})
run_on_docker += ${INTERACTIVE_DOCKER_SESSION}

${HOME}/.ccache:
	mkdir -p $@

# Compile marian-decoder to WASM
compile-wasm: BUILD_DIR = /repo/build-wasm-docker
compile-wasm: ${HOME}/.ccache
	${run_on_docker} ${WASM_IMAGE} bash -c 'mkdir -p ${BUILD_DIR} && \
cd ${BUILD_DIR} && \
(emcmake ${wasm_cmake_cmd} .. && \
(emmake ${make_cmd}))'

# Prepare files to be used with WASM-compiled marian-decoder
package-files-wasm: BUILD_DIR = /repo/build-wasm-docker
package-files-wasm: MODELS_DIR = /repo/models
package-files-wasm:
	${run_on_docker} ${WASM_IMAGE} bash -c 'sacrebleu -t wmt13 -l es-en --echo src > /repo/models/newstest2013.es && \
head -n300 /repo/models/newstest2013.es > /repo/models/newstest2013.es.top300lines && \
python3 /emsdk/upstream/emscripten/tools/file_packager.py ${BUILD_DIR}/model-files.data --preload ${MODELS_DIR} --js-output=${BUILD_DIR}/model-files.js'

# Run WASM-compiled marian-decoder
run-wasm: BUILD_DIR = /repo/build-wasm-docker
run-wasm:
	${run_on_docker} -p 8000:8000 ${WASM_IMAGE} bash -c 'emrun --no_browser --port 8000 ${BUILD_DIR}'


# Remove the marian-decoder WASM-without-pthreads build dir, forcing a clean compilation attempt
clean-wasm-without-pthreads: BUILD_DIR = /repo/build-wasm-without-pthreads-docker
clean-wasm-without-pthreads: ${HOME}/.ccache
	${run_on_docker} ${WASM_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true)'

# Compile marian-decoder to WASM-without-pthreads
compile-wasm-without-pthreads: BUILD_DIR = /repo/build-wasm-without-pthreads-docker
compile-wasm-without-pthreads: ${HOME}/.ccache
	${run_on_docker} ${WASM_IMAGE} bash -c 'mkdir -p ${BUILD_DIR} && \
cd ${BUILD_DIR} && \
(emcmake ${wasm_without_pthreads_cmake_cmd} .. && \
(emmake ${make_cmd}))'

# Prepare files to be used with WASM-without-pthreads-compiled marian-decoder
package-files-wasm-without-pthreads: BUILD_DIR = /repo/build-wasm-without-pthreads-docker
package-files-wasm-without-pthreads: MODELS_DIR = /repo/models
package-files-wasm-without-pthreads:
	${run_on_docker} ${WASM_IMAGE} bash -c 'sacrebleu -t wmt13 -l es-en --echo src > /repo/models/newstest2013.es && \
head -n10 /repo/models/newstest2013.es > /repo/models/newstest2013.es.top10lines && \
python3 /emsdk/upstream/emscripten/tools/file_packager.py ${BUILD_DIR}/model-files.data --preload ${MODELS_DIR} --js-output=${BUILD_DIR}/model-files.js'

# Run WASM-without-pthreads-compiled marian-decoder
run-wasm-without-pthreads: BUILD_DIR = /repo/build-wasm-without-pthreads-docker
run-wasm-without-pthreads:
	${run_on_docker} -p 8000:8000 ${WASM_IMAGE} bash -c 'emrun --no_browser --port 8000 ${BUILD_DIR}'


# # Compile full native version
# compile-native: BUILD_DIR = /repo/build-native-docker
# compile-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true) && \
# mkdir -p ${BUILD_DIR} &&\
# cd ${BUILD_DIR} &&\
# (${native_cmake_cmd} .. &&\
# ${make_cmd} || rm CMakeCache.txt)'

# # Echo the command to compile the native version of marian-decoder with a wasm-incompatible blas
# echo-compile-decoder-only-native: BUILD_DIR = ./build-decoder-only-native
# echo-compile-decoder-only-native: ${HOME}/.ccache
# 	echo 'mkdir -p ${BUILD_DIR} && \
# cd ${BUILD_DIR} && \
# (${decoder_only_native_cmake_cmd} .. && \
# ${make_cmd} || rm CMakeCache.txt)'

# echo-benchmark-decoder-only-native: BUILD_DIR = ./build-decoder-only-native
# echo-benchmark-decoder-only-native: MODEL_DIR = ./docker/students/esen/esen.student.tiny11
# echo-benchmark-decoder-only-native: ${HOME}/.ccache
# 	echo 'mkdir -p ./docker/students/marian-dev/ && \
# (rm ./docker/students/marian-dev/build || true) && \
# ln -s "$$PWD/${BUILD_DIR}" ./docker/students/marian-dev/build && \
# cd ${MODEL_DIR}/ && \
# ./speed.cpu.sh; cd -; \
# (rm ./docker/students/marian-dev/build || true)'

# # Echo the command to compile the native version of marian-decoder with a wasm-compatible blas
# echo-compile-wasm-compatible-decoder-only-native: BUILD_DIR = ./build-wasm-compatible-decoder-only-native
# echo-compile-wasm-compatible-decoder-only-native: ${HOME}/.ccache
# 	echo 'mkdir -p ${BUILD_DIR} && \
# cd ${BUILD_DIR} && \
# (${wasm_compatible_decoder_only_native_cmake_cmd} .. && \
# ${make_cmd} || rm CMakeCache.txt)'

# echo-benchmark-wasm-compatible-decoder-only-native: BUILD_DIR = ./build-wasm-compatible-decoder-only-native
# echo-benchmark-wasm-compatible-decoder-only-native: MODEL_DIR = ./docker/students/esen/esen.student.tiny11
# echo-benchmark-wasm-compatible-decoder-only-native: ${HOME}/.ccache
# 	echo 'mkdir -p ./docker/students/marian-dev/ && \
# (rm ./docker/students/marian-dev/build || true) && \
# ln -s "$$PWD/${BUILD_DIR}" ./docker/students/marian-dev/build && \
# cd ${MODEL_DIR}/ && \
# ./speed.cpu.sh; cd -; \
# (rm ./docker/students/marian-dev/build || true)'

# # Remove the build directory of native marian-decoder, forcing a clean compilation attempt
# clean-decoder-only-native: BUILD_DIR = /repo/build-decoder-only-native-docker
# clean-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true)'

# # Compile native version of marian-decoder with a wasm-incompatible blas
# compile-decoder-only-native: BUILD_DIR = /repo/build-decoder-only-native-docker
# compile-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c 'mkdir -p ${BUILD_DIR} &&\
# cd ${BUILD_DIR} &&\
# (${decoder_only_native_cmake_cmd} .. &&\
# ${make_cmd} || rm CMakeCache.txt)'

# benchmark-decoder-only-native: BUILD_DIR = /repo/build-decoder-only-native-docker
# benchmark-decoder-only-native: MODEL_DIR = /repo/docker/students/esen/esen.student.tiny11
# benchmark-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c 'mkdir -p /repo/docker/students/marian-dev/ && \
# (rm /repo/docker/students/marian-dev/build || true) && \
# ln -s ${BUILD_DIR} /repo/docker/students/marian-dev/build && \
# cd ${MODEL_DIR}/ && \
# ./speed.cpu.sh; \
# (rm /repo/docker/students/marian-dev/build || true)'

# # Remove the build directory of wasm-compatible native marian-decoder, forcing a clean compilation attempt
# clean-wasm-compatible-decoder-only-native: BUILD_DIR = /repo/build-wasm-compatible-decoder-only-native-docker
# clean-wasm-compatible-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true)'

# # Compile native version of marian-decoder with a wasm-compatible blas
# compile-wasm-compatible-decoder-only-native: BUILD_DIR = /repo/build-wasm-compatible-decoder-only-native-docker
# compile-wasm-compatible-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c 'mkdir -p ${BUILD_DIR} &&\
# cd ${BUILD_DIR} &&\
# (${wasm_compatible_decoder_only_native_cmake_cmd} .. &&\
# ${make_cmd})'

# benchmark-wasm-compatible-decoder-only-native: BUILD_DIR = /repo/build-wasm-compatible-decoder-only-native-docker
# benchmark-wasm-compatible-decoder-only-native: MODEL_DIR = /repo/docker/students/esen/esen.student.tiny11
# benchmark-wasm-compatible-decoder-only-native: ${HOME}/.ccache
# 	${run_on_docker} ${NATIVE_IMAGE} bash -c 'mkdir -p /repo/docker/students/marian-dev/ && \
# (rm /repo/docker/students/marian-dev/build || true) && \
# ln -s ${BUILD_DIR} /repo/docker/students/marian-dev/build && \
# cd ${MODEL_DIR}/ && \
# ./speed.cpu.sh; \
# (rm /repo/docker/students/marian-dev/build || true)'

# # Remove the marian-decoder WASM build dir, forcing a clean compilation attempt
# clean-wasm: BUILD_DIR = /repo/build-wasm-docker
# clean-wasm: ${HOME}/.ccache
# 	${run_on_docker} ${WASM_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true)'

# # Compile sentencepiece to WASM
# sentencepiece_wasm_cmake_cmd = cmake --debug-output -Wno-dev
# sentencepiece_wasm_cmake_cmd += -DCOMPILE_WASM=on
# sentencepiece_wasm_cmake_cmd += -DSPM_ENABLE_SHARED=off
# sentencepiece_wasm_cmake_cmd += -DProtobuf_INCLUDE_DIR=/usr/opt/protobuf-wasm-lib/dist/include
# sentencepiece_wasm_cmake_cmd += -DProtobuf_LIBRARY=/usr/opt/protobuf-wasm-lib/dist/lib/libprotobuf.a

# compile-sentencepiece-wasm: BUILD_DIR = /repo/src/3rd_party/sentencepiece/build-wasm-docker
# compile-sentencepiece-wasm: ${HOME}/.ccache
# 	${run_on_docker} ${WASM_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true) && \
# mkdir -p ${BUILD_DIR} && \
# cd ${BUILD_DIR} && \
# (emcmake ${sentencepiece_wasm_cmake_cmd} .. && \
# (emmake ${make_cmd}) || \
# rm CMakeCache.txt)'

# # Compile stdin test directly
# compile_test_stdin_cmd  = em++
# compile_test_stdin_cmd += -O2 -s WASM=1 -s ASSERTIONS=1 -s DISABLE_EXCEPTION_CATCHING=0
# compile_test_stdin_cmd += -s FORCE_FILESYSTEM=1
# compile_test_stdin_cmd += -s ALLOW_MEMORY_GROWTH=1
# compile_test_stdin_cmd += -o test-stdin-wasm.html
# compile_test_stdin_cmd += --pre-js /repo/wasm/pre-module.js --post-js /repo/wasm/post-module.js --shell-file /repo/wasm/custom_shell.html
# compile_test_stdin_cmd += /repo/wasm/test_stdin.cpp

# compile-and-run-test-stdin-wasm: BUILD_DIR = /repo/build-test-stdin-wasm
# compile-and-run-test-stdin-wasm:
# 	${run_on_docker} ${WASM_IMAGE} bash -c '(rm -rf ${BUILD_DIR} || true) && \
# mkdir -p ${BUILD_DIR} &&\
# cd ${BUILD_DIR} &&\
# ${compile_test_stdin_cmd} &&\
# ${run_on_docker} -p 8009:8009 ${WASM_IMAGE} bash -c 'emrun --no_browser --port 8009 ${BUILD_DIR}'

# # Start interactive shells for development / debugging purposes
# native-shell: INTERACTIVE_DOCKER_SESSION = -it
# native-shell:
# 	${run_on_docker} ${NATIVE_IMAGE} bash

# wasm-shell: INTERACTIVE_DOCKER_SESSION = -it
# wasm-shell:
# 	${run_on_docker} ${WASM_IMAGE} bash
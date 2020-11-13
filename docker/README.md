## Native (via Docker)

Prepare docker image for Native compilation:

```bash
make native-image
```

Compile natively (Note: currently failing):

```bash
make compile-native
```

Enter a docker container shell for manually running commands:

```bash
make native-shell
```

## WASM

Prepare docker image for WASM compilation:

```bash
make wasm-image
```

Compile to wasm:

```bash
make compile-wasm
```

Compile only sentencepiece to wasm:

```bash
make compile-sentencepiece-wasm
```

Enter a docker container shell for manually running commands:

```bash
make wasm-shell
```

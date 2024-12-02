// TypeScript bindings for emscripten-generated code.  Automatically generated at compile time.
interface WasmModule {
  __emscripten_tls_init(): number;
  _pthread_self(): number;
  __embind_initialize_bindings(): void;
  __emscripten_thread_init(_0: number, _1: number, _2: number, _3: number, _4: number, _5: number): void;
  __emscripten_thread_crashed(): void;
  __emscripten_thread_exit(_0: number): void;
}

interface EmbindModule {
  read_file(_0: ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string): string;
  list_files(_0: ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string): void;
  setup_fs(_0: ArrayBuffer|Uint8Array|Uint8ClampedArray|Int8Array|string): void;
}
export type MainModule = WasmModule & EmbindModule;

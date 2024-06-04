#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/wasmfs.h>
#include <stdio.h>
#include <unistd.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

void run_async(std::string id, void (*fn)()) {
  std::thread([=] {
    fn();
    // clang-format off
    MAIN_THREAD_EM_ASM(
        {
          const event = new CustomEvent(UTF8ToString($0));
          dispatchEvent(event);
        },
        id.c_str());
    // clang-format on
  }).detach();
}

std::string buffer_to_string(char* buffer, size_t buffer_size) {
  return std::string(buffer, buffer_size);
}

auto read_file(std::string path) {
  const auto handle = fopen(path.c_str(), "rb");

  if (!handle) {
    return std::string("Unable to establish file handle.");
  }

  fseek(handle, 0, SEEK_END);
  size_t length = ftell(handle);
  fseek(handle, 0, SEEK_SET);

  auto buffer = malloc(length);
  fread(buffer, sizeof(char), length, handle);

  return buffer_to_string(reinterpret_cast<char*>(buffer), length);
}

auto list_files(std::string path) {
  printf("Listing files in directory..\n");
  for (const auto& entry : std::filesystem::directory_iterator(path)) {
    printf("%s\n", entry.path().c_str());
  }
}

void setup_fs(std::string id) {
  run_async(id, [] {
    // This does not actually create a directory in OPFS.
    // What this does is create a virtual directory in WASM and any operations to or from that directory in WASM are
    // associated with the assigned backend. Using wasmfs_create_directory, you could create a directory
    // structure that looked like:
    // -- /opfs
    // -- /idbfs
    // -- /memfs
    // and associate each directory with a particular backend. Then any writes from WASM-land that write to /opfs
    // would write to OPFS, any writes to /memfs would write to memory, and any writes to /idbfs would write to
    // IndexedDB. This is pretty confusing cause any write to /opfs/somewhere from WASM would actually be written
    // to /somewhere in OPFS. If we wanted to write a file from JavaScript and have it be available to
    // /persistent/my-file.txt, we would actually write to /my-file.txt.

    auto opfs = wasmfs_create_opfs_backend();
    wasmfs_create_directory("opfs", 0777, opfs);
  });
}

EMSCRIPTEN_BINDINGS(module) {
  emscripten::function("read_file", &read_file, emscripten::allow_raw_pointers());
  emscripten::function("list_files", &list_files);
  emscripten::function("setup_fs", &setup_fs);
}
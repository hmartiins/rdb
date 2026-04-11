# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Requires CMake, a C++17-capable compiler, and [nlohmann/json](https://github.com/nlohmann/json) installed (e.g. via Homebrew: `brew install nlohmann-json`).

```bash
# Configure (from repo root)
cmake -B build .

# Build both server and client
cmake --build build

# Or use the generated Makefile directly
make
```

Binaries land in `build/out/` (server and client).

## Running

Server and client must run in separate terminals. Default port is `3333`; CMake run targets use `3332`.

```bash
# Start server
./build/out/server --port 3333

# Connect client
./build/out/client --port 3333

# Or via Make targets (uses port 3332)
make run-server
make run-client
```

## Architecture

This is a **multi-client JSON "database" over raw TCP sockets**. The server stores JSON data in flat files, and clients issue text commands to create, read, and update them.

### Wire protocol (`src/protocol.hpp`)

All messages use a length-prefixed framing: `[4-byte big-endian length][JSON payload]`. Both `sendMessage` and `recvMessage` enforce this, with a 10 MB max message size. The JSON payload contains an `op` field (`CREATE`, `READ`, `UPDATE`), a `filename`, and optionally `data` and `id`.

### Server (`src/server.cpp`)

- Starts a `ThreadPool` with 10 workers.
- Accepts connections in a loop; each client is dispatched to the pool via `pool.enqueue(...)`.
- Per-client loop: parse JSON request → dispatch to `create_json_file`, `read_json_file`, or `update_json_file` → send JSON response.
- On `READ`, the client joins a **room** (keyed by filename). On `UPDATE`, all other clients in that room are notified with the updated file contents.

### RoomManager (`src/room_manager.hpp`)

Maintains a `map<string, set<int>>` of filename → connected client sockets. Provides `joinRoom`, `leaveAllRooms`, and `notifyOthers`. Protected by a single `std::mutex`.

### ThreadPool (`src/thread_pool.hpp`)

Standard fixed-size thread pool: worker threads block on a `condition_variable`, dequeue `std::function<void()>` tasks, and execute them. Graceful shutdown on destructor.

### JSON operations (`src/json/`)

Header-only implementations for `CREATE`, `READ`, and `UPDATE` file operations. `create-file.hpp` uses `src/utils/random-id.hpp` to assign IDs to new records.

### Client (`src/client.cpp`)

Spawns a receive thread (`receiveServerMessages`) for async server push notifications while the main thread runs `sendMessagesToServer`, which reads stdin lines and serializes them to JSON requests.

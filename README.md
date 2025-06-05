# 📡 C++ Client-Server Room Management System 📡

## 👀 About

A lightweight and efficient C++ application to manage "rooms" and communication over sockets. It enables JSON-based file manipulation, dynamic ID creation, and provides a structured environment for networking and client-server interactions.

---

## 🚀 Technologies

C++

Sockets (low-level networking)

JSON file operations

CMake

Make

Linux development environment

---

## 💻 Getting started

### Requirements

C++ compiler (e.g., g++ or clang++)

CMake

Make

Unix-based system (Linux or macOS)

### Installing and running the project

Clone the repository:

```bash
$ git clone git@github.com:your-user/rdb-main.git
$ cd rdb-main
```

Build the project using cmake and make:

```bash
$ cmake .
$ make
```

Run the server and client in separate terminals:

#In one terminal

```bash
$ ./server
```
#In another terminal

```bash
$ ./client
```

---

### 📁 Project Structure

```
.
├── CMakeLists.txt         # CMake build config
├── Makefile               # Alternative build option
├── src/
│   ├── client.cpp         # Client application
│   ├── server.cpp         # Server application
│   ├── room_manager.hpp   # Room control logic
│   ├── socket_utils.hpp   # Socket helper functions
│   ├── json/
│   │   ├── create-file.hpp
│   │   ├── read-file.hpp
│   │   └── update-file.hpp
│   └── utils/
│       ├── random-id.hpp  # ID generation helper
│       └── split.hpp      # String splitting utilities
```

---

## 🛠 Contribution

A big thank you to everyone contributing to the project. Below are some of our main contributors:

- [Henrique Martins](https://github.com/hmartiins)
- [João Victor](https://github.com/joaovds)
- [Gabriel Carvalho](https://github.com/GabrielQuinteiro)
- [Leticia Silverio](https://github.com/devLeSilverio)
- [Vitor Cavicholli](https://github.com/Vitorcavic)
- [Rennys Cardoso](https://github.com/yrsenn)


## Presentation

LCSF Generator is a C++/Qt graphical tool used to create, edit and deploy LCSF protocols.

It generates code for [LCSF C stack](https://github.com/jean-roland/LCSF_C_Stack) and [LCSF Stack Rust](https://github.com/jean-roland/LCSF_Stack_Rust), as well as documentation (markdown and wiki format).

There is also a CLI variant to help automate things or if you can't be bothered with a GUI, see below for the arguments documentation.

## About LCSF

LCSF (Light Command Set Format) is a specification to easily create and deploy custom command sets.

For more information, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

## How to install

### Windows
* Note: release 1.4 ships Linux binaries only. For Windows, use an earlier release or build from source (see below).
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Windows version.
* Unzip the archive and execute `LCSF_Generator.exe` (GUI) or `LCSF_Generator_CLI.exe + args` (CLI).

### Linux
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Linux version.
* Unzip the archive and execute `lcsf_generator_gui.sh` or `lcsf_generator_cli.sh + args`
* Requires the Qt5 runtime. On Debian/Ubuntu: `sudo apt install libqt5widgets5`.
* There might be other missing libraries, in that case you should use the `ldd` command on the executables to find the missing packages.

### Other OS
* No other OS are currently supported.

## How to use

### GUI
Refer to the project user manual found [here](https://jean-roland.github.io/LCSF_Generator/).

### CLI
The CLI is used to generate C code directly from a protocol description json file.
It can be generated beforehand through the GUI or you can make one based on the one found in `example/`.
Like the GUI, you can import previous C code if you're updating your protocol.

Options:
* `-h, --help` Print a help message.
* `-v, --version` Displays version information.
* `-l, --load <path/to/file>` Load a protocol description file (REQUIRED)
* `-a, --import-a <path/to/file>` Import specific protocol code, A point of view
* `-b, --import-b <path/to/file>` Import specific protocol code, B point of view

Examples:
* For a raw generation: `./LCSG_Generator_CLI -l Description/Test.json`
* For a generation with code imported for the A point of view: `./LCSG_Generator_CLI -l Description/Test.json -a OutputA/Test_Main_a.c`

### Struct types
Structs give a byte array attribute an explicit layout, they are defined once and reusable across commands. In the GUI they are edited under the `Structs` tree node and picked as `struct <name>` or `struct <name>[]` in the attribute data type. In the json (see `tests/data/model_struct_desc.json`):
```json
"structs": [{"name": "link", "desc": "", "isExtensible": false, "fields": [
    {"name": "sent", "dataType": "LCSF_UINT32", "desc": ""},
    {"name": "lost", "dataType": "LCSF_UINT32", "desc": ""}]}],
...
{"name": "links", "id": 1, "dataType": "LCSF_STRUCT", "structName": "link", "isArray": true, ...}
```
* On the wire a struct attribute is a byte array: fields packed in order, little endian. Field types: `LCSF_(U)INT8` to `LCSF_(U)INT64`, `LCSF_FLOAT32`, `LCSF_FLOAT64`, and the variable-length `LCSF_VARUINT16/32/64`, `LCSF_VARINT16/32/64`.
* Variable-length fields are unsigned LEB128: 7 bits per byte from the least significant, MSb set when more bytes follow. Signed ones are zigzag encoded first (0, -1, 1, -2 give 0, 1, 2, 3). A value over its field type or over 10 bytes ends the decode like a missing field. A struct with variable-length fields must be extensible, its array elements each start with their size on one byte, and its generated size is a maximum.
* Fields can only be appended: a shorter payload reads missing fields as 0, a longer one ignores the extra bytes.
* `isArray` sends structs back to back. An `isExtensible` struct prefixes its arrays with the element size on one byte so its fields can still be appended.
* An attribute payload is at most `LCSF_ATT_PAYLOAD_MAX_SIZE` bytes, 255 with `LCSF_SMALL` and 65535 otherwise, the C stack refuses bigger ones. The generated bridge asserts each struct fits, array lengths are checked by the application with `<PROT>_<STRUCT>_ARRAY_SIZE(nb)`.
* An empty array can't be sent since the stacks reject zero-size byte arrays. Make the attribute optional and leave it out, or make the struct extensible.
* Generated helpers: `LCSF_Bridge_<Prot><struct>Serialize/Deserialize/ArraySerialize/ArrayDeserialize` in C, `serialize/deserialize/serialize_array/deserialize_array` in Rust. The C bridge handles single structs, struct arrays are left to the application.

## How to build

If for some reasons you want or need to build the application yourself, you will need to have Qt5.14 or higher installed on your machine.

Next, you have two options:
* Use the Qt Creator IDE, the project file is found at `src/LCSF_Generator.pro`, and compile from here.
* Use CMake 3.14 or higher (indicate your Qt directories in the `CMakeLists.txt` in the project root directory if needed).

If you're new with CMake, here's my process:
* Install [Ninja](https://ninja-build.org/)
* On Debian/Ubuntu, the build dependencies are: `sudo apt install qtbase5-dev cmake ninja-build`.
* Run the command `cmake -B build/ -G Ninja -DCMAKE_BUILD_TYPE="Release"` at the project root directory.
* `ninja -C build/`

Everything should now be compiling without issues.

## How to install from source

After building, you can install the GUI and CLI executables (along with the shared library they depend on) using CMake's install target.

To install to a custom location:
* `cmake --install build/ --prefix /your/install/path`

To install to the system default (e.g. `/usr/local` on Linux), pass the prefix at configure time or omit `--prefix`:
* `cmake --install build/`

The install layout is:
* `<prefix>/bin/lcsf_generator_gui` and `<prefix>/bin/lcsf_generator_cli`
* `<prefix>/lib/liblcsf_generator_lib.so`

The executables are installed with an rpath of `$ORIGIN/../lib`, so they will locate the shared library without needing `LD_LIBRARY_PATH`.

## How to run tests

To run unit-testing, you'll need [googletest](https://github.com/google/googletest) and (re)build the project using CMake.

After that, from root directory, you can run:
* `cmake --build build/ --target run_tests`

To run cross-testing between LCSF Generator CLI generated code and the unit tests of LCSF C Stack, you need to specify the location of root LCSF C Stack directory in the main CMakeLists:
* `set(LCSF_C_STACK_ROOT_DIR "path/to/lcsf_c_stack")`
Now from root directory, you can run:
* `cmake --build build/ --target cross_testing`


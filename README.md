## Presentation

LCSF Generator is a C++/Qt graphical tool used to create, edit and deploy LCSF protocols.

It generates code for the [LCSF C stack](https://github.com/jean-roland/LCSF_C_Stack) as well as documentation (markdown and wiki format).

There is also a CLI variant to help automate things or if you can't be bothered with a GUI, see below for the arguments documentation.

## About LCSF

LCSF (Light Command Set Format) is a specification to easily create and deploy custom command sets.

For more information, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

## How to install

### Windows
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Windows version.
* Uncompress the archive and execute `LCSF_Generator.exe` (GUI) or `LCSF_Generator_CLI.exe + args` (CLI).

### Linux
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Linux version.
* Uncompress the archive and execute `LCSF_Generator.sh`(GUI) or `LCSF_Generator_CLI.sh + args` (CLI).
* There might be missing libraries, in that case you should use the `ldd` command on `LCSF_Generator` and `platforms/libqxcb.so` and install the missing packages.

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
* For a generation with code imported for the A point of view: `./LCSG_Generator_CLI -l Description/Test.json -a OutputA/Test_Main.c`

## How to build

If for some reasons you want or need to build the application yourself, you will need to have Qt5.10 or higher installed on your machine.

Next, you have two options:
* Use the Qt Creator IDE, the project file is found at `src/LCSF_Generator.pro`, and compile from here.
* Use CMake 3.10 or higher (indicate your Qt directories in the `CMakeLists.txt` in the project root directory if needed).

If you're new with CMake, here's my process:
* Install [Ninja](https://ninja-build.org/)
* Run the command `cmake -B build/ -G Ninja` at the project root directory.
* `cd build/`
* `ninja`

Everything should now be compiling without issues.

## How to run tests

To test, you'll need [googletest](https://github.com/google/googletest) and (re)build the project using CMake.

After that:
* Go to build/
* Run `./tests/LCSF_Generator_lib_tests`


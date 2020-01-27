## Presentation

LCSF Generator is a C++/Qt graphical tool used to create, edit and deploy LCSF protocols.

It generates code for the [LCSF C stack](https://github.com/jean-roland/LCSF_C_Stack) as well as documentation (markdown and wiki format).

## About LCSF

LCSF (Light Command Set Format) is a specification to easily create and deploy custom command sets.

For more information, check the official LCSF documentation [here](https://jean-roland.github.io/LCSF_Doc/).

## How to install

### Windows
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Windows version.
* Uncompress the archive and execute `LCSF_Generator.exe`.

### Linux
* Go to the [release section](https://github.com/jean-roland/LCSF_Generator/releases) and download the latest Linux version.
* Uncompress the archive and execute `LCSF_Generator.sh`.
* There might be missing libraries, in that case you should use the `ldd` command on `LCSF_Generator` and `platforms/libqxcb.so` and install the missing packages.

### Other OS
* No other OS are currently supported.

## How to use

Refer to the project user manual found [here](https://jean-roland.github.io/LCSF_Generator/).

## How to build

If for some reasons you want or need to build the application yourself you have two options:
* Use CMake (3.10 or higher) in the project root directory after indicating your Qt directories in `CMakeLists.txt` also at the project root directory.
* Use the Qt Creator IDE, the project file is found at `src/LCSF_Generator.pro`
For both, you will need to have Qt5.10 or higher installed on your machine.

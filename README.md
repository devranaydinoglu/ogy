# Ogy

Ogy is a CLI tool for Unix and Linux systems which offers new and improved, existing commands with the purpose of making the command line experience more pleasant and convenient.

- [Getting Started](#getting-started)
- [Available Commands](#available-commands)
    - [Help](#help)
    - [Info](#info)
    - [List](#list)
    - [Find](#find)
    - [Change Directory](#change-directory)

https://github.com/user-attachments/assets/b2257a9e-030c-43c5-9500-eb3fef720306

## Getting Started

#### 1. Ensure CMake is installed

#### 2. Run `install.sh`

This script will:
1. Create a new directory in `/home/.local/bin` for the executable, config file, and `init.sh` script.\
It uses the `install_dir` variable in the `install.sh` script. This can be changed to a desired directory.
2. Build the project with CMake.
3. Add the built executable to the install directory from step 1.

#### 3. Terminal Setup

- Add Ogy's bin directory (directory containing the executable) to your PATH variable if necessary. 
- Add `source your_install_dir/init.sh` (e.g. `source $HOME/.local/bin/ogy/init.sh`) to your terminal's config file.

## Available Commands

### Help

Get info about Ogy and view a summary of the available commands.

```
$ ogy help
```

### Info

Show info about the specified file.

```
$ ogy info {file name} -rec
```
- Arguments
    - {file name} - name of the file or directory (should include the file extension and is not case-sensitive).
- Flags
    - -rec - recursively iterate through all subdirectories of the specified directory to get its total size.

### List

List info about the files and directories in the current directory.

```
$ ogy ls -all -rec -mt
```
- Flags
    - -all - include hidden files.
    - -rec - recursively iterate through all subdirectories of a directory to get its total size.
    - -mt - enable multithreading.

### Find

Find file(s) in the current directory containing the specified term.

```
$ ogy find {term} -rec
```
- Arguments
    - {term} - term to search for in file names (does not need to include the extensions and is not case-sensitive).
- Flags
    - -rec - recursively search for files containing the specified term in subdirectories.

### Change Directory

Change directory to an alias' corresponding path. If the alias exists, go to its corresponding path. Otherwise store the path as the alias in the config file and go to the specified path. (Alias is optional, so it could also be used as the built-in cd command)

```
$ ogy cd {alias} {path}
```

- Arguments
    - {alias} - alias that was used to store the path (optioal).
    - {path} - path to the directory from the current directory.

__Note__: The alias in `ogy cd {alias} {path}` cannot contain any forward slashes.

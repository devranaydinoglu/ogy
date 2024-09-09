#! /bin/bash

# create ogy directory for binaries and config file
create_dir() {
    if [[ "$OSTYPE" == "linux"* ]]; then
        # linux - usr/bin/ogy
        # linux - usr/local/bin/ogy
        echo "OS detected: Linux"
        sudo mkdir -p "$1/bin"
        sudo chmod 777 "$1/bin"
        sudo chmod 777 $1
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macos/linux - usr/local/ogy
        echo "OS detected: MacOS"
        sudo mkdir -p "$1/bin"
        sudo chmod 777 "$1/bin"
        sudo chmod 777 $1
    else
        echo "Unsupported OS."
        exit 1
    fi
}

# add config.json file to ogy install dir with default template
add_config_file() {
    echo "{
    \"paths\": [
        {\"\": \"\"}
    ]
}" > "$1/config.json" || err 

    if [ -f "$1/config.json" ]; then
        echo "Config file created."
    else
        echo "Unable to create config file."
    fi
}

# add init.sh script to ogy install dir
add_init_script() {
    cp "$2/scripts/init.sh" $1 || err

    if [ -f "$1/init.sh" ]; then
        sudo chmod 777 "$1/init.sh"
        echo "Script init.sh added."
    else
        echo "Unable to add init script file."
    fi
}

cmake_build() {
    if [ ! -d "$2/build" ]; then
        mkdir "$2/build"
    fi
    
    cmake -DCMAKE_BUILD_TYPE=Release -S $2 -B "$2/build"
    cmake --build "$2/build"

    if [ -f "$2/build/ogy" ]; then
        echo "Executable built."

        cp "$2/build/ogy" "$1/bin"
        if [ -f "$2/build/ogy" ]; then
            echo "Executable added to install directory."
        else
            echo "Failed to add executable to install directory."
        fi
    else
        echo "Failed to build executable."
    fi
}

main() {
    local install_dir="$HOME/.local/bin/ogy" # change the install directory here

    create_dir "$install_dir"
    add_config_file "$install_dir"

    local source_dir=$(dirname -- "$(readlink -f -- "$0";)";) # ogy source directory containing CMakeLists.txt
    source_dir=${source_dir%/*}

    add_init_script "$install_dir" "$source_dir"
    cmake_build "$install_dir" "$source_dir"

    echo -e "\nREMINDER: add ogy's bin directory to your PATH variable and add 'source path/to/init.sh' to your shell config file"
}

main
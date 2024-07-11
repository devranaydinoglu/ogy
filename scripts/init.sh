ogy() {
    if [ "$1" = cd ]; then
        out=$($HOME/.local/bin/ogy/bin/ogy "$@")
        if [[ "$out" == *"/"* ]]; then  # if a forward slash is included, it is most likely a path which can be cd'ed to
            builtin cd "$out"
        else    # otherwise it's an error which should be printed
            echo "$out"
        fi
    else
        out=$($HOME/.local/bin/ogy/bin/ogy "$@")
        echo "$out"
    fi
}
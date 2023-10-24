ogy() {
    if [ "$1" = cd ]; then
        out=$($HOME/.local/bin/ogy/bin/ogy "$@")
        if [[ "$out" == *"/"* ]]; then
            builtin cd "$out"
        else
            echo "$out"
        fi
    else
        out=$($HOME/.local/bin/ogy/bin/ogy "$@")
        echo "$out"
    fi
}
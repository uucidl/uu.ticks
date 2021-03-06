#!/usr/bin/env sh
SCRIPTS="$(dirname "${0}")"
TOP="${SCRIPTS}"/..

TOP="${SCRIPTS}"/..

while [ "$#" -gt 0 ]; do
    case $1 in
        --top-dir)
            TOP=${2:?"source directory expected"}
            if [ ! -d "${TOP}" ]; then
                printf -- "top dir expected in '%s'\n" "${TOP}"
                exit 1
            fi

            shift
            shift
            ;;
        *)
            break
            ;;
    esac
done

BUILD="${TOP}"/builds
[ -d "${BUILD}" ] || mkdir -p "${BUILD}"

function lint () {
    git --no-pager grep GL_VIEWPORT -- *.cpp *.c && printf "don't use GL_VIEWPORT\n" && return 1
    return 0
}

"${TOP}"/modules/uu.micros/build --src-dir "${TOP}"/src --output-dir "${BUILD}" "$@"
lint || exit 1

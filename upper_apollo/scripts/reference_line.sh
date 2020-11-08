#!/usr/bin/env bash

REFERENCE_LINE_FILE_PATH="/apollo/modules/tools/navigator/line.bag.txt.smoothed"

PREFIX="python /apollo/modules/tools/navigator/navigator.py"

CMD="${PREFIX} ${REFERENCE_LINE_FILE_PATH}"

function start() {
    NUM_PROCESSES="$(pgrep -c -f "${PREFIX}")"
    if [ "${NUM_PROCESSES}" -eq 0 ]; then
       eval "nohup "${CMD}" >/dev/null 2>&1 &"
    fi
}

function stop() {
    pid=$(ps -ef | grep "${PREFIX}" | grep -v 'grep' | awk '{print $2}')
    if [ "${pid}" ]; then
       kill -9 ${pid}
    fi
}

function run() {
    case $1 in
        start)
            start
            ;;
        stop)
            stop
            ;;
        *)
            start
            ;;
    esac
}

run "$1"

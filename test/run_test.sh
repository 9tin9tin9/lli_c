#!/bin/bash

# Constants
cannot_compile=0
passed=1
failed=2

# Variables
parallel=1
tests=()

print_msg(){
    bold_attr="tput bold"
    test_name_attr="tput setaf 4"
    reset_attr="tput sgr0"
    passed_attr="tput setaf 2"
    failed_attr="tput setaf 1"
    case $2 in
        $passed)
            echo $($bold_attr)$($test_name_attr)\`${1##*/}\`----$($passed_attr)passed$($reset_attr)
            ;;
        $cannot_compile)
            echo $($bold_attr)$($test_name_attr)\`${1##*/}\`----$($failed_attr)cannot compile$($reset_attr)
            echo $1 > failed.txt
            ;;
        $failed)
            echo $($bold_attr)$($test_name_attr)\`${1##*/}\`----$($failed_attr)failed$($reset_attr)
            echo $1 > failed.txt
            ;;
    esac
}

compile_and_run(){
    clang -g -o $n ../target/obj/*.o $n.c -fsanitize=address -ftrapv -pedantic 2>&1
    if [[ $? != 0 ]]; then
        printf "$compile_output"
        print_msg $1 $cannot_compile
        return
    fi
    output=$(./$1)
    if [[ $? == 0 ]]; then
        print_msg $1 $passed
        rm -rf $1 $1.dSYM
    else
        printf "%s\n" "$output"
        print_msg $1 $failed
    fi
}

kill_tests(){
    for n in "${tests[@]}"; do
        killall $n 2>/dev/null
        if [[ $? == 0 ]]; then
            echo killed $n
        fi
    done
    sleep 1
}

include_all_tests(){
    src=*.c
    for s in ${src[@]}; do
        tests+=(${s%.c})
    done
}

# Parse args
if [[ $# == 0 ]]; then
    include_all_tests
else
    for a in $@; do
        case $a in
            "-f" )
                src=$(cat failed.txt)
                for s in ${src[@]}; do
                    tests+=($s)
                done
                ;;
            "-np" )
                parallel=0
                # include_all_tests
                ;;
            *)
                tests+=(${a%.c})
                ;;
        esac
    done
fi
if [[ -e failed.txt ]]; then rm failed.txt; fi

cd ..
make build
cd -

for n in "${tests[@]}"; do
    if [[ $parallel -eq 1 ]]; then
        compile_and_run $n &
    else 
        compile_and_run $n
    fi
done
trap kill_tests SIGINT SIGTERM SIGKILL SIGHUP
wait

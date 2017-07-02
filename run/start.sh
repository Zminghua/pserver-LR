#!/bin/bash
# set -x

ulimit -c unlimited

if [ $# -lt 10 ]; then
    echo "usage: $0 run_scheduler run_server run_worker start_s end_s start_w end_w num_servers num_workers bin [args..]"
    exit -1;
fi

# model conf
export RANDOM_SEED=10
export DATA_DIR=./census-income
export NUM_FEATURE_DIM=921
export LEARNING_RATE=0.01
export TEST_FREQ=1
export SYNC_MODE=0
export NUM_EPOCH=20
export BATCH_SIZE=256 # -1 means take all examples in each iteration

# ps settings
run_scheduler=$1
shift
run_server=$1
shift
run_worker=$1
shift
start_s=$1
shift
end_s=$1
shift
start_w=$1
shift
end_w=$1
shift
export DMLC_NUM_SERVER=$1
shift
export DMLC_NUM_WORKER=$1
shift
bin=$1
shift
arg="$@"

# start scheduler
export DMLC_PS_ROOT_URI='127.0.0.1'
export DMLC_PS_ROOT_PORT=8001
if [ $run_scheduler -eq 1 ]; then
    export DMLC_ROLE='scheduler'
    ${bin} ${arg} &
fi

# start servers
if [ $run_server -eq 1 ]; then
    export DMLC_ROLE='server'
    for ((i=${start_s}; i<${end_s}; ++i)); do
        export HEAPPROFILE=./S${i}
        ${bin} ${arg} &
    done
fi

# start workers
if [ $run_worker -eq 1 ]; then
    export DMLC_ROLE='worker'
    for ((i=${start_w}; i<${end_w}; ++i)); do
        export HEAPPROFILE=./W${i}
        ${bin} ${arg} &
    done
fi


wait



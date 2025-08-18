# build

`git submodule update --init --recursive`

### build cadical

`cd simple_CAR/src/sat/cadical/`

`./configure --competition && make`

`cd ../../../..`

### build kissat

`cd simple_CAR/src/sat/kissat/`

`./configure --competition && make`

`cd ../../../..`

### build simple_CAR

`mkdir release`

`cd release`

`cmake .. -DCMAKE_BUILD_TYPE=Release -DCADICAL=1 -DKISSAT=1`

`make`

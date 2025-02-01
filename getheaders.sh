#!/bin/bash

# Get shogsrc22.zip
WORK_PATH=`pwd`

git clone https://github.com/wfr/unshieldv3
pushd unshieldv3/build
cmake ..
make -j`nproc`
popd

#wget https://www.gamers.org/pub/idgames2/planetquake/planetshogo/tools/shogsrc22.zip
TEMP_PATH=`mktemp -d`
pushd $TEMP_PATH
unzip $WORK_PATH/shogsrc22.zip
mkdir shogo_src
$WORK_PATH/unshieldv3/build/unshieldv3 extract data.z shogo_src
mv shogo_src $WORK_PATH/
popd

rm -fr unshieldv3
rm -r $TEMP_PATH

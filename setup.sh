#!/bin/bash

# setup storage files
if [[ ! -d ./data ]]; then
  mkdir ./data
> data/items.txt
fi

# setup symbolic link
WORKING_DIR=$(pwd)
echo "#!/bin/bash
cd $WORKING_DIR
./treeplanner" > $WORKING_DIR/run_script
chmod u+x run_script
sudo mv $WORKING_DIR/run_script /usr/local/bin/treeplanner

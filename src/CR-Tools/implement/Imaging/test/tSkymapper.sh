#!

## Determine local file name of the dataset

dataset_lopes_local=`echo ${dataset_lopes} | tr '/' '\n' | grep ".event"`

## copy the original dataset to the local directory

echo "-- Copying dataset to local directory..."
cp ${dataset_lopes} .
echo "-- Adjusting file permissions on test dataset ..."
chmod a+rw $dataset_lopes_local

## Run the test program

echo "-- Running test program on input file $dataset_lopes_local ..."
./tSkymapper $dataset_lopes_local

## Clean up the test directory

echo "-- Cleaning up the test directory ..."
rm $dataset_lopes_local
rm -rf skymap-*

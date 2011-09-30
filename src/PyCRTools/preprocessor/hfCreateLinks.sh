#!/bin/sh

src_d=$1
dst_d=$2

# Overwrite command line values for testing
#src_d=${LOFARSOFT}/src/pycrtools/modules/
#dst_d=${LOFARSOFT}/release/lib/python/pycrtools/

# Check if source directory exists
if test -d $src_d ; then
    src_d=$(cd $src_d ; pwd)
    #echo "src: $src_d"

    # Check if destination directory exists
    if ! test -d $dst_d ; then
	echo "Creating destination directory..."
	mkdir -p $dst_d
    fi

    for dir in $(find $src_d -type d); do
	mkdir -p $dst_d${dir#$src_d}
    done

    dst_d=$(cd $dst_d ; pwd)
    #echo "dst: $dst_d"

    # Loop over files in source directory and create symlinks
    for src_f in $(find $src_d -iname "*py" -type f); do
	dst_f=$dst_d${src_f#$src_d}
	#echo a $src_f
	#echo b $dst_f
	rm -f $dst_f
	ln -s $src_f $dst_f
    done
else
    echo "Source directory does not exist."
fi

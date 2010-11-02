#! /bin/sh

src=$(cd $1 ; pwd)
dst=$(cd $2 ; pwd)

echo "Building new directories in $dest..."
for dir in $(find $src -type d); do
	mkdir -p $dst${dir#$src}
done

echo "Symlinking files..."
for src_f in $(find $src -type f); do
	dst_f=$dst${src_f#$src}
	ln -s $src_f $dst_f
done

echo "Success"


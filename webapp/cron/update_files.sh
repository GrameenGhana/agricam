#!/bin/bash

#
# Script to copy files from the ftp server to the gallery-images folder
#
set -e

copy=`which cp`
chown=`which chown`
ftp_folder="/srv/ftp/agricam"
img_folder="/var/www/html/agricam/webapp/gallery-images"

MAINSTART=$(date +%s)

echo ""
echo "Running Update script: `date`"
echo ""

for file in "$ftp_folder"/*.jpg
do
    image="${file/\/srv\/ftp\/agricam\//}"
    dst=`printf "%s/%s\n" $img_folder $image`

    # Check if file exists.
    if [ ! -e "$file" ]
    then
        #echo "** $file does not exist."; echo
        continue
    fi

    if [ -e "$dst" ]
    then
        #echo "** $dst exists already. Skipping."; echo
        continue
    fi

    START=$(date +%s)
    # echo "- Copying $file to $dst"

    $copy $file $dst
    $chown www-data:www-data $dst
    DIFF=$(($(date +%s)-START))
    echo ">>> $(($DIFF / 60)) minutes and $(($DIFF % 60)) seconds elapsed."
done

MAINDIFF=$(($(date +%s)-MAINSTART))

echo "Finished: `date`. Took $(($MAINDIFF / 60)) minutes and $(($MAINDIFF % 60)) seconds."
echo ""

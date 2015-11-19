#/bin/sh
cd code
gmake Makefile clean
cd ..
tar -cf code.tar code
gzip code.tar
echo "code.tar.gz created. Please upload code.tar.gz to the submission ftp site"

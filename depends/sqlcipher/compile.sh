unzip  sqlcipher-master.zip
cd sqlcipher-master
./configure --enable-tempstore=yes CFLAGS="-fPIC -DSQLITE_HAS_CODEC" LDFLAGS="../../openssl/openssl-1.0.1p/libcrypto.a"
make
cp -f .libs/libsqlcipher.a ../../../src/libs
cp -f sqlite3.h ../../../src/includes

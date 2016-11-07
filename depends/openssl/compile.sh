tar -zxvf openssl-1.0.1p.tar.gz
cd openssl-1.0.1p
./config -fPIC
make
cp -f libssl.a libcrypto.a ../../../src/libs/

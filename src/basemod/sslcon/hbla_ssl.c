/**
 * author:pc
 * copyRight:hba.cn
 * time:2015-09-21
 * desc:init ssl connections,use openssl in static libriary.
 */
#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "openssl/rand.h"
#include "openssl/engine.h"
#include "hbla_ssl.h"

/**
 * service public key & client private key
 */
#define HA_LA_SSL_PASS	"pclvmm"

static char cpServerKey[256] = {0};
static char cpClientKey[256] = {0};
static char cpClientCCert[256] = {0};
static char cpKey[256] = {0};

/**
 * server ip(name) & port
 */
static char cpServerIP[32] = {0};
static unsigned int iServerPort = 0 ;

/**
 * SSL info & context
 */
static SSL *ssl = NULL;
static BIO *bio = NULL;
static SSL_CTX *ctx = NULL;

/**
 * global sock
 */
static int hbla_ssl_sock;

/**
 * select time & fd_set to the sock
 */
static fd_set fds;
static struct timeval tmo;

/**
 * Initialize the openssl,& load the server_Key
 */
static int hbla_ssl_init_ca(const char* const ca_cert_filename)
{
    /* Initializing OpenSSL */
    ENGINE_load_builtin_engines();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    SSLeay_add_ssl_algorithms();
 
    RAND_load_file("/dev/urandom", 1024);
 
    ctx = SSL_CTX_new(SSLv23_method()); // create SSL context
 
    if (NULL == ctx) {
        return 2;
    }
 
    if (!SSL_CTX_load_verify_locations(ctx, ca_cert_filename, NULL)) {
        ERR_print_errors_fp(stderr);
        return 3;
    }
 
    return 0;
	
}

/**
 * Install Client certificate to SSL context.
 */
static int hbla_ssl_set_client_cert(const char *const cert_file, const char *const password)
{
    if (NULL == ctx) {
        return 1;
    }
    if (NULL == cert_file) {
        return 2;
    }
 
    if (NULL != password) {
        SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)password);
    }
 
    if (SSL_CTX_use_certificate_file(ctx, cert_file,SSL_FILETYPE_PEM) != 1) {
        ERR_print_errors_fp(stderr);
        return 3;
    }
 
    return 0;
}

/**
 * Set the client's private key from file to SSL context
 */
static int hbla_ssl_set_client_priv_key(const char *const key_file,
              const char *const password)
{
	
    if (NULL == ctx) {
        return 1;
    }
    if (NULL == key_file) {
        return 2;
    }
 
    if (NULL != password) {
        SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)password);
    }
 
    if (SSL_CTX_use_PrivateKey_file(ctx, key_file, SSL_FILETYPE_PEM) != 1) {
        ERR_print_errors_fp(stderr);
        return 3;
    }
 
    return 0;
}

/**
 * initialize the ssl,the ccpIp,server_key,private key
 * input: 
 * 		ccpIP is the serverIP/name, iPort is the server Port
 * return:
 * 		0 :	success!
 * 		-1:	server_Key is error!
 * 		-2:	client_key is error!
 */
static int hbla_ssl_init()
{
	//set the select block time
	tmo.tv_sec = 1;
	tmo.tv_usec = 0;
	
	// init SSL Librart give Certification Authority(CA) certificate
	if(hbla_ssl_init_ca(cpServerKey) != 0)
		return -1;
	// setup Client's certificate
	if(hbla_ssl_set_client_cert(cpClientCCert, HA_LA_SSL_PASS) != 0)
		return -2;
	//setup client's private key
	if(hbla_ssl_set_client_priv_key(cpClientKey, HA_LA_SSL_PASS) != 0)
		return -2;

//	SSL_CTX_set_timeout(ctx, 120);

	return 0;
}

/**
 * connect to server
 * input: 
 * 		ccpIP is the serverIP/name, iPort is the server Port
 * return:
 * 		0 : success!
 * 		-1: ccpIP error
 * 		-2: bio error
 * 		-3: ssl error
 * 		-4: conenction error
 * 		-5: handshake error
 * 		-6: certificate error
 */
static int hbla_ssl_connectex(const char *const ccpIP,const unsigned int iPort)
{
	long result;
	char conn_string[256];

	if(ccpIP == NULL || strlen(ccpIP) > 32)
		return -1;

	strncpy(cpServerIP, ccpIP, 32);
	iServerPort = iPort;
	
	snprintf(conn_string, sizeof(conn_string), "%s:%i", ccpIP, iPort);

	bio = BIO_new_ssl_connect(ctx);
	if(NULL == bio){
		ERR_print_errors_fp(stderr);
		return -2;
	}

	BIO_get_ssl(bio, &ssl);
	if(NULL == ssl){
		ERR_print_errors_fp(stderr);
		return -3;
	}
	
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

	/* Attempt to connect */
	BIO_set_conn_hostname(bio, conn_string);

	/* Verify the connection opened and perform the handshake */
	if(BIO_do_connect(bio) <= 0){
		ERR_print_errors_fp(stderr);
		return -4;
	}

	if(BIO_do_handshake(bio) <= 0){
		ERR_print_errors_fp(stderr);
		return -5;
	}

	result = SSL_get_verify_result(ssl);

	if(result != X509_V_OK &&
		result != 10){
		fprintf(stderr, "ssl\tcertificate verification error:%i\n",
				(int)SSL_get_verify_result(ssl));

		ERR_print_errors_fp(stderr);

		return -6;
	}

	hbla_ssl_sock = SSL_get_fd(ssl);

	return 0;
}

static void hbla_ssl_free()
{
	if(hbla_ssl_sock != 0)
	{
		hbla_ssl_sock = 0;
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	}
}

/**
 * author:pc
 * time:2015-10-10 16:11
 * desc:connect to server
 */
int hbla_ssl_connect(const char * cpClientPriKey,const char * cpClientCert, const char * cpServerPubKey,
				const char * cpCenterAddr,int iCenterPort)
{
	if((cpClientPriKey == NULL || cpServerPubKey == NULL)
		|| cpCenterAddr == NULL){
		return 1;
	}
	
	strncpy(cpClientKey, cpClientPriKey, 256);
	strncpy(cpClientCCert, cpClientCert, 256);
	strncpy(cpServerKey, cpServerPubKey, 256);
	strncpy(cpServerIP, cpCenterAddr, 32);
	iServerPort = iCenterPort;
	
	if(ctx != NULL)
		hbla_ssl_free();
	if(hbla_ssl_init())
		return 2;

	return hbla_ssl_connectex(cpServerIP, iServerPort);
}

/**
 * read from ssl
 * input:
 * 		buf:malloc by upside
 * 		size:read lengths
 */
int hbla_ssl_read(char *buf,unsigned int size)
{
	int readsocks;
	int ret;
	if(hbla_ssl_sock == 0 || buf == NULL)
		return -1;
	while(hbla_ssl_sock)
	{
		if(SSL_pending(ssl) > 0)
		{
			SSL_read(ssl, buf, size);
			return 0;
		}
		FD_ZERO(&fds);
		FD_SET(hbla_ssl_sock, &fds);
		tmo.tv_sec = 1;
		tmo.tv_usec = 0;
		readsocks = select(hbla_ssl_sock + 1, &fds, NULL, NULL, &tmo);
		if(readsocks < 0)
		{
			hbla_ssl_sock = 0;
			return -1;	
		}else if(readsocks != 0)
		{
			ret = SSL_read(ssl, buf, size);
			if(ret <= 0){
				hbla_ssl_sock = 0;
				return ret;
			}
			return 0;
		}
	}
	return -1;
}
/**
 * get the sock
 */
int hbla_ssl_getSock()
{
	return hbla_ssl_sock;
}
#if 0
int hbla_ssl_read(char *buf,unsigned int size)
{
	int i = 0;
	int len_t = 0;
	int len_all = 0;
	int ret;

	while((len_t = SSL_pending(ssl)) > 0)
	{
		if(len_t == 0){
			usleep(100);
			continue;
		}
		if((len_t + len_all) >= size){
			ret = SSL_read(ssl, buf + len_all, (size - len_all));
			if(ret <= 0)
			{
				hbla_ssl_sock = 0;
				return ret;
			}
			return size;
		}else{
			ret = SSL_read(ssl, buf + len_all, len_t);
			if(ret <= 0)
			{
				hbla_ssl_sock = 0;
				return ret;
			}
			len_all += len_t; 
		}
	}
	
	return 0;
}
#endif

/**
 * write to ssl
 * input:
 * 		buf:malloc by upside
 * 		size:read lengths
 */
int hbla_ssl_write(char *buf,unsigned int size)
{
	int ret;
	
	if(hbla_ssl_sock == 0)
		return -1;
	ret = SSL_write(ssl, buf, size);
	if(ret <= 0)
	{
		hbla_ssl_sock = 0;
		return ret;
	}
	return 0;
}

/**
 * judge the sock is ok
 * return:
 * 		0 is unconnected
 * 		1 is connected
 */
int hbla_ssl_isConnected()
{
	if(hbla_ssl_sock == 0)
		return 0;
	return 1;
}

/**
 * disConnect the link
 */
void hbla_ssl_disConnect()
{
	if(hbla_ssl_sock != 0)
		hbla_ssl_sock = 0;
}

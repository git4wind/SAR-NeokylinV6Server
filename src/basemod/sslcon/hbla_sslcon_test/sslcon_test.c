/*
 * This is a test fro sslCon
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hbla_sslcon.h"

#define CKEY "./client_cert.pem"
#define SKEY "./server_cert.pem"
#define CIP "192.168.0.150"
#define PORT 45451

static iCount;

void send_run(void * tem)
{
	int i;
	i = iCount++;
	while(1)
	{
		//send a message
		char buf[5] = "1234";
		//printf("%d:\n\t\tconnected is = %d\n",i++,hbla_sslcon_isConnected());
		if(hbla_sslcon_isConnected())
			printf("Thread %d:\t\tsned is =%d\n",i,hbla_sslcon_send(0x1,0x12,buf,5));
#if 0
		if(hbla_sslcon_isConnected())
		{
			BASEFRAME * pBf = hbla_sslcon_recv(&ret);
		}
#endif
		sleep(20);
	}

}

int main(int argc, char **argv)
{
	int ret;
	hbla_sslcon_init(1, 4);
	int i = 1;
	pthread_t recvHand[2];

	ret = hbla_sslcon_connect(CKEY, SKEY, CIP, PORT);
	/*
	while(1)
	{
		if(hbla_sslcon_isConnected())
		{
			BASEFRAME * pBf = hbla_sslcon_recv(&ret);
		}
	}
	*/
	ret = pthread_create(&recvHand[0], NULL, &send_run, NULL);
	sleep(1);
	ret = pthread_create(&recvHand[1], NULL, &send_run, NULL);
	
	if(ret != 0)
	{
		printf("crate the recv_run error\n");
		return -1;
	}

	while(1){
		if(hbla_sslcon_isConnected())
		{
			int ret;
			int allCommand;
			BASEFRAME * pBf = hbla_sslcon_recv(&ret);
			if(pBf == NULL)
			{
				sleep(1);
				continue;
			}
			switch(pBf->cFNo)
			{
				//center
			case 0x01:
				allCommand = (((int)pBf->cMNo << 16) | ((int)pBf->sComNo));
				switch(allCommand)
				{
					case 0x707700:
						printf("recv the keeplivePack\n");
						break;
					case 0x010012:
						printf("recv message: %s\n",&pBf->cFBf);
				}
				break;
			default:
				printf("get error package!\n");
				break;
			}
		}
	}

	return 0;
}

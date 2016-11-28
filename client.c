#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <resolv.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/ip.h>
#include <netinet/in.h>


/**********************************************
Step 1: initialize the OpenSSL library
#include <openssl/ssl.h>
int SSL_library_init(void)? ! added to 
**********************************************/

SSL_CTX* clt_ini_opssl_lib(){   


	SSL_library_init();
	const  SSL_METHOD *method;
	SSL_CTX *ctx;

	
	
	/* Load cryptos, et.al. */
	OpenSSL_add_all_algorithms();
	
	/* Bring in and register error messages */	
	SSL_load_error_strings();

	/* Create new client-method instance */			
	method = SSLv2_client_method();	
	
	/* Create new context */
	ctx = SSL_CTX_new(method);
			
	if(ctx!=NULL)	return ctx;

	else{		
		printf("error 1-1\n");
		abort();
		
	}
   

}


/****************************************************
step 2:	 create the socket


****************************************************/


int clt_create_socket(char *hostname, int portnum){


	/*---Standard TCP Client---*/
	int sd;
	struct hostent *host;
	struct sockaddr_in addr;

	//host = gethostbyname(hostname);
 
	/* convert hostname IP addr */
	if ( (host = gethostbyname(hostname)) == NULL ){
        
		printf("error 2-1, hostname\n");
       		abort();
    	}
	

	sd = socket(PF_INET, SOCK_STREAM, 0); 

	
	/* create TCP socket */
  
	bzero(&addr, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(portnum);
 
	/* set the desired port */
	addr.sin_addr.s_addr = *(long*)(host->h_addr); 
	/* and address */
	//connect(sd, (struct sockaddr*)&addr, sizeof(addr));
	/* connect */
	//suceed return 0, error -1;
	if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) !=0 ){

		//perror(hostname);
        	close(sd);
        	printf("error 2-2, create socket\n");
        	abort();
	}

	return sd;
}




int main(int argc, char * argv[]){
	SSL_CTX *ctx;
	int server;
	SSL *ssl;
	char string[100];
	//bzero(string,100);
	//memset(string, 0, 100);
	int x=0;
	int y;
	int bytes;
	char *hostname;
	int portnum;

	if ( argc != 3 ){

		printf("usage: %s <hostname> <portnum>\n", argv[0]);
        	exit(0);
    	}

	hostname = argv[1];
	portnum = atoi(argv[2]);

	//printf("\n%s\n%s\n",hostname,portnum);
    	ctx = clt_ini_opssl_lib();

	//printf("\n%s\n%s\n",hostname,portnum);

	
    	server = clt_create_socket(hostname, portnum);



	//printf("\n%8d\n\n",portnum);
	/* create new SSL connection state */
    	ssl = SSL_new(ctx);

	//printf("\n%d",server);

	/* attach the socket descriptor */
				
	SSL_set_fd(ssl, server);




	//printf("\n8\n");
	/* perform the connection */			
    	if ( -1 == SSL_connect(ssl) ){		
        	printf("error in main; connect fail\n");

	}
	else{

		while(x!= -1){

			printf("please input a number bigger than 0,-1 to exit program after this round \n");
			
			fgets(string,99,stdin);

			//printf("3\n");
			//printf("%s\n",string);
			//printf("3-3\n");
			x = atoi(string);
			//printf("4\n");
			//printf("%d",x);

			while((x<0)&&(x!=-1)){
				printf("please input a number bigger than 0,-1 to exit program after this round \n");
				memset(string, 0, 100);
				//scanf("%s",&string);
				fgets(string,99,stdin);
				x = atoi(string);
			}

			//printf("this is 1\n");
			//bzero(string,100);
			//memset(string, 0, 100);
        		//printf("encryption is %s \n", SSL_get_cipher(ssl));
			
			//itoa(x,string,10);
			//printf("this is 2\n");
       			//encrypt & send message
						
       			bytes=SSL_write(ssl, string, strlen(string));	

			if(bytes<0){
				printf("error in main; bytes<0\n");

			}
			//printf("this is byte 1 %d\n",bytes);
			memset(string, 0, 100);

			//printf(" get ready tp read");
			/* get reply & decrypt */
       			bytes = SSL_read(ssl, string, sizeof(string));
			if(bytes<0){
				printf("error in main; read bytes<0\n");

			}
			//printf("this is byte 2 %d\n",bytes);
			
			y=atoi(string);
        		
			if(y==(x-1)){
				printf("received value is %d, it is correct\n",y);
			}
			else{
				printf("received value is %d, it is not correct\n",y);
			}
			if(x!=-1)
			printf("\nGoing to next round\n\n");
			else
			printf("\nThis is last round\n\n");

			
        		

		}//end of while

		SSL_free(ssl);	
		printf("\n press any key and then press enter to exit");
		scanf("%d",&x);
	}

	/* close socket */
	close(server);	
	/* release context */								
	SSL_CTX_free(ctx);	


	return 0;							
}


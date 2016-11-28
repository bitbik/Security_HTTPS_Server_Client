#include <stdio.h>
#include <netdb.h>//may only used for client
#include <string.h>
#include <resolv.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


/**********************************************
Step 1: initialize the OpenSSL library
#include <openssl/ssl.h>
int SSL_library_init(void)?
**********************************************/

//including load ciphers, error messages, 
//create server instances, create ssl context
SSL_CTX* svr_ini_opssl_lib( ){
	
	const SSL_METHOD *method;
	SSL_CTX *ctx;
	SSL_library_init();
	/* load & register all cryptos, etc. */
	OpenSSL_add_all_algorithms();		
	/* load all error messages */
	SSL_load_error_strings();	
	/* create new server-method instance */
	method = SSLv2_server_method();
	/* create new context from method */
	ctx = SSL_CTX_new(method);			

	if(ctx!=NULL)	return ctx;

	else{		
		printf("error 1-1\n");
		abort();
		
	}

}


/*********************************************
Step 2: load server certificate

*********************************************/




void svr_ld_ctfc(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
	/* set the local certificate from CertFile */
	//Return code 1 indicates that the function was successful, 0 eeror
	//SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) ;
	//success 1; else 0
	if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <1){

		printf("error 2-1\n");
		abort();
		
	}
		

	/* set the private key from KeyFile (may be the same as CertFile) */
	//SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM
	//success 1; else 0
	if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <1 ){

		printf("error 2-2\n");
		abort();
		
	}
		
	/* verify private key */
	if ( !SSL_CTX_check_private_key(ctx) ){

		printf("error 2-3\n");
		abort();
		
	}

	printf("\nload server certificate succeed! \n");
    
}

/****************************************************
step 3:	create server socket

bind to a particular port and convert it to a listenning socket

****************************************************/
int svr_create_socket(int port){

	/*--- Standard TCP server setup and connection ---*/
	int sd;
	//int  client;
	struct sockaddr_in addr;
	//struct sockaddr_in addr2;

	//should be >0
	/* create stream socket */
	sd = socket(PF_INET, SOCK_STREAM, 0); 					

	if(sd<0){
		printf("error 3-1\n");
		abort();
		
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	/* select 'port' */
	addr.sin_port = htons(port);								
	/* any available addr */
	addr.sin_addr.s_addr = INADDR_ANY;			 			

	//bind, no eeror, return 0
	/* bind it */
	if(bind(sd, (struct sockaddr*)&addr, sizeof(addr))!=0){ 
		
		printf("error 3-2");
        	abort();
	}
	/* make into listening socket */
	if(-1==listen(sd,10)){									
		printf("error 3-3\n");
		abort();
	}

	/* await and accept connections */

	/*	
	client = accept(server_sd, NULL, NULL);	
	client = accept(sd, 0, 0); 								
	if(client<0){
		printf("error 3-4\n");
		abort();
		
	}

	*/

	printf("wait for  connection\n");
	
	return sd;
}



/****************************************************
step 4:	hand of client socket descriptper
		to make secure connection
	
****************************************************/

int svr_clt_socket(SSL_CTX * ctx, int client){

	int err;
	int i=1;
	//SSL * ssl;
	int bytes;
	int y;

	char buf[100];
	
	bzero(buf,100);

	 /* get new SSL state with context */

	SSL *ssl = SSL_new(ctx);
	 /* set connection to SSL state */
	 
	SSL_set_fd(ssl, client); 


	/* start the handshaking */
	
	//fail <0,0:was not successful but shut down 

	err=SSL_accept(ssl); 
	if(err<0){
		printf("error 4-1-1\n");
		abort();
	}
	else if(err==0){
		printf("error 4-1-2\n");
		abort();
	}

	else{

		while(1==i){	
			bzero(buf,100);
			/* get HTTP request */
			bytes = SSL_read(ssl, buf, 99); 

			if(bytes>0){

				y = atoi(buf);

				printf("You have entered : %d in server\n",y);
				y--;
				sprintf(buf, "%d", y);


				printf("The new value is %d\n",y);
				
				bytes = SSL_write(ssl, buf, strlen(buf)); /* send reply */
				if (bytes < 0) {

					printf("error in SSL_write\n");

					abort();
				}

			}//end of if,  bytes>0;

			else{

				printf("\n4-1-3 error,SSL_read faild or socket closed \n");
				i=0;

			}//end of else, bytes<0;

	


		}//end of while



		//* close connection & clean up */
		//* get the raw connection */
		//returns the file descriptor which is linked to ssl

		client = SSL_get_fd(ssl); 

		//* release SSL state */
		SSL_free(ssl); 

		//* close connection */
		//original close(sd),no defined sd 
		close(client); 


	}
	return 0;

}


int main(int argc, char *argv[]){

	SSL_CTX *ctx;
	int server;
	char *portnum;
	int client;
	int port;
	
	//socklen_t clientlenth;

	//struct sockaddr_in client_addr;

	if(argc != 4){
		printf("The format of input: ./server ./cert.kem ./key.kem portnumber\n");
		exit(0);
	}
	portnum = argv[3];
	port=atoi(portnum);

	ctx = svr_ini_opssl_lib();
	svr_ld_ctfc(ctx, argv[1], argv[2]);

	server = svr_create_socket(port);

	
	/* await and accept connections */

		
	client = accept(server, NULL, NULL);	
	//client = accept(sd, 0, 0); 								
	if(client<0){
		printf("error 3-4\n");
		abort();
		
	}

	svr_clt_socket(ctx, client);

	
	

	/* close server socket */
	close(server);	
	/* release context */									
	SSL_CTX_free(ctx);


	printf("\nsuceessful exit\n");	

	return 0;								
}






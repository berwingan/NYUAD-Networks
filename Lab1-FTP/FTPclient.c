//Berwin Gan, wqg203, FTPclient

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <ctype.h>
#include <fcntl.h>
#include <netdb.h> //define hostent structer

//GLOBAL
#define MAGGIE 256
#define ARGU 100
#define LINE 2000
#define BIGFISH 128
char prompt[] =">>";

//argv[0] filename, argv[1] server_ipaddress, argv[2] portno

void error(const char *msg){
	perror(msg);
	exit(1);
}
int uber_create(int port, char*cheddar); //argv[1] add
int command(char **arr, int argu_num,int gold, int silver);
int parseline(const char *cmd, char **arr);
void change_director(char **arr, int argu_num);

int main(int argc, char *argv[]){ //need to get host and port from argv//FTP it from TCP , create socket and connect it to remote add
	//first is add, 2nd is port using atoi, 0 is dam file 
	if(argc<3){error("Not enough arguments \n");}
	char msg[MAGGIE];
	int port;
	port  = atoi(argv[2]);

	int shoes;//creation of sockt //socket need port number and address , port and 
	shoes =uber_create(port,argv[1]);
	int check; //use for read and write thanks
	//already connected, place while loop here
	int silver = 0; //account check
	int gold = 0; //ACCOUNt plus pass
	while(1){ //party client start here
	//recv from the server, GOES out and connect to listening server 
		char * arr[ARGU];
		printf("%s",prompt);
		
		bzero(msg, MAGGIE); bzero(arr,ARGU);
		fgets(msg, MAGGIE, stdin); //waits here
		check = write(shoes,msg,strlen(msg)); //send info over before anything else, deal with it there
		if (check<0){error("Writing Error\n");}

		int argu_num = parseline(msg,arr);
//		printf("%s %s %d", arr[1], arr[0], argu_num);
		
		int cmd_check = command(arr,argu_num, gold, silver); //12 for user, 14 for pass
		if(cmd_check==3){printf("There is no such command!\n");}
		else if(cmd_check==86){printf("You need an account!\n");}
		else if(cmd_check ==88){printf("You need to log in first!\n");}
		else if(cmd_check==2){// for ls in server(just receive stuff back)
			sleep(1); //sleep used to prevent racing, there must be a better way to do this
			char bowl[BIGFISH]; //to receive
			char boat[BIGFISH] = "1"; // to check that incoming data is still valid
			int  data;
			data = uber_create(port+1,argv[1]); //THIS IS THE DATA SOCKET,//port is next port over, which means it need to wrtie 1st
			recv(data,boat,BIGFISH,0);//connect and write the first 1

			while(strcmp("1",boat)==0){ //to compare in order to tell more is otw
				recv(data,bowl,BIGFISH,0);
				printf("%s",bowl);
				recv(data,boat,BIGFISH,0); //boat = 1st send already
				if(strcmp("0",boat)==0){
					break;} //no data left incoming
			}
			close(data); //fine to close here bacause it is reusable as per uber
		}
		else if(cmd_check==12){ //USER
			sleep(1);	
			int data =uber_create(port+1,argv[1]);
			char bowl[BIGFISH];
			recv(data,bowl,BIGFISH,0);
			if(strcmp("1",bowl)==0){printf("Password please\n");silver =1;}
			else{silver = 0; printf("No such account!\n");}
			close(data);}

		else if(cmd_check ==14){ //only is silver == 1
			sleep(1);
			int data= uber_create(port+1,argv[1]);
			char bowl[BIGFISH];
			recv(data,bowl,BIGFISH,0);
			if(strcmp("1",bowl)==0){gold=1;printf("Welcome!\n");} //a one time log in
			else{printf("Wrong Password!\n");}
			close(data);}
		else if(cmd_check==6){ //pwd for server
			sleep(1);
			int data;
			data =uber_create(port+1,argv[1]);
			char bowl[BIGFISH]; //only need one transfer
			recv(data,bowl,BIGFISH,0);
			printf("%s\n",bowl);
			close(data);}
		
		else if(cmd_check==4){ //GET FILE FROM server side, port+1
			sleep(1); int biscuit =1;
			FILE *barry;
			int data = uber_create(port+1,argv[1]);
			char bowl[BIGFISH];
			char boat[BIGFISH]; //if 1 that means its real else does not exixst
			recv(data,boat,BIGFISH,0);
			if(strcmp("1",boat)==0){biscuit = 0; barry = fopen(arr[1],"w");} //if there is on that side, open one here
			while(strcmp("1",boat)==0){
				recv(data,bowl,BIGFISH,0); //write to file here
//				if(strcmp("0",bowl)==0){
//					break;}
				fwrite(bowl,sizeof(char),sizeof(bowl),barry); //check final write
				recv(data,boat,BIGFISH,0);
				if(strcmp("0",boat)==0){
					break;}
			}//get the remaining char, break from bowl, which means boat ==1
	
			fclose(barry);
			close(data);
			if(biscuit == 1){printf("There is no such file in the server directory\n");fflush(stdout);}		
		}	

		else if(cmd_check==10){ //put
			FILE *rice; int tea = 0;
			sleep(1);
			char runner[BIGFISH];
			int data =uber_create(port+1,argv[1]); //connect here
			if((rice=fopen(arr[1],"r"))==0){
				tea = 1; // no file
				send(data,"0",BIGFISH,0);} //there is no such file
			else{
				while(fread(runner,sizeof(char),BIGFISH,rice)!=0){
					send(data,"1",BIGFISH,0);
					send(data,runner,BIGFISH,0);
                                                                 } // use ftell then fget
                           
                                  send(data,"0",BIGFISH,0);}								

			if(tea==1){printf("There is no such file in the client directory\n");fflush(stdout);}
			
			fclose(rice);
			close(data);
} 

/*		bzero(msg, MAGGIE);
		check = read(shoes,msg,MAGGIE);
		if (check<0){error("Reading Error\n");}
		printf("Server: %s", msg);  */
		
// ASK JAY WHY		if(cmd_check == 99){break;}
		if(cmd_check==9){break;} //QUIT
	}
	
	close(shoes);

	
	return 0;
}

//parse into arr and return num of argu, make own .c later
int parseline(const char *cmd, char**arr){ //hold in arr, last is NULL
	int num = 0; //num of arguments
	char * space;
	char local_holder[LINE];
	char * ptr = local_holder;
	strcpy(ptr,cmd);
	while(isspace(*ptr)){ptr++;}
	ptr[strlen(ptr)-1] =' ';
	space = strchr(ptr,' ');
	while(space){
		arr[num] =ptr;
		num++;
		ptr = space;
		ptr++;
		while(isspace(*ptr)){ptr++;}
		*space ='\0';
		space = strchr(ptr,' ');
	}
	arr[num] = NULL; //for last
	return num;
}


	

int command(char **arr, int argu_num, int gold, int silver){ //1 means it is within the client, don't need any interaction
	if(strcmp("!ls",arr[0]) ==0){
		system("ls"); return 1;  }
	else if(strcmp("!pwd",arr[0])==0){
		system("pwd"); return 1;}	
	else if(strcmp("!cd",arr[0])==0){change_director(arr,argu_num); return 1; }

	else if(strcmp("pwd",arr[0])==0){if(gold==1){return 6;} else{return 88;}} //all even numb need gold
	else if(strcmp("cd",arr[0])==0){ if(gold==1){return 8;} else{return 88;}}//do nothing except check for even permission	
	else if(strcmp("ls",arr[0])==0){ if(gold==1){return 2;} else{return 88;}}//even need to check for permission

	else if(strcmp("GET",arr[0])==0){ if(gold==1){return 4;} else{return 88;}}
	else if(strcmp("PUT",arr[0])==0){ if(gold==1){return 10;} else{return 88;}}
	else if(strcmp("USER",arr[0])==0){ return 12;}
	else if(strcmp("PASS",arr[0])==0){ if(silver == 1){return 14;} else{return 86;}}
	
	else if(strcmp("QUIT",arr[0])==0){return 9;}  //wtf is going on here

	return 3; // no such command;
}


void change_director(char **arr, int argu_number){
	if(argu_number == 1){chdir("..");}
	else if(chdir(arr[1])<0){ //not work
		printf("No such path\n");}

	return;
}


 
int uber_create(int port, char*cheddar){
	struct sockaddr_in server_address; //check it  out 
	struct hostent *server;
	int shoes;
	shoes = socket(AF_INET,SOCK_STREAM,0); //sock_stream = tcp, 0= default = tcp
	if (shoes<0){error("Socket not found\n");}
	int reuse = 1;
	setsockopt(shoes, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //res in peperonis reuse	

	//inet have structure to connect to 
	//specify add for socket
	server = gethostbyname(cheddar);
	if(server==NULL){ fprintf(stderr,"No such host\n");}

	bzero((char *) &server_address, sizeof(server_address));	
	server_address.sin_family = AF_INET; // type of sock working with
	
	bcopy((char *) server->h_addr,(char *) &server_address.sin_addr.s_addr, server->h_length); //hostent ->server_address bcopy

	server_address.sin_port = htons(port);  //port number
	server_address.sin_addr.s_addr = INADDR_ANY;  //(0.0.0.0)= in_addr_any write another server to handle from this client 
	
	int status = connect(shoes, (struct sockaddr *) &server_address, sizeof(server_address));  //last one is size of add, cast server add struct from sockaddr_in to sockaddr
	//if status = -1 its no bueno
	if (status<0){ error("Connection failed\n");}

	return shoes;

}

/*  I'm keeping this here in case the uber suddently does not work
	struct sockaddr_in server_address; //check it  out 
	struct hostent *server;

	shoes = socket(AF_INET,SOCK_STREAM,0); //sock_stream = tcp, 0= default = tcp
	if (shoes<0){error("Socket not found\n");}
	int reuse = 1;
	setsockopt(shoes, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //res in peperonis reuse	

	//inet have structure to connect to 
	//specify add for socket
	server = gethostbyname(argv[1]);
	if(server==NULL){ fprintf(stderr,"No such host\n");}

	bzero((char *) &server_address, sizeof(server_address));	
	server_address.sin_family = AF_INET; // type of sock working with
	
	bcopy((char *) server->h_addr,(char *) &server_address.sin_addr.s_addr, server->h_length); //hostent ->server_address bcopy

	server_address.sin_port = htons(port);  //port number
	server_address.sin_addr.s_addr = INADDR_ANY;  //(0.0.0.0)= in_addr_any write another server to handle from this client 
	
	int status = connect(shoes, (struct sockaddr *) &server_address, sizeof(server_address));  //last one is size of add, cast server add struct from sockaddr_in to sockaddr
	//if status = -1 its no bueno
	if (status<0){ error("Connection failed\n");}  */


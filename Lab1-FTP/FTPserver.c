//Berwin Gan, wqg203, FTPserver

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <fcntl.h>

//Pyramid
#define ARGU 100
#define LINE 2000
#define MAGGIE 256
#define BIGFISH 128


void error(const char *tix){ //error function for checking
	perror(tix);
	exit(1);
}
int command(char **arr,int argu_num);
int parseline(const char *cmd, char **arr);
void change_director(char **arr, int argu_number);
int bat_create(int port);//the port number need change if specifically 21 for control, 20 for data

//char user[6]= "berwin";
//char password[6] ="123456";


int main(int argc, char *argv[]){//file name and port number, need both
	if(argc<2){exit(1);} //must have both file name and port, dam please 

	int shelf,port, check;
	port = atoi(argv[1]); //only port no such thing as add
	//creating socket
	int shoes;
	shoes =bat_create(port); //shoes is the accept part to read from
	char server_message[MAGGIE];
	int spoon, butt;
	while(1){  //this is server
		bzero(server_message, MAGGIE);
		check = read(shoes, server_message, MAGGIE);
		if(check<0){error("Nothing to read \n");} //double check

		printf("Client: %s",server_message);
		char *arr[ARGU];
		int argu_num = parseline(server_message,arr);
		int cmd_check =0;
		cmd_check = command(arr,argu_num); //2 for ls // should prin number here
//		printf("%d",cmd_check);
//		fflush(stdout);		
		if(cmd_check == 2){ //done using FILE by filling and sending,
			FILE *horse;		
			char runner[BIGFISH];
			spoon = bat_create(port+1); //actual number need to check
			horse = popen("ls","r");
			while(fgets(runner,sizeof(runner),horse) !=NULL){ //close after
				send(spoon,"1",BIGFISH,0); //1 must be first due to wait cycle on other side
				send(spoon,runner,BIGFISH,0);
				bzero(runner,BIGFISH);}
			send(spoon,"0",BIGFISH,0);
			pclose(horse);			
			close(spoon);
		}
		else if(cmd_check==6){ //user
			spoon = bat_create(port+1);
			printf("user is berwin and given is %s\n", arr[1]);
			if(strcmp("berwin",arr[1]) == 0){send(spoon,"1",BIGFISH,0);} //correct user name
			else{send(spoon,"0",BIGFISH,0);}
			close(spoon);}
		else if(cmd_check==7){//pass
 			spoon = bat_create(port+1);
			if(strcmp("123456",arr[1])==0){send(spoon,"1",BIGFISH,0);}
			else{send(spoon,"0",BIGFISH,0);}
			close(spoon);}

	
		else if(cmd_check==3){ //pwd
			spoon = bat_create(port+1);
			char runner[BIGFISH];
			bzero(runner,BIGFISH);
			getcwd(runner,BIGFISH);
			send(spoon,runner,BIGFISH,0);
			close(spoon);}
		else if(cmd_check==4){ //get from this side
			spoon = bat_create(port+1);
			FILE *pencil;
			char runner[BIGFISH];	
			if((pencil =fopen(arr[1],"r"))==0){send(spoon,"0",BIGFISH,0);} //there is no such file;
			else{//send(spoon,"1",BIGFISH,1); //there is such file and do transfer
//				pencil = fopen(arr[1],"r"); //fread returns 0 when done
				while(fread(runner,sizeof(char),BIGFISH,pencil)!=0){
				send(spoon,"1",BIGFISH,0);
				send(spoon,runner,BIGFISH,0);

				} // use ftell then fget
//				send(spoon,"0",BIGFISH,0); //easy ans turn down the BIGfish
				send(spoon,"0",BIGFISH,0);}
			fclose(pencil);
			close(spoon);	
				
}	
		else if(cmd_check==5){ //put on this side
			spoon= bat_create(port+1);
			char runner[BIGFISH];
			char bowl[BIGFISH];
			FILE *cream;
			recv(spoon,runner,BIGFISH,0);
			if(strcmp("0",runner)==0){;} //no such file
			else{ //write to place,runner run
				cream = fopen(arr[1],"w");
				while(strcmp("1",runner)==0){
					recv(spoon,bowl,BIGFISH,0); //use to write
		//			if(strcmp("0",bowl)==0){break;}
					fwrite(bowl,sizeof(char),sizeof(bowl),cream);
					recv(spoon,runner,BIGFISH,0);
		//			if(strcmp("0",runner)==0){break;} //actuall not really needed
				} fclose(cream);}
			close(spoon);
}
/*		bzero(server_message, MAGGIE);

		fgets(server_message, MAGGIE, stdin); //waits here
		check = write(shoes, server_message, strlen(server_message));
		if(check<0){error("Nothing to write \n");}
	//may wanna put a command check here to simplify	*/

	//send msg, DON"T FORGETII THIS SPAGETII
//	send(shoes,server_message, sizeof(server_message),0);
		int i =strncmp("QUIT", server_message, 4);
		if(i==0){break;}
}
	//close actual and accept

	close(shoes);
	
	return 0;
}

int parseline(const char *cmd, char **arr){
	int num = 0;
	char *space;
	char local_holder[LINE];
	char *ptr = local_holder;
	strcpy(ptr,cmd);
	while(isspace(*ptr)){ptr++;}
	ptr[strlen(ptr)-1]=' ';
	space = strchr(ptr,' ');
	while(space){
		arr[num]=ptr;
		num++;
		ptr= space;
		ptr++;
		while(isspace(*ptr)){ptr++;}
		*space = '\0';
		space = strchr(ptr,' ');
	}
	arr[num] = NULL;
	return num;
}

int command(char **arr, int argu_num){
	if(strcmp("cd",arr[0]) == 0 ){
		change_director(arr,argu_num);return 1;}  //can do a return call to client to check if it works or not
	else if(strcmp("ls",arr[0]) == 0){return 2;}
	else if(strcmp("pwd",arr[0]) ==0){return 3;}
	else if(strcmp("GET",arr[0]) == 0){return 4;}
	else if(strcmp("PUT",arr[0]) ==0){return 5;} //sleep need not be reversed, just recv the otehr way
	else if(strcmp("USER",arr[0]) ==0){return 6;}
	else if(strcmp("PASS",arr[0])==0){return 7;}
	return 0; // no such cmd
}


void change_director(char **arr, int argu_number){
	if(argu_number == 1){chdir("..");}
	else if(chdir(arr[1])<0){
		printf("No such path\n");} //may want to inform client 
	return;
}

int bat_create(int port){
	int shelf;
	//def serv add
	struct sockaddr_in server_address , client_address; //cli
	socklen_t client_length; //make ealier

	shelf = socket(AF_INET,SOCK_STREAM, 0);
	if (shelf<0){error("Socket not found \n");}
	int reuse = 1;
 	setsockopt(shelf, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //REMEMBER TO ALWAYS REUSE ALL THE SOCKETS CAUSE 
	
	bzero((char *) &server_address, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port); //change for listen, hosst to network short

	//bind the socket to IP & port	, DON"T TOUCH
	bind(shelf, (struct sockaddr *) &server_address, sizeof(server_address));//check again addrlen
	if(bind<0){error("Bind not found \n");} //at least till here

	//at least 1 for 1 client
	listen(shelf, 5); //3 = number of connection the system can handle at once, only need 1 for 1 client
	//integer to hold client socket, gotta while(1) here for the cycle
	client_length = sizeof(client_address); //shelf is sock

	int shoes;
	shoes = accept(shelf, (struct sockaddr *) &client_address, &client_length); //2&3 struct and sizeof address of client, not needed,// waits for listen
	if(shoes<0){error("Accept not found \n");} 
	close(shelf); //transfer lmaow why did i do this
//	printf("This");
	return shoes;
}

/*	//def serv add  just in case
	struct sockaddr_in server_address , client_address; //cli
	socklen_t client_length; //make ealier

	shelf = socket(AF_INET,SOCK_STREAM, 0);
	if (shelf<0){error("Socket not found \n");}
	int reuse = 1;
 	setsockopt(shelf, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); //REMEMBER TO ALWAYS REUSE ALL THE SOCKETS CAUSE 
	
	bzero((char *) &server_address, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port); //change for listen, hosst to network short

	//bind the socket to IP & port	, DON"T TOUCH
	bind(shelf, (struct sockaddr *) &server_address, sizeof(server_address));//check again addrlen
	if(bind<0){error("Bind not found \n");} //at least till here

	//at least 1 for 1 client
	listen(shelf, 5); //3 = number of connection the system can handle at once, only need 1 for 1 client
	//integer to hold client socket, gotta while(1) here for the cycle
	client_length = sizeof(client_address); //shelf is sock  */

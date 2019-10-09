/*
 * Nabil Rahiman
 * NYU Abudhabi
 * email: nr83@nyu.edu
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>

#include"packet.h"
#include"common.h"

#define STDIN_FD    0
#define RETRY  120 //millisecond 

int ssthresh = 64;
double cwnd = 1;

int eof = 0;
int finish = 0;
int sockfd, serverlen;
struct sockaddr_in serveraddr;
struct itimerval timer; 
tcp_packet *sndpkt;
tcp_packet *recvpkt;
sigset_t sigmask;

int dup_cnt = 0;
int next_expect = 0; //= last unacked
int next_send = 0; //= next one to be send
char buffer[DATA_SIZE];
int round_cnt = 0;
FILE *fp;
// FILE *fs;
FILE *ft;
int len;
int last_3_seq = -1;
double t;
int timeout_cnt = 0;
void sendp();
void resend();
void resend_packets(int sig);

clock_t start, stop;

void start_timer()
{
    // VLOG(DEBUG, "Start Timer");
    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
    setitimer(ITIMER_REAL, &timer, NULL);
}


void stop_timer()
{
    // VLOG(DEBUG, "Stop Timer");
    sigprocmask(SIG_BLOCK, &sigmask, NULL);
}


/*
 * init_timer: Initialize timer
 * delay: delay in milliseconds
 * sig_handler: signal handler function for resending unacknowledged packets
 */
void init_timer(int delay, void (*sig_handler)(int)) 
{
    signal(SIGALRM, resend_packets);
    timer.it_interval.tv_sec = delay / 1000;    // sets an interval of the timer
    timer.it_interval.tv_usec = (delay % 1000) * 1000;  
    timer.it_value.tv_sec = delay / 1000;       // sets an initial value
    timer.it_value.tv_usec = (delay % 1000) * 1000;

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);
}

void sendp(){ //send packets until there are cwnd packets unacked
    // fprintf(fs, "%d,%lf,%d\n", round_cnt, cwnd,ssthresh );
    round_cnt++;
    while(next_send<=next_expect+(cwnd-1)*DATA_SIZE){
        fseek(fp, next_send, SEEK_SET);
        len = fread(buffer, 1, DATA_SIZE, fp);
        if (len <= 0){//EOF reached
            eof = next_send;
            sndpkt = make_packet(0);
            VLOG(INFO, "End Of File has been reached");
            sendto(sockfd, sndpkt, TCP_HDR_SIZE,  0,
                    (const struct sockaddr *)&serveraddr, serverlen);
            break;
        }  
        else{
            sndpkt = make_packet(len);
            memcpy(sndpkt->data,buffer,len);
            sndpkt->hdr.seqno = next_send;
            VLOG(DEBUG, "Sending packet %d to %s", 
                    next_send, inet_ntoa(serveraddr.sin_addr));
            if(sendto(sockfd, sndpkt, TCP_HDR_SIZE + get_data_size(sndpkt), 0, 
                ( const struct sockaddr *)&serveraddr, serverlen) < 0){
                error("sendto");
            }
            next_send+=len;
        }
    }
    start_timer();  
}

void resend(){//resend packages from next_expected
    if(eof){
        timeout_cnt++;
        if(timeout_cnt==5){
            finish =1;
            exit(0);
        }
        
    }
    next_send = next_expect;
    
    
    sendp();
}

void resend_packets(int sig)
{
    if (sig == SIGALRM)
    {
        //Resend WND packets starting from next_expect.
        stop = clock();
        t = ((double) (stop - start)) / CLOCKS_PER_SEC;
        fprintf(ft, "%lf,%lf\n", t, cwnd); //print time and cwnd to output.csv


        ssthresh = (int)(cwnd/2.0);
        if(ssthresh<2) ssthresh = 2;
        cwnd = 1.0; 

        stop = clock();
        t = ((double) (stop - start)) / CLOCKS_PER_SEC;
        fprintf(ft, "%lf,%lf\n", t, cwnd); //print time and cwnd to output.csv

        stop_timer();
        VLOG(INFO, "Timeout happened. EOF: %d, FINISH: %d",eof,finish);
        resend();
    }
}





int main (int argc, char **argv)
{
    int portno;
    char *hostname;

    start = clock();
    ft = fopen("CWND.csv","w");
    // fs = fopen("output.csv","w");
    /* check command line arguments */
    if (argc != 4) {
        fprintf(stderr,"usage: %s <hostname> <port> <FILE>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    fp = fopen(argv[3], "r");
    if (fp == NULL) {
        error(argv[3]);
    }

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");


    /* initialize server server details */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serverlen = sizeof(serveraddr);

    /* covert host into network byte order */
    if (inet_aton(hostname, &serveraddr.sin_addr) == 0) {
        fprintf(stderr,"ERROR, invalid host %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(portno);

    assert(MSS_SIZE - TCP_HDR_SIZE > 0);

    //Stop and wait protocol

    init_timer(RETRY, resend_packets);
    
    sendp();//send the first packages



    do{
        if(finish) break;
        while(1){//wait for ACK
            stop = clock();
            t = ((double) (stop - start)) / CLOCKS_PER_SEC;
            fprintf(ft, "%lf,%lf\n", t, cwnd); //print time and cwnd to output.csv

            if(finish) break;
            if(recvfrom(sockfd, buffer, MSS_SIZE, 0,
                        (struct sockaddr *) &serveraddr, (socklen_t *)&serverlen) < 0)
            {
                error("recvfrom");
            }

            recvpkt = (tcp_packet *)buffer;
            // printf("%d \n", get_data_size(recvpkt));
            // assert(get_data_size(recvpkt) <= DATA_SIZE);
            VLOG(DEBUG,"Received ACK %d %d",recvpkt->hdr.ackno,get_data_size(recvpkt));
            if(recvpkt->hdr.ackno <= next_expect){//if ACK is duplicate
                if(recvpkt->hdr.ackno - last_3_seq <= 2*ssthresh*DATA_SIZE){ 
                // This is to avoid one round of sending causing multiple 3 duplicates
                    stop_timer();
                    start_timer();
                    continue;
                }
                dup_cnt++;
                if(dup_cnt==3){//three duplicate, fast retransmit
                    if(eof){
                        finish = 1;
                        break;
                    }
                    stop = clock();
                    t = ((double) (stop - start)) / CLOCKS_PER_SEC;
                    fprintf(ft, "%lf,%lf\n", t, cwnd); //print time and cwnd to output.csv

                    ssthresh = (int)(cwnd/2.0);
                    if(ssthresh<2) ssthresh = 2;
                    cwnd = 1;

                    stop = clock();
                    t = ((double) (stop - start)) / CLOCKS_PER_SEC;
                    fprintf(ft, "%lf,%lf\n", t, cwnd); //print time and cwnd to output.csv

                    last_3_seq = recvpkt->hdr.ackno;
                    next_expect = recvpkt->hdr.ackno;
                    VLOG(INFO, "Received Three Duplicates");
                    dup_cnt=0;
                    stop_timer();
                    resend();
                }
            
            }
            else{//ACK is in order
                if(recvpkt->hdr.ackno == eof){
                    finish = 1;
                    break;
                }
                dup_cnt = 1;
                stop_timer();
                next_expect = recvpkt->hdr.ackno;
                if(cwnd<ssthresh){//Slow Start
                    cwnd+=1.0;
                }
                else{//Congestion Avoidance
                    cwnd+=(1.0/cwnd);
                }
                if(next_expect<next_send) start_timer();
                break;
            }
        }
        if(finish) break;
        if(next_expect>=next_send){//finished this round, go to next
            next_send = next_expect;
            sendp();
        }
    }while(1);
    fclose(ft);
    fclose(fp);
    // fclose(fs);
    return 0;

}




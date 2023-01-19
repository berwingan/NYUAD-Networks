# NYUAD-Networks

Spring 2019
Computer Networks CS-UH 3012-001

Lab 1: FTP Client Server interface

The server would always be required to listen for the client socket. This include the starting socket and the later data transfer socket. I place the socket creation into a function. As such, I would create an extra socket with a port number of +1 from the one used to start the client-server communication. This data trasnfer socket will always be closed at the end of any function. A problem I faced was sycncing the server and client when opening a new socket for data transfer. I sidestep this by placing a sleep(1) on the client side to wait for the server to set up its socket.

In hindsight, I should have just set up two sockets in the beginning and close them in the end and not bother to open and close repeatedly the data socket.

I would directly write from stdin into the socket and parse it individually on bothside. As such, for any function that needed any data transfer (eg. pwd,ls,GET), I would open a data socket and listen. If no action was required, I would send a '0' character signalling that nothing was to be done on the other side.

Lab 2: Traffic

This is a simple version of the TCP reliable data transfer based on UDP (No connection, only packets and ACKs).

The program runs like follows:
sender:
1) send first ten packets
2) start timer, wait for ACK
	2a) if timeout, resend the ten packets
	2b) if out-of-order ACK, disregard
	2c) if in-order ACK, stop timer, read another segment, send as packet, start timer
3) repeat 2) until EOF

receiver:
1) wait for packet, and send back ACK along with sequence no of the packet
2) repeat 1) until EOF

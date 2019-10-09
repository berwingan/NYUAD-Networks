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


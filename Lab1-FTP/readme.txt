Lab 1 (FTP Client Server interface)
username:berwin
password:123456 
commands are seperated by spaces

The server would always be required to listen for the client socket. This include the starting socket and the later data transfer socket. I place the socket creation into a function. As such, I would create an extra socket with a port number of +1 from the one used to start the client-server communication. This data trasnfer socket will always be closed at the end of any function. A problem I faced was sycncing the server and client when opening a new socket for data transfer. I sidestep this by placing a sleep(1) on the client side to wait for the server to set up its socket.

In hindsight, I should have just set up two sockets in the beginning and close them in the end and not bother to open and close repeatedly the data socket.

I would directly write from stdin into the socket and parse it individually on bothside. As such, for any function that needed any data transfer (eg. pwd,ls,GET), I would open a data socket and listen. If no action was required, I would send a '0' character signalling that nothing was to be done on the other side.

The biggest problem was the GET and PUT, when setting up the data conenction, I used a certain size char array to transfer the data to and from the client and server. Using fread = 0 to monitor wether or not I can continue reading, I will eventually run across some characters that were missed. For example, if the char array is too big, a fread would return 0 as it reaches the end before the entire array is used, which meant that I would not read the last bit. I tried to reduce this problem by bringing the size of the array down to 128. However, that meant that any files I was trying to PUT and GET that had a total that is less than the equivalent of 128 character would result in the first however many character being copied followed by a bunch of nonsense in the remaining 128 character.

Sometimes the USER and PASS do not work. I can't figure out why. The rate of failure which result in a "Writing Error" is 1/7. 


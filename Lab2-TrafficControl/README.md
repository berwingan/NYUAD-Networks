# project2
starter code project2
## How to Build

cd ~/github/project2/rdt2.0/src

mininet@mininet-vm:~/github/project2/rdt2.0/src$ make


## How to test Task1 ?


cd ~/github/project2/

dd if=/dev/urandom of=small_file.bin  bs=1048576 count=1

### run mininet
sudo mn --link tc,bw=10,delay=10ms,loss=2

mininet> xterm h1 h2

### Two terminal will popup for h1 and h2; Run sender and receiver on these two terminal

### terminal node h1:
./rdt2.0/obj/rdt_receiver 60001 FILE_RCVD

### terminal node h2:
./rdt2.0/obj/rdt_sender 10.0.0.1 60001 small_file.bin


### verify the two files

cksum FILE_RCVD small_file.bin


## How to test Task 2 ?


### Repeat the step from task1

### From mininet VM run 

scp -r -P 4410 nr83@M-DCLAP-P302-CSD.ABUDHABI.NYU.EDU:/var/ftp/project2.tar.gz .

### Extract the file and replace the folder project2/rdt2.0 with your version

### Go to rdt2.0/src directory and build

### Go to project2 directory

project2$ ls -l

total 181912

-rw-r--r-- 1 mininet mininet   1647925 Apr  5  2017 DL_2_16Mbps

-rw-rw-r-- 1 mininet mininet 182501376 Apr  5  2017 FILE

-rw-r--r-- 1 root    root      1048576 Mar 13 23:02 FILE_RCVD

-rw-r--r-- 1 root    root         1036 Mar 13 22:30 c1

drwxrwxr-x 5 mininet mininet      4096 Apr  5  2017 cellsim

#### drwxr-xr-x 2 mininet mininet      4096 Mar 13 22:30 output

#### -rw-r--r-- 1 mininet mininet      1753 Mar 13 22:27 plot.py

#### drwxrwxr-x 3 mininet mininet      4096 Mar 13 23:04 rdt2.0

#### -rw-r--r-- 1 mininet mininet      3819 Apr 12  2017 run.py

#### -rwxrwxr-x 1 mininet mininet       272 Apr 11  2017 run.sh

-rw-rw-r-- 1 mininet mininet   1048576 Mar 13 22:45 small_file.bin


project2$ sudo ./run.sh


### Verify the throughput file in output directory



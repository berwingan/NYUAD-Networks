from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Controller, RemoteController, OVSController, OVSKernelSwitch, UserSwitch, Host, Node
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.log import setLogLevel, info

from subprocess import Popen, PIPE
from argparse import ArgumentParser
from multiprocessing import Process

import sys
import os
import threading
from time import sleep, time

parser = ArgumentParser(description="Shallow queue tests")

parser.add_argument('--dir', '-d',
					help="Directory to store outputs",
					required=True)

parser.add_argument('--trace', '-tr',
					help="Cellsim traces to be used",
					required=True)

parser.add_argument('--time', '-t',
					help="Duration (sec) to run the experiment",
				   type=int,
					default=10)

parser.add_argument('--name', '-n',
					help="name of the experiment",
					required=True)

parser.add_argument('--loss', '-l',
					help="link loss rate 0 for 0\% and 1 for 100\%",
					type=float,
					default=0.0)

args = parser.parse_args()

def simpleRun():
	if not os.path.exists(args.dir):
		os.makedirs(args.dir)

	net = Mininet( topo=None,
				   build=False,
				   ipBase='10.0.0.0/8')

	c0=net.addController(name='c0',
					  controller=Controller,
					  protocol='tcp',
					  port=6633)

	s2 = net.addSwitch('s2', cls=OVSKernelSwitch)
	s1 = net.addSwitch('s1', cls=OVSKernelSwitch)

	h2 = net.addHost('router', cls=Host, defaultRoute=None,  mac='00:00:00:00:00:02')
	h3 = net.addHost('sender', cls=Host, defaultRoute=None,  mac='00:00:00:00:00:03')
	h1 = net.addHost('receiver', cls=Host, defaultRoute=None,  mac='00:00:00:00:00:01')

	net.addLink(h3, s2)
	net.addLink(s2, h2)
	net.addLink(h2, s1)
	net.addLink(s1, h1)

	net.build()

	for h in net.hosts:
		h.cmd("sysctl -w net.ipv6.conf.all.disable_ipv6=1")
		h.cmd("sysctl -w net.ipv6.conf.default.disable_ipv6=1")
		h.cmd("sysctl -w net.ipv6.conf.lo.disable_ipv6=1")

	for controller in net.controllers:
		controller.start()

	net.get('s2').start([c0])
	net.get('s1').start([c0])

	net.start()

	h1.cmd('ifconfig receiver-eth0 10.0.0.1 netmask 255.255.255.0 mtu 1500')
	h2.cmd('ifconfig router-eth1 10.0.0.2 netmask 255.255.255.0 mtu 1500')
	h2.cmd('ifconfig router-eth0 10.0.1.2 netmask 255.255.255.0 mtu 1500')
	h3.cmd('ifconfig sender-eth0 10.0.1.1 netmask 255.255.255.0 mtu 1500')

	h1.cmd('ethtool --offload receiver-eth0 gso off  tso off gro off')
	h3.cmd('ethtool --offload sender-eth0 gso off  tso off gro off')

	h2.cmd('ifconfig router-eth0 up promisc')
	h2.cmd('ifconfig router-eth1 up promisc')
	h2.cmd('ethtool --offload router-eth0 gso off  tso off gro off')
	h2.cmd('ethtool --offload router-eth1 gso off  tso off gro off')

	h3.cmd('ip route flush route 0/0')
	h3.cmd('route add -net 10.0.0.0/24 dev sender-eth0')
	h3.cmd('route add default gateway 10.0.1.1')

	h1.cmd('ip route flush route 0/0')
	h1.cmd('route add -net 10.0.0.0/24 dev receiver-eth0')
	h1.cmd('route add default gateway 10.0.0.1')

	RunTest(net)
	print ("Finished")

	return

def RunTest(net):
	h3 = net.get('sender')
	h2 = net.get('router')
	h1 = net.get('receiver')

	# CLI(net)

	h1.cmd("./rdt2.0/obj/rdt_receiver 60001 FILE_RCVD 2> "+args.dir+"/"+args.name+"_receiver.csv &")

	print("Begin " + str(args.time) + " seconds of transmission")

	h2.cmd('./cellsim/sender/cellsim1 ./'+ str(args.trace) + ' ' + str(args.trace) +' 00:00:00:00:00:01 '+str(args.loss)+' router-eth0 router-eth1  2> c1 &')
	sleep(1)

	h3.cmd("./rdt2.0/obj/rdt_sender 10.0.0.1 60001 FILE 2> "+args.dir+"/"+args.name+"_sender.csv &")
	# CLI (net)		

	sleep(args.time)

	h3.cmd("ps | pgrep -f rdt_server | xargs kill -9")
	h1.cmd("ps | pgrep -f rdt_sender | xargs kill -9")
	h2.cmd("ps | pgrep -f cellsim1 | xargs kill -9")
	
if __name__ == '__main__':
	simpleRun()

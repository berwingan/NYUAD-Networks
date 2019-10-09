from pylab import *

c = "CWND.csv"
x, y= loadtxt(c, delimiter=',', unpack=True)

plot(x,y)

savefig("cwnd.png")
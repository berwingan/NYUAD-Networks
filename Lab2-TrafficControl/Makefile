all: rdt_sender rdt_receiver
rdt_sender: rdt_sender.c common.c common.h packet.c packet.h
	gcc -o rdt_sender rdt_sender.c common.c packet.c -std=gnu99
rdt_receiver: rdt_receiver.c common.c common.h packet.c packet.h
	gcc -o rdt_receiver rdt_receiver.c common.c packet.c -std=gnu99

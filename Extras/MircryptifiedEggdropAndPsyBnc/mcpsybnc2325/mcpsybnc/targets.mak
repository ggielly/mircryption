$(SRC)p_blowfish.o : $(SRC)p_blowfish.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_blowfish.c -o $(SRC)p_blowfish.o

$(SRC)p_client.o : $(SRC)p_client.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_client.c -o $(SRC)p_client.o

$(SRC)p_crypt.o : $(SRC)p_crypt.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_crypt.c -o $(SRC)p_crypt.o

$(SRC)p_dcc.o : $(SRC)p_dcc.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_dcc.c -o $(SRC)p_dcc.o

$(SRC)p_hash.o : $(SRC)p_hash.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_hash.c -o $(SRC)p_hash.o

$(SRC)p_idea.o : $(SRC)p_idea.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_idea.c -o $(SRC)p_idea.o

$(SRC)p_inifunc.o : $(SRC)p_inifunc.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_inifunc.c -o $(SRC)p_inifunc.o

$(SRC)p_link.o : $(SRC)p_link.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_link.c -o $(SRC)p_link.o

$(SRC)p_log.o : $(SRC)p_log.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_log.c -o $(SRC)p_log.o

$(SRC)p_memory.o : $(SRC)p_memory.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_memory.c -o $(SRC)p_memory.o

$(SRC)p_network.o : $(SRC)p_network.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_network.c -o $(SRC)p_network.o

$(SRC)p_translate.o : $(SRC)p_translate.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_translate.c -o $(SRC)p_translate.o

$(SRC)p_parse.o : $(SRC)p_parse.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_parse.c -o $(SRC)p_parse.o

$(SRC)p_peer.o : $(SRC)p_peer.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_peer.c -o $(SRC)p_peer.o

$(SRC)p_server.o : $(SRC)p_server.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_server.c -o $(SRC)p_server.o

$(SRC)p_socket.o : $(SRC)p_socket.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_socket.c -o $(SRC)p_socket.o

$(SRC)p_string.o : $(SRC)p_string.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_string.c -o $(SRC)p_string.o

$(SRC)p_sysmsg.o : $(SRC)p_sysmsg.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_sysmsg.c -o $(SRC)p_sysmsg.o

$(SRC)p_userfile.o : $(SRC)p_userfile.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_userfile.c -o $(SRC)p_userfile.o

$(SRC)p_uchannel.o : $(SRC)p_uchannel.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_uchannel.c -o $(SRC)p_uchannel.o

$(SRC)p_script.o : $(SRC)p_script.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_script.c -o $(SRC)p_script.o

$(SRC)p_topology.o : $(SRC)p_topology.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_topology.c -o $(SRC)p_topology.o

$(SRC)p_intnet.o : $(SRC)p_intnet.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_intnet.c -o $(SRC)p_intnet.o

$(SRC)p_coredns.o : $(SRC)p_coredns.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_coredns.c -o $(SRC)p_coredns.o

$(SRC)psybnc.o : $(SRC)psybnc.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)psybnc.c -o $(SRC)psybnc.o

$(SRC)match.o : $(SRC)match.c
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)match.c -o $(SRC)match.o

$(SRC)snprintf.o : $(SRC)snprintf.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)snprintf.c -o $(SRC)snprintf.o

$(SRC)bsd-setenv.o : $(SRC)bsd-setenv.c $(SRC)p_global.h config.h
	$(CC) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)bsd-setenv.c -o $(SRC)bsd-setenv.o

$(SRC)p_mircryptionwrap.o : $(SRC)p_mircryptionwrap.cpp $(SRC)p_global.h $(SRC)p_mircryptionwrap.h config.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)p_mircryptionwrap.cpp -o $(SRC)p_mircryptionwrap.o

$(SRC)mircryption/b64stuff.o : $(SRC)mircryption/b64stuff.cpp $(SRC)mircryption/b64stuff.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)mircryption/b64stuff.cpp -o $(SRC)mircryption/b64stuff.o

$(SRC)mircryption/BlowfishCbc.o : $(SRC)mircryption/BlowfishCbc.cpp $(SRC)mircryption/BlowfishCbc.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)mircryption/BlowfishCbc.cpp -o $(SRC)mircryption/BlowfishCbc.o

$(SRC)mircryption/mc_blowfish.o : $(SRC)mircryption/mc_blowfish.cpp $(SRC)mircryption/mc_blowfish.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)mircryption/mc_blowfish.cpp -o $(SRC)mircryption/mc_blowfish.o

$(SRC)mircryption/newblowfish.o : $(SRC)mircryption/newblowfish.cpp $(SRC)mircryption/newblowfish.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)mircryption/newblowfish.cpp -o $(SRC)mircryption/newblowfish.o

$(SRC)mircryption/oldblowfish.o : $(SRC)mircryption/oldblowfish.cpp $(SRC)mircryption/oldblowfish.h
	$(CPP) $(INCLUDE) -c $(CFLAGS) $(DEFINE) $(SRC)mircryption/oldblowfish.cpp -o $(SRC)mircryption/oldblowfish.o

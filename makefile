.Silent:

Socket=Socket/
Prot=Protocole/
CC = g++ -I $(Socket) -I $(Prot)
all: Serveur Client CreationBD

Serveur:	Serveur.cpp $(Socket)TCP.o $(Prot)OVESP.o
	echo "Creation de Ser"
	$(CC) Serveur.cpp $(Socket)TCP.o $(Prot)OVESP.o -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl -o Serveur 

CreationBD:	CreationBD.cpp
	g++ -o CreationBD CreationBD.cpp -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl
$(Socket)TCP.o :	$(Socket)TCP.cpp
	echo "Creation de CreaS.o"
	$(CC) $(Socket)TCP.cpp -c -o $(Socket)TCP.o 

$(Prot)SMOP.o:	$(Prot)SMOP.cpp
	echo "Creation de SMOP.o"
	$(CC) $(Prot)SMOP.cpp -c -o $(Prot)SMOP.o

$(Prot)OVESP.o:	$(Prot)OVESP.cpp
	echo "Creation de OVESP.o"
	$(CC) $(Prot)OVESP.cpp -c -I/usr/include/mysql -m64 -L/usr/lib64/mysql -lmysqlclient -lpthread -lz -lm -lrt -lssl -lcrypto -ldl -o $(Prot)OVESP.o

Client: mainclient.o windowclient.o moc_windowclient.o $(Socket)TCP.o $(Socket)TCP.h 
	g++ -Wno-unused-parameter -o Client mainclient.o windowclient.o moc_windowclient.o $(Socket)TCP.o /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread

moc_windowclient.o: moc_windowclient.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o moc_windowclient.o moc_windowclient.cpp

windowclient.o: windowclient.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o windowclient.o windowclient.cpp

mainclient.o: mainclient.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o mainclient.o mainclient.cpp


clean:
	rm $(Socket)*.o $(Prot)*.o *.o
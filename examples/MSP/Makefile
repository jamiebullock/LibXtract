NAME=xtract~
CSYM=xtract~

all: universal
current: universal

MAXINCLUDE = /usr/local/include/max-includes 
MSPINCLUDE = /usr/local/include/msp-includes 
FRAMEWORKS = /Library/Frameworks
INSTALLDIR = /Applications/Audio/MaxMSP\ 4.6.3/Cycling\ \'74/externals/

LIPO = /usr/bin/lipo
CC = gcc

CFLAGS = -F$(FRAMEWORKS) -Wmost -Wno-four-char-constants -Wno-unknown-pragmas -x c -pipe -Wno-trigraphs -fpascal-strings -fasm-blocks -Os -fmessage-length=0 -I$(MAXINCLUDE) -I$(MSPINCLUDE) -include macho-prefix.h  -Werror

I386CFLAGS = -arch i386 
PPCCFLAGS = -arch ppc -faltivec -fasm-blocks -Wno-unused

LDFLAGS = -F$(FRAMEWORKS) -L$(MAXINCLUDE) -L$(MSPINCLUDE) -framework Carbon -framework MaxAPI -framework MaxAudioAPI -Wl,-Y,1455 -bundle -L/usr/local/lib -lxtract

I386LDFLAGS = -arch i386
PPCLDFLAGS = -arch ppc

universal: 
	$(CC) $(CFLAGS) $(I386CFLAGS) -o $(NAME)-i386.o -c $(NAME).c
	$(CC) $(CFLAGS) $(PPCCFLAGS) -o $(NAME)-ppc.o -c $(NAME).c
	$(CC) $(LDFLAGS) $(I386LDFLAGS) -o $(NAME)-i386 $(NAME)-i386.o
	$(CC) $(LDFLAGS) $(PPCLDFLAGS) -o $(NAME)-ppc $(NAME)-ppc.o
	$(LIPO) -create $(NAME)-ppc $(NAME)-i386 -output $(NAME)
	$(LIPO) -create $(NAME)-i386 -output $(NAME)
	mkdir -p $(NAME).mxo/Contents/MacOS
	cp Info.plist PkgInfo $(NAME).mxo/Contents/
	cp $(NAME) $(NAME).mxo/Contents/MacOS

clean:
	rm -rf *i386* *ppc* xtract~ *.mxo

maintainer-clean:
	rm -rf *i386* *ppc* xtract~ *.mxo

install:
	$(INSTALL) cp -r $(NAME).mxo $(INSTALLDIR)

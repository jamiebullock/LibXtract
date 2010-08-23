NAME=xtract~
CSYM=xtract~

all: universal
current: universal

FRAMEWORKS = /Library/Frameworks
MAXINCLUDE = $(FRAMEWORKS)/MaxAPI.framework/Headers
MSPINCLUDE = $(FRAMEWORKS)/MaxAudioAPI.framework/Headers
INSTALLDIR = /Applications/Max5/Cycling\ \'74/externals/
XTRACT_HEADERS = /usr/local/include
FFTW_LIBS = -lfftw3f #include this if you compiled libxtract with --enable-fft

LIPO = /usr/bin/lipo
CC = gcc

#DEBUG_FLAGS = -Werror -Wall

CFLAGS = -F$(FRAMEWORKS) -Wmost -Wno-four-char-constants -Wno-unknown-pragmas -x c -pipe -Wno-trigraphs -fpascal-strings -fasm-blocks -Os -fmessage-length=0 -I$(MAXINCLUDE) -I$(MSPINCLUDE) -include macho-prefix.pch $(DEBUG_FLAGS) -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5 -I$(XTRACT_HEADERS)

I386CFLAGS = -arch i386 
IX86_64CFLAGS = -arch x86_64
PPCCFLAGS = -arch ppc -faltivec -fasm-blocks -Wno-unused

LDFLAGS = -F$(FRAMEWORKS) -L$(MAXINCLUDE) -L$(MSPINCLUDE) -framework Carbon -framework MaxAPI -framework MaxAudioAPI -Wl,-Y,1455 -bundle -L/usr/local/lib -lxtract $(FFTW_LIBS)

I386LDFLAGS = -arch i386
IX86_64CFLAGS = -arch x86_64
PPCLDFLAGS = -arch ppc

universal: 
	$(CC) $(CFLAGS) $(I386CFLAGS) -o $(NAME)-i386.o -c $(NAME).c
	$(CC) $(CFLAGS) $(PPCCFLAGS) -o $(NAME)-ppc.o -c $(NAME).c
	$(CC) $(LDFLAGS) $(I386LDFLAGS) -o $(NAME)-i386 $(NAME)-i386.o
	$(CC) $(LDFLAGS) $(PPCLDFLAGS) -o $(NAME)-ppc $(NAME)-ppc.o
	$(LIPO) -create $(NAME)-ppc $(NAME)-i386 -output $(NAME)
	mkdir -p $(NAME).mxo/Contents/MacOS
	cp Info.plist PkgInfo $(NAME).mxo/Contents/
	cp $(NAME) $(NAME).mxo/Contents/MacOS

ppc:
	$(CC) $(CFLAGS) $(PPCCFLAGS) -o $(NAME)-ppc.o -c $(NAME).c
	$(CC) $(LDFLAGS) $(PPCLDFLAGS) -o $(NAME)-ppc $(NAME)-ppc.o
	$(LIPO) -create $(NAME)-ppc -output $(NAME)
	mkdir -p $(NAME).mxo/Contents/MacOS
	cp Info.plist PkgInfo $(NAME).mxo/Contents/
	cp $(NAME) $(NAME).mxo/Contents/MacOS

intel:
	$(CC) $(CFLAGS) $(I386CFLAGS) -o $(NAME)-i386.o -c $(NAME).c
	$(CC) $(LDFLAGS) $(I386LDFLAGS) -o $(NAME)-i386 $(NAME)-i386.o
	$(LIPO) -create $(NAME)-i386 -output $(NAME)
	mkdir -p $(NAME).mxo/Contents/MacOS
	cp Info.plist PkgInfo $(NAME).mxo/Contents/
	cp $(NAME) $(NAME).mxo/Contents/MacOS

intel64:
	$(CC) $(CFLAGS) $(IX86_64CFLAGS) -o $(NAME)-64.o -c $(NAME).c
	$(CC) $(LDFLAGS) $(IX86_64LDFLAGS) -o $(NAME)-64 $(NAME)-64.o
	$(LIPO) -create $(NAME)-64 -output $(NAME)
	mkdir -p $(NAME).mxo/Contents/MacOS
	cp Info.plist PkgInfo $(NAME).mxo/Contents/
	cp $(NAME) $(NAME).mxo/Contents/MacOS



clean:
	rm -rf *64* *i386* *ppc* xtract~ *.mxo

install:
	$(INSTALL) cp -r $(NAME).mxo $(INSTALLDIR)

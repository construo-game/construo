all : construo

construo : Construo.cxx Construo.hxx DeltaManager.hxx Makefile Config.hxx Config.cxx
	g++ -O2 -Wall `xml2-config --cflags`  Config.cxx Construo.cxx -o construo -lclanDisplay -lclanApp -lclanCore `xml2-config --libs`

construo-static : Construo.cxx Construo.hxx DeltaManager.hxx Makefile Config.hxx Config.cxx
	g++ -O2 -Wall Config.cxx Construo.cxx -o construo-static libclanstatic.a libHermes.a \
-L/usr/X11R6/lib/ \
-static \
-lxml2 \
-lpthread \
-lm \
-lbz2 \
-ldl \
-lz \
-ltiff \
-ljpeg \
-lpng \
-lXext \
-lXt \
-lX11 \
-lSM \
-lICE \
-lXxf86vm
	strip construo-static

release: dist dist-static

dist:
	rm -rf construo-0.0.2
	mkdir -p construo-0.0.2
	cp -i *.cxx *.hxx README COPYING INSTALL Makefile construo-0.0.2/
	tar czf construo-0.0.2.tar.gz construo-0.0.2/

dist-static: construo-static
	rm -rf construo-0.0.2-binary-i386	
	mkdir -p construo-0.0.2-binary-i386
	cp -i construo-static construo-0.0.2-binary-i386/construo
	cp -i README COPYING construo-0.0.2-binary-i386
	tar czf construo-0.0.2-binary-i386.tar.gz construo-0.0.2-binary-i386/	

clean :
	rm -f construo *.o core

# EOF #

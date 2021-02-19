
SOURCE := src/spacy/attrs.cpp \
          src/spacy/nlp.cpp \
          src/spacy/python.cpp \
          src/spacy/span.cpp \
          src/spacy/doc.cpp \
          src/spacy/pyobjectptr.cpp \
          src/spacy/spacy.cpp \
          src/spacy/stringstore.cpp \
          src/spacy/token.cpp \
          src/spacy/vocab.cpp

OBJ := obj/attrs.o obj/nlp.o obj/python.o obj/span.o obj/doc.o obj/pyobjectptr.o \
       obj/spacy.o obj/stringstore.o obj/token.o obj/vocab.o

# build from https://github.com/datastax/cpp-driver.git;  cmake .. -DCASS_BUILD_STATIC='OFF'
CPLUS_INCLUDE_PATH := /usr/lib/x86_64-linux-gnu

C_INCLUDE_PATH := /usr/include/python3.8

.PHONY : build clean test debug


# build and package the software into dist
build :
	mkdir -p obj
	rm -f *.o obj/*.o *.so
	gcc -I$(C_INCLUDE_PATH) $(SOURCE) -c -fPIC
	mv *.o ./obj/
	gcc -shared -o spacy.so -Wl,-rpath,$(LIB_PATH) -lm -lpthread -lstdc++ $(OBJ)
	#cp *.so ../../../resources/libs/
	#cp setup/* ../../../resources/libs/

clean :
	rm -f *.o obj/*.o spacy.so

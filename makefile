SOURCE := $(wildcard *.cpp)

TARGET := ser
CC     := g++
LIBS   := -lpthread
CFLAGS := -std=c++11 -g -Wall
CFLAGS2 := -std=c++11 -g -Wall

#clean :
#	rm -fr *.o
clean : clean
	rm $(TARGET)

$(TARGET) : $(SOURCE)
	$(CC) $(CFLAGS2) $(SOURCE) -o $(TARGET) $(LIBS)
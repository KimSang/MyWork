.SUFFIXES : .cpp .o
CC=g++
CPPFLAGS = -std=c++11 
OBJS=main.o block.o blockchain.o sha256.o
TARGET = TestChain 

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(CPPFLAGS) $(OBJS)

main.o: blockchain.h main.cpp 
	$(CC) $(CPPFLAGS) -c main.cpp -o main.o

block.o: block.cpp block.h sha256.h
	$(CC) $(CPPFLAGS) -c block.cpp -o block.o

blockchain.o: blockchain.h
	$(CC) $(CPPFLAGS) -c blockchain.cpp -o blockchain.o 

sha256.o: sha256.h
	$(CC) $(CPPFLAGS) -c sha256.cpp 	

clean:
	rm -f *.o
	rm -f $(TARGET)

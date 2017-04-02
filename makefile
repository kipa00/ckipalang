CC = g++
TARGET = kipa kipac

all : $(TARGET)

$(TARGET) :
	$(CC) $@.cpp -o $@ -O2

clean :
	rm $(TARGET)

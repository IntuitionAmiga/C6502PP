CC      := g++
CFLAGS  := -Wall -Wextra -flto -march=native -falign-labels=16 -Ofast --std=c++20 -I./include -fconcepts-diagnostics-depth=2 -fcf-protection=none
TARGET  := test
SRCS    := main.cpp
OBJS    := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

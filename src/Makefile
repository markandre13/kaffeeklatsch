APP=tests

MEM=-fsanitize=address -fsanitize=leak -g

CXX=/usr/local/opt/llvm/bin/clang++
CFLAGS=-std=c++23 $(MEM) -O0 -g \
	-Wall \
	-Wextra \
	-Wno-deprecated-anon-enum-enum-conversion \
	-Werror=return-type \
	-Werror=shadow \
	-Werror=suggest-override \
	-Wno-unused-parameter \
	-I/usr/local/opt/llvm/include/c++

LDFLAGS=-L/usr/local/opt/llvm/lib/c++ -Wl,-rpath,/usr/local/opt/llvm/lib/c++ \
	-L/usr/local/lib $(MEM)

SRC = kaffeeklatsch.spec.cc \
	main.cc kaffeeklatsch.cc

OBJ = $(SRC:.cc=.o)

.SUFFIXES: .cc .o

all: $(APP)

depend:
	makedepend -I. -Y $(SRC)

run:
	./$(APP)

clean:
	rm -f $(OBJ)

$(APP): $(OBJ)
	@echo "linking..."
	$(CXX) $(LDFLAGS) $(PROTOBUF_LDFLAGS) $(MEDIAPIPE_CPP_LDFLAGS) $(LIB) $(OBJ) -o $(APP)

.cc.o:
	@echo compiling $*.cc ...
	$(CXX) $(PROTOBUF_FLAGS) $(CFLAGS) $(WSLAG_FLAGS) $(OPENCV_FLAGS) \
	-c -o $*.o $*.cc

# DO NOT DELETE

kaffeeklatsch.spec.o: kaffeeklatsch.hh
main.o: kaffeeklatsch.hh
kaffeeklatsch.o: kaffeeklatsch.hh

CPP=g++
CFLAGS=-W -Wall -Wno-unused-parameter -Wno-unused-variable
LDFLAGS=-lspider -lpthread
EXEC=stereo

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))


all: $(EXEC)

obj:
	@mkdir obj

$(EXEC): $(OBJ_FILES)
	@echo "Building $@"
	@$(CPP) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.cpp obj
	@echo "Building $<"
	@$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf obj $(EXEC) *.gv *.pgantt *.dat result.ppm

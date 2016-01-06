CPP=g++
CFLAGS=-W -Wall -Wno-unused-parameter -Wno-unused-variable -Iinclude -Igenerated
LDFLAGS=-lspider -lpthread
EXEC=stereo

CPP_FILES := $(wildcard src/*.cpp) $(wildcard generated/*.cpp)
OBJ_FILES := $(addprefix obj/,$(CPP_FILES:.cpp=.o))


all: $(EXEC)

obj:
	@mkdir obj

$(EXEC): $(OBJ_FILES)
	@echo "Building $@"
	@$(CPP) -o $@ $^ $(LDFLAGS)

obj/%.o: %.cpp obj
	@echo "Building $<"
	@mkdir -p $(dir $@)
	@$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf obj $(EXEC) *.gv *.pgantt *.dat result.ppm

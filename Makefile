# Makefile

# Get the include path for Erlang headers
ERLANG_PATH = $(shell erl -eval 'io:format("~s", [lists:concat([code:root_dir(), "/erts-", erlang:system_info(version), "/include"])])' -s init stop -noshell)

# Define path to the FIT SDK
FIT_SDK_DIR = c_src/fit_sdk/cpp

# Use g++ for C++ compilation
CXX = g++

# Define all C++ source files: your nif.cpp + all .cpp files in the SDK
SOURCES = c_src/nif.cpp $(wildcard $(FIT_SDK_DIR)/*.cpp)

# Define compiler flags, including include paths for Erlang and the FIT SDK
# -fPIC is necessary for creating a shared library
CXXFLAGS = -fPIC -I"$(ERLANG_PATH)" -I"$(FIT_SDK_DIR)"

# Define linker flags for creating the dynamic library on macOS
LDFLAGS = -undefined dynamic_lookup -dynamiclib

# Define the output file
OUTPUT = priv/nif.so

# Default target
all: $(OUTPUT)

# Rule to build the output file
$(OUTPUT): $(SOURCES)
	@mkdir -p priv
	$(CXX) $(LDFLAGS) -o $@ $^ $(CXXFLAGS)

# Rule to clean up build artifacts
clean:
	rm -f "$(OUTPUT)"

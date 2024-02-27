TARGET_EXEC := libcod1

CONSTANTS = -D COD_VERSION=COD1_1_1

BUILD_DIR = ./build
SRC_DIRS = ./src

SRC_FILES += cracking.cpp gsc.cpp libcod.cpp lib/qvsnprintf.c lib/strcmp_constant_time.c


## default values
DEBUG ?= 0
COMPILE_PLAYER ?= 1
COMPILE_ENTITY ?= 1
COMPILE_SQLITE ?= 1
COMPILE_UTILS ?= 1
ENABLE_UNSAFE ?= 1


## defines
ifeq ($(DEBUG),1)
DEBUG_FLAGS += -g -ggdb -O0
endif

ifeq ($(COMPILE_PLAYER),1)
SRC_FILES += gsc_player.cpp
CUSTOMDEFINES += -D COMPILE_PLAYER=1
else
CUSTOMDEFINES += -D COMPILE_PLAYER=0
endif

ifeq ($(COMPILE_ENTITY),1)
SRC_FILES += gsc_entity.cpp
CUSTOMDEFINES += -D COMPILE_ENTITY=1
else
CUSTOMDEFINES += -D COMPILE_ENTITY=0
endif

ifeq ($(COMPILE_SQLITE),1)
SRC_FILES += gsc_sqlite.cpp
LDFLAGS += -lsqlite3
CUSTOMDEFINES += -D COMPILE_SQLITE=1
else
CUSTOMDEFINES += -D COMPILE_SQLITE=0
endif

ifeq ($(COMPILE_UTILS),1)
SRC_FILES += gsc_utils.cpp
CUSTOMDEFINES += -D COMPILE_UTILS=1
else
CUSTOMDEFINES += -D COMPILE_UTILS=0
endif

ifeq ($(ENABLE_UNSAFE),1)
CUSTOMDEFINES += -D COMPILE_EXEC=1 -D COMPILE_MEMORY=1 -D ENABLE_UNSAFE=1
else
CUSTOMDEFINES += -D COMPILE_EXEC=0 -D COMPILE_MEMORY=0
endif


CPPFLAGS := -I. -m32 -fPIC -Wall -fvisibility=hidden $(CONSTANTS) $(DEBUG_FLAGS) $(CUSTOMDEFINES)
LDFLAGS += -m32 -shared -L/lib32 -ldl -lpthread

SRCS := $(addprefix $(SRC_DIRS)/,$(SRC_FILES))

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)


# The final build step.
.PHONY: libcod1
.libcod1:
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@.so

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: default
default: libcod1


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)


# sudo apt update
# sudo apt install --no-install-recommends -y g++-multilib libsqlite3-dev:i386


CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -L/usr/lib -lstdc++ -lm 
STATICLIB:= ../../jjj_orig/build/lib/libjjj.a
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := mqsp
INCLUDE  := -Iinclude -I../jjj_orig/include
SRC      :=                                                 \
cpp/mqsProvider/Controller.cpp                              \
cpp/mqsProvider/ListClient.cpp                              \
cpp/mqsProvider/ListServer.cpp                              \
cpp/mqsProvider/main.cpp                                    \
cpp/mqsProvider/MessageQueueClient.cpp                      \
cpp/mqsProvider/MessageQueueServer.cpp                      \
cpp/mqsProvider/MessageQueueServiceClient.cpp               \
cpp/mqsProvider/MessageQueueServiceServer.cpp               \
cpp/mqsProvider/MutualExclusionClient.cpp                   \
cpp/mqsProvider/MutualExclusionServer.cpp                   \
cpp/mqsProvider/PosixBinSemaphoreClient.cpp                 \
cpp/mqsProvider/PosixBinSemaphoreServer.cpp                 \
cpp/mqsProvider/PosixSharedMemoryAccess.cpp                 \
cpp/mqsProvider/PosixSharedMemoryAllocator.cpp              \
cpp/mqsProvider/PrivateSharedMemoryAccess.cpp               \
cpp/mqsProvider/PrivateSharedMemoryAllocator.cpp            \
cpp/mqsProvider/Publisher.cpp                               \
cpp/mqsProvider/ServerSession.cpp                           \
cpp/mqsProvider/Session.cpp                                 \
cpp/mqsProvider/SharedMemoryAccess.cpp                      \
cpp/mqsProvider/SharedMemoryAllocator.cpp                   \
cpp/mqsProvider/Subscriber.cpp                              \
cpp/mqsProvider/TreeClient.cpp                              \
cpp/mqsFactory/MutualExclusionClientFactory.cpp             \
cpp/mqsFactory/SharedMemoryAccessFactory.cpp


OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(APP_DIR)/$(TARGET) $(OBJECTS) $(STATICLIB)

.PHONY: all build clean debug release

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*


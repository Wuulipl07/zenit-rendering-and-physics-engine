# zenit makefile

CXX      := g++
TARGET   := zenit
SRC_DIR  := src
BUILD    := build

CXXFLAGS := -std=c++17 -Wall -Wextra -I$(SRC_DIR) -DGLFW_INCLUDE_NONE -DZENIT_ROOT=\"$(CURDIR)\"
LDLIBS   := -lglfw -lGL

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all debug release run clean

all: debug

debug: CXXFLAGS += -g -O0
debug: $(BUILD)/$(TARGET)

release: CXXFLAGS += -O2 -DNDEBUG
release: clean $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDLIBS)

$(BUILD)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: debug
	./$(BUILD)/$(TARGET)

clean:
	rm -rf $(BUILD)

-include $(DEPS)

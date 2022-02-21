# Debug Build Configuration

TARGET     := code
BLDDIR     := debug

CPP        := g++
CPPFLAGS   := -std=c++20 -g -Og -D DEBUG -MMD -MP -Wall
IMGUIFLAGS := -std=c++20 -O3 -MMD -MP
LD         := g++
LDFLAGS    := -Wl,--gc-sections -pthread -lvulkan -lglfw

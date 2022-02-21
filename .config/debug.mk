# Debug Build Configuration

TARGET   := code
BLDDIR   := debug

CPP      := g++
CPPFLAGS := -std=c++20 -g -Og -D DEBUG -MMD -MP -Wall
LD       := g++
LDFLAGS  := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

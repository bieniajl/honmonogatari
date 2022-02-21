# Release Build Configuration

TARGET   := honmono
BLDDIR   := release

CPP      := g++
CPPFLAGS := -std=c++20 -O3 -MMD -MP -Werror
LD       := g++
LDFLAGS  := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -O3

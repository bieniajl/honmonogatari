# Release Build Configuration

TARGET     := honmono
BLDDIR     := release

CPP        := g++
CPPFLAGS   := -std=c++23 -O3 -MMD -MP -Werror
IMGUIFLAGS := -std=c++23 -O3 -MMD -MP
LD         := g++
LDFLAGS    := -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -O3

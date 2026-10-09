# Nova Studio – Win32 IDE for NovaLang
# Build with MinGW (MSYS2): make

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Wno-missing-field-initializers \
           -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX \
           -D_WIN32_WINNT=0x0601 -municode
LDFLAGS  = -mwindows -lcomctl32 -lcomdlg32 -lshlwapi -lole32 \
           -ldwmapi -lshell32 -luser32 -lgdi32

SRCS = src/main.cpp src/editor.cpp src/explorer.cpp src/highlighter.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = nova-studio.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LDFLAGS)

clean:
	del /Q $(TARGET) 2>nul || true
	del /Q src\*.o 2>nul || true

.PHONY: all clean

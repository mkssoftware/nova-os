# Nova Studio 2.0 – Win32 IDE for NovaLang
# Build: make   (requires MSYS2 ucrt64 g++)
# Standalone: statisch gelinkt, keine MSYS2-Laufzeit-DLLs nötig

CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra \
           -Wno-missing-field-initializers \
           -Wno-unused-parameter \
           -DUNICODE -D_UNICODE \
           -DWIN32_LEAN_AND_MEAN -DNOMINMAX \
           -D_WIN32_WINNT=0x0601 \
           -municode

# -static-libgcc / -static-libstdc++ : GCC/C++ Runtime einbetten
# Kein -lwinpthread: pthread_win32.cpp stellt Win32-native Implementierung
# bereit → keine libwinpthread-1.dll Abhängigkeit, kein clock_gettime64-Fehler
LDFLAGS  = -mwindows \
           -static-libgcc -static-libstdc++ \
           -lcomctl32 -lcomdlg32 -lshlwapi \
           -lole32 -ldwmapi -lshell32 \
           -luser32 -lgdi32 -luxtheme

SRCS = src/main.cpp \
       src/editor.cpp \
       src/explorer.cpp \
       src/highlighter.cpp \
       src/dialogs.cpp \
       src/ribbon.cpp \
       src/pthread_win32.cpp

TARGET = nova-studio.exe

all: $(TARGET)

$(TARGET): $(SRCS) src/studio.h
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS) $(LDFLAGS)
	@echo "Gebaut: $(TARGET)"

clean:
	-rm -f $(TARGET)

.PHONY: all clean

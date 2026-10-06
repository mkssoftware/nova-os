# NPSPEC-COMPAT-WIN32-0001 – Nova Win32 Compatibility

## Status

Angenommen

## Kategorie

Compatibility / Win32

## Zweck

NovaOS definiert eine Win32-Kompatibilitätsschicht für klassische Windows-Anwendungen.

Win32-Anwendungen werden über eine Windows Compatibility Personality ausgeführt. Win32-APIs, Windows-ABIs, PE-Binärdateien und Windows-Systemkonventionen werden auf native NovaOS-Mechanismen abgebildet, ohne Windows-Systemstrukturen Bestandteil der nativen NovaOS-Architektur werden zu lassen.

## Grundprinzipien

```text
Win32 Compatibility ≠ Windows
Win32 API ≠ Nova API
Windows ABI ≠ Nova ABI
Windows Handle ≠ Nova Handle
Windows Path ≠ Nova Object Identity
Windows Registry ≠ Nova Registry
Administrator ≠ Nova Authority
```

## Modell

```text
Windows Application
        ↓
Windows Personality
        ↓
Win32 API
        ↓
Windows ABI / Runtime
        ↓
Compatibility Translation
        ↓
Nova System Interfaces
        ↓
Capabilities / Policy
        ↓
NovaOS
```

## Binärprogramme

Win32-Anwendungen werden anhand ihres Binärformats und ihrer ABI-Anforderungen erkannt.

Typische Formate:

```text
.exe
.dll
PE / PE+
```

```text
PE Binary
    ↓
Architecture / ABI Detection
    ↓
Win32 Compatibility Provider
    ↓
Loader
    ↓
Nova Process
```

## Win32 API

Win32-Aufrufe werden durch Compatibility Provider auf native NovaOS-Funktionen abgebildet.

Dazu können gehören:

```text
Process / Thread
File I/O
Memory
Synchronization
Networking
Windowing
Graphics
Input
Audio
Printing
Clipboard
Configuration
COM
```

Nicht unterstützte Funktionen dürfen kontrolliert emuliert oder als nicht verfügbar gemeldet werden.

## Windows Handles

Windows-Handles werden innerhalb des Compatibility-Kontexts verwaltet:

```text
Windows HANDLE
      ↓
Compatibility Handle Table
      ↓
Authorized Nova Handle
```

Ein Windows-Handle darf keine größere Authority besitzen als die zugrunde liegende NovaOS-Ressource.

## Dateisystem

Windows-Pfade werden auf den NovaOS-Namespace projiziert:

```text
C:\Users\...
      ↓
Windows Namespace View
      ↓
Nova Namespace Resolution
      ↓
ObjectID
```

Laufwerksbuchstaben sind ausschließlich Teil der Compatibility-Ansicht und werden nicht in das native NovaOS-Volumemodell übernommen.

Windows-spezifische Regeln wie Laufwerksbuchstaben, Pfadseparatoren und Case-Verhalten werden innerhalb der Personality behandelt.

## Windows Registry

Die Windows Registry wird als Compatibility-Konfigurationsraum bereitgestellt.

```text
Windows Registry API
        ↓
Compatibility Registry
        ↓
Isolated Configuration State
```

Sie ist von der nativen NovaOS-System- und Capability-Registry getrennt.

## DLLs und Runtime

Windows-Anwendungen dürfen kompatible Laufzeitkomponenten verwenden:

```text
DLLs
Runtime Libraries
Application Libraries
Compatibility Libraries
```

Diese bleiben innerhalb der Windows-Compatibility-Umgebung und ersetzen keine nativen NovaOS-Systembibliotheken.

## GUI

Win32-Fenster, Eingaben und Grafikoperationen werden auf native NovaOS-Grafik- und Desktopdienste abgebildet.

```text
Win32 GUI
    ↓
Window / Graphics Compatibility
    ↓
Nova UI / Graphics Services
```

Die Anwendung darf dadurch als reguläres Fenster in die NovaOS-Oberfläche integriert werden.

## Prozesse und IPC

Windows-Konzepte wie:

```text
Processes
Threads
Events
Mutexes
Named Pipes
Shared Memory
Sockets
COM
```

werden auf geeignete NovaOS-Prozess-, Synchronisations- und IPC-Mechanismen abgebildet.

## Sicherheit

Windows-Benutzer-, Token-, ACL- und Administrator-Konzepte werden als Compatibility-Semantik behandelt.

```text
Windows Access Request
        ↓
Win32 Compatibility
        ↓
Nova Capability / Policy
        ↓
Effective Authority
```

Ein Windows-Administratorprozess erhält keine universelle NovaOS-Authority.

## Isolation

Windows-spezifischer Zustand bleibt von NovaOS getrennt:

```text
Registry State
Compatibility Files
Runtime State
Temporary Data
DLL State
Application Configuration
```

Mehrere Compatibility-Umgebungen dürfen voneinander isoliert betrieben werden.

## Kompatibilitätszustand

Win32-Funktionen können gekennzeichnet werden als:

```text
NativeMapping
Translated
Emulated
PartiallySupported
Unsupported
Unavailable
```

## Normative Anforderungen

1. Win32 MUSS als Compatibility-Schicht und nicht als native NovaOS-API behandelt werden.
2. PE-Binärprogramme MÜSSEN anhand ihrer Architektur und ABI-Anforderungen analysierbar sein.
3. Win32-Aufrufe MÜSSEN über kontrollierte Compatibility Provider verarbeitet werden.
4. Windows-Handles MÜSSEN auf kontrollierte NovaOS-Ressourcen und Handles abgebildet werden.
5. Windows-Pfade MÜSSEN über eine isolierte Namespace-Projektion auflösbar sein.
6. Laufwerksbuchstaben DÜRFEN das native NovaOS-Volumemodell nicht verändern.
7. Die Windows Registry MUSS von der nativen NovaOS-Registry getrennt bleiben.
8. Windows-DLLs DÜRFEN native NovaOS-Systembibliotheken nicht unkontrolliert ersetzen.
9. Win32-GUI-Anwendungen MÜSSEN über kontrollierte Grafik- und Desktopprovider integrierbar sein.
10. Windows-IPC MUSS auf kontrollierte NovaOS-IPC-Mechanismen abgebildet werden.
11. Windows-Sicherheitskonzepte DÜRFEN das NovaOS-Capability-Modell nicht umgehen.
12. Windows-Administratorrechte DÜRFEN keine universelle NovaOS-Authority erzeugen.
13. Nicht unterstützte Win32-Funktionen MÜSSEN kontrolliert fehlschlagen oder degradieren.
14. Compatibility-Zustand, verwendete Provider und Emulationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-PROGRAM-COMPATIBILITY-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-RESOLUTION-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann klassische Windows-Anwendungen über eine isolierte Win32-Kompatibilitätsschicht ausführen. Win32-APIs, PE-Binärdateien, DLLs, Windows-Pfade, Registry, Handles, GUI und IPC werden kontrolliert auf native NovaOS-Mechanismen abgebildet, während NovaOS-Namespace, Capabilities, Sicherheit und Systemarchitektur maßgeblich bleiben.
# NPSPEC-COMPAT-UBUNTU-0001 – Nova Ubuntu Compatibility

## Status

Angenommen

## Kategorie

Compatibility / Ubuntu

## Zweck

NovaOS definiert eine Ubuntu-Kompatibilitätsumgebung für Software, die neben Linux-Kompatibilität zusätzlich Ubuntu-spezifische Laufzeiten, Bibliotheken, Paketstrukturen und Systemkonventionen erwartet.

Ubuntu-Kompatibilität baut auf der Linux- und POSIX-Personality auf und bleibt vollständig von der nativen NovaOS-Systemarchitektur getrennt.

## Grundprinzipien

```text
Ubuntu Compatibility ≠ Ubuntu Distribution
Ubuntu Personality ≠ Linux Kernel
Ubuntu Package ≠ Nova Package
APT/dpkg ≠ Nova Package Manager
Ubuntu root ≠ Nova Authority
Ubuntu Filesystem ≠ Nova Namespace
Ubuntu Service ≠ Native Nova Service
```

## Modell

```text
Ubuntu Application
       ↓
Ubuntu Personality
       ↓
Linux Personality
       ↓
POSIX Compatibility
       ↓
Compatibility ABI
       ↓
Nova System Interfaces
       ↓
NovaOS
```

Ubuntu-spezifische Funktionen erweitern die Linux-Personality, duplizieren sie jedoch nicht.

## Ubuntu-Profil

Eine Ubuntu-Personality wird mindestens beschrieben durch:

```text
UbuntuProfile
├── Release
├── Architecture
├── LinuxABI
├── RuntimeEnvironment
├── LibrarySet
├── PackageEnvironment
└── CompatibilityLevel
```

Mehrere Ubuntu-Releases dürfen parallel unterstützt werden.

## Dateisystemumgebung

Ubuntu-typische Strukturen können als isolierte Namespace-Projektion bereitgestellt werden:

```text
/
├── bin
├── boot
├── dev
├── etc
├── home
├── lib
├── opt
├── proc
├── run
├── sys
├── tmp
├── usr
└── var
```

Diese Struktur verändert nicht den nativen NovaOS-Namespace.

```text
Ubuntu Path
     ↓
Ubuntu Namespace Projection
     ↓
Linux Personality
     ↓
Nova Namespace
     ↓
ObjectID / ResourceID
```

## Bibliotheken und Runtime

Ubuntu-Anwendungen dürfen erwartete Bibliotheken innerhalb ihrer Compatibility-Umgebung verwenden:

```text
glibc
Dynamic Loader
Ubuntu Libraries
Application Libraries
Runtime Dependencies
```

Diese dürfen native NovaOS-Systembibliotheken nicht ersetzen.

## Pakete

Ubuntu-Paketformate dürfen innerhalb der Personality unterstützt werden:

```text
.deb
dpkg
APT Package Metadata
Repositories
```

Ubuntu-Pakete bleiben Compatibility-Pakete und werden nicht zu nativen NovaOS-Paketen.

```text
Ubuntu Package
      ↓
Compatibility Package Environment
      ↓
Ubuntu Runtime
```

Paketinstallation darf globale NovaOS-Systembereiche nicht unkontrolliert verändern.

## Paketabhängigkeiten

Ubuntu-Abhängigkeiten werden innerhalb der jeweiligen Ubuntu-Umgebung aufgelöst.

Mehrere Ubuntu-Umgebungen dürfen unterschiedliche Bibliotheks- und Paketversionen besitzen, ohne globale NovaOS-Abhängigkeitskonflikte zu erzeugen.

## Dienste

Ubuntu-Software darf Linux- beziehungsweise Ubuntu-typische Service-Erwartungen besitzen.

Diese können über Compatibility Provider auf NovaOS-Dienste und Prozessmechanismen abgebildet werden.

Ein Ubuntu-Dienst wird dadurch nicht automatisch zu einem nativen NovaOS-Systemdienst.

## Desktop-Anwendungen

Grafische Ubuntu-Anwendungen dürfen über geeignete Compatibility Provider auf NovaOS-Grafik-, Fenster-, Eingabe-, Audio- und Desktopdienste zugreifen.

```text
Ubuntu GUI Application
        ↓
Compatibility Graphics Provider
        ↓
Nova Graphics / UI Services
```

Die Anwendung bleibt dabei innerhalb ihres Compatibility-Kontexts.

## Sicherheit

Ubuntu-Benutzer-, Gruppen- und Root-Konzepte bleiben Compatibility-Semantik.

```text
Ubuntu root
      ↓
Compatibility Mapping
      ↓
Nova Capability / Policy
      ↓
Effective Authority
```

`root` innerhalb einer Ubuntu-Umgebung besitzt keine universelle NovaOS-Authority.

## Isolation

Ubuntu-spezifische Zustände bleiben von NovaOS getrennt:

```text
/etc
/var
Package Database
Runtime State
Service State
Temporary Data
User Environment
```

Mehrere Ubuntu-Personalities dürfen voneinander isolierte Zustände besitzen.

## Versionierung

Ubuntu-Releases werden als unterschiedliche Compatibility-Profile behandelt:

```text
Ubuntu Personality
├── Release A
├── Release B
└── Release C
```

Anwendungen können gegen ein passendes Profil aufgelöst werden.

## Normative Anforderungen

1. Ubuntu-Kompatibilität MUSS auf der Linux-Personality aufbauen.
2. Ubuntu-spezifische Funktionen DÜRFEN die Linux-Kompatibilität erweitern, aber nicht die native NovaOS-Architektur ersetzen.
3. Ubuntu-Releases MÜSSEN als versionierte Compatibility-Profile darstellbar sein.
4. Ubuntu-Dateisystemstrukturen MÜSSEN als isolierte Namespace-Projektionen realisierbar sein.
5. Ubuntu-Bibliotheken DÜRFEN native NovaOS-Systembibliotheken nicht unkontrolliert ersetzen.
6. `.deb`-Pakete MÜSSEN innerhalb einer Compatibility-Paketumgebung verarbeitet werden können.
7. Ubuntu-Paketabhängigkeiten MÜSSEN von nativen NovaOS-Abhängigkeiten getrennt bleiben.
8. Ubuntu-Paketinstallationen DÜRFEN globale NovaOS-Systembereiche nicht unkontrolliert verändern.
9. Ubuntu-Dienste DÜRFEN native NovaOS-Systemdienste nicht automatisch ersetzen.
10. Grafische Ubuntu-Anwendungen MÜSSEN über kontrollierte Compatibility Provider integrierbar sein.
11. Ubuntu `root` DARF keine universelle NovaOS-Authority erzeugen.
12. Alle geschützten Operationen MÜSSEN weiterhin NovaOS-Capability- und Policy-Prüfungen unterliegen.
13. Mehrere Ubuntu-Versionen MÜSSEN parallel isoliert unterstützt werden können.
14. Ubuntu-Profil, Release, Abhängigkeiten und Compatibility-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-POSIX-0001`
- `NPSPEC-COMPAT-LINUX-0001`
- `NPSPEC-PROGRAM-LEGACY-0001`
- `NPSPEC-NAMESPACE-PROCESS-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-NAMESPACE-VIRTUAL-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`

## Ergebnis

NovaOS kann Ubuntu-Anwendungen über eine auf Linux und POSIX aufbauende Ubuntu Compatibility Personality ausführen. Ubuntu-spezifische Releases, Bibliotheken, Pakete, Dateisystemstrukturen und Dienste bleiben dabei in einer isolierten Compatibility-Umgebung, während NovaOS-Namespace, Capabilities, Sicherheit und native Systemkomponenten maßgeblich bleiben.
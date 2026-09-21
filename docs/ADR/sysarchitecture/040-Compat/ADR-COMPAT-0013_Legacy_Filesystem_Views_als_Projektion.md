# ADR-COMPAT-0013 – Legacy Filesystem Views als Projektion

## Status

Angenommen

## Kategorie

Compatibility / Filesystem / Legacy / Projection

## Kontext

Legacy-Anwendungen erwarten häufig bestimmte Dateisystemstrukturen.

Beispiele:

```text
POSIX: /
Linux: /proc, /sys, /dev, /home
Win32: C:\, D:\, Program Files, AppData
```

NovaOS verwendet dagegen eigene Storage-, Object- und Volume-Konzepte mit stabiler objektbasierter Identität.

Legacy-Dateisystemmodelle dürfen deshalb nicht die native Storage-Architektur bestimmen.

## Entscheidung

NovaOS stellt **Legacy Filesystem Views als Projektionen auf native NovaOS-Objekte und Ressourcen** bereit.

```text
Native Nova Objects
        ↓
Projection Layer
        ↓
Legacy Filesystem View
        ↓
Legacy Application
```

Die Legacy-Anwendung sieht die erwartete Struktur, während NovaOS intern sein natives Objekt- und Storage-Modell beibehält.

## Projektion

Eine Projektion ordnet native Ressourcen einer Legacy-Darstellung zu.

Beispiel:

```text
Nova Object
    ↓
Projection
    ↓
C:\Users\User\Document.txt
```

oder:

```text
Nova Object
    ↓
Projection
    ↓
/home/user/document.txt
```

Der Legacy-Pfad ist dabei eine Ansicht und nicht die eigentliche Objektidentität.

## Objektidentität

Native Ressourcen behalten ihre stabile NovaOS-Identität.

```text
Object ID
   ↓
Multiple Views
   ├── Nova View
   ├── POSIX View
   ├── Linux View
   └── Win32 View
```

Dasselbe Objekt kann dadurch in unterschiedlichen Compatibility Domains unterschiedlich dargestellt werden.

## Virtuelle Strukturen

Legacy-spezifische Strukturen können vollständig virtuell erzeugt werden.

Beispiele:

```text
/proc
/sys
/dev

C:\Windows
C:\ProgramData
Registry-backed Paths
```

Eine sichtbare Legacy-Struktur muss keinem identischen physischen NovaOS-Verzeichnis entsprechen.

## Domain-spezifische Views

Filesystem Views werden an den jeweiligen Compatibility Context gebunden.

```text
Application A → Linux View
Application B → Win32 View
Application C → Native Nova View
```

Dadurch müssen Legacy-Namensräume nicht global im System existieren.

## Zugriffskontrolle

Eine Projektion erzeugt keine zusätzlichen Rechte.

```text
Visible Path
    ≠
Authorized Access
```

Der tatsächliche Zugriff bleibt durch NovaOS-Capabilities, Permissions und Security Policies bestimmt.

## Änderungen

Schreiboperationen über eine Legacy View werden auf das zugrunde liegende NovaOS-Objekt übertragen, sofern dies erlaubt ist.

```text
Legacy Write
     ↓
Projection
     ↓
Authorization
     ↓
Native Object Operation
```

Read-only oder virtuelle Projektionen können Schreibzugriffe explizit ablehnen.

## Normative Anforderungen

1. Legacy-Dateisysteme SOLLEN als Projektionen auf native NovaOS-Ressourcen dargestellt werden.
2. Legacy-Pfade DÜRFEN nicht zur nativen Objektidentität werden.
3. Native Object IDs MÜSSEN unabhängig von der dargestellten Legacy View bleiben.
4. Dasselbe Objekt MUSS in unterschiedlichen Views unterschiedlich darstellbar sein.
5. Virtuelle Legacy-Strukturen MÜSSEN ohne entsprechende physische Verzeichnisstruktur möglich sein.
6. Filesystem Views SOLLEN an Compatibility Domains gebunden werden können.
7. Sichtbarkeit innerhalb einer Projektion DARF keine zusätzliche Autorität erzeugen.
8. Schreibzugriffe MÜSSEN weiterhin die nativen NovaOS-Sicherheitsregeln durchlaufen.
9. Änderungen der Legacy-Darstellung DÜRFEN keine Änderung der nativen Storage-Architektur erzwingen.

## Konsequenzen

### Positiv

- Legacy-Anwendungen erhalten ihre erwarteten Dateisystemstrukturen,
- NovaOS bleibt intern objekt- und nicht pfadzentriert,
- verschiedene Filesystem-Semantiken können parallel existieren,
- Compatibility Domains können eigene isolierte Views besitzen.

### Negativ

- Projektionen benötigen zusätzliche Mapping-Logik,
- unterschiedliche Pfadsemantiken können Sonderfälle erzeugen,
- Änderungen müssen zwischen View und nativen Objekten korrekt übersetzt werden.

## Abhängigkeiten

- `ADR-COMPAT-0003_Compatibility_Personalities`
- `ADR-COMPAT-0004_POSIX_als_Kompatibilitätsschicht`
- `ADR-COMPAT-0005_Linux_ABI_über_Syscall_Translation`
- `ADR-COMPAT-0006_Win32_über_API_und_ABI_Translation`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-OBJECT-0001`
- `ADR-FILESYSTEM-0001`

## Ergebnis

NovaOS behandelt Legacy-Dateisystemstrukturen als Ansichten:

```text
Native Object Model
        ↓
Projection
        ↓
POSIX / Linux / Win32 View
```

Damit können Legacy-Anwendungen ihre gewohnte Dateisystemstruktur verwenden, ohne dass diese zur internen Storage- und Objektarchitektur von NovaOS wird.
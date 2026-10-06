# NPSPEC-COMPAT-BINARYTRANSLATION-0001 – Nova Binary Translation

## Status

Angenommen

## Kategorie

Compatibility / Binary Translation

## Zweck

NovaOS definiert Binary Translation für die Ausführung von Binärprogrammen, deren Maschinenbefehle nicht direkt auf der aktuellen Prozessorarchitektur ausgeführt werden können.

Binary Translation übersetzt fremden Maschinencode kontrolliert in ausführbaren Code der Zielarchitektur und arbeitet dabei mit Personality-, ABI-, API- und Syscall-Kompatibilität zusammen.

## Grundprinzipien

```text
Binary Translation ≠ ABI Translation
Binary Translation ≠ API Translation
Binary Translation ≠ Emulation des gesamten Systems
Guest Architecture ≠ Host Architecture
Translated Code ≠ Trusted Code
Translation ≠ Authority
```

## Modell

```text
Foreign Binary
      ↓
Binary / Architecture Detection
      ↓
Guest Instructions
      ↓
Binary Translator
      ↓
Host Instructions
      ↓
Compatibility Environment
      ↓
NovaOS
```

## Translation Context

Eine Übersetzungsinstanz beschreibt mindestens:

```text
BinaryTranslation
├── GuestArchitecture
├── HostArchitecture
├── GuestISA
├── HostISA
├── ABIProfile
├── PersonalityID
├── TranslationMode
└── SecurityContext
```

## Übersetzungsmodi

NovaOS darf unterschiedliche Verfahren unterstützen:

```text
Static Translation
Dynamic Translation
Just-in-Time Translation
Cached Translation
Hybrid Translation
```

Der verwendete Modus kann anhand von Kompatibilität, Sicherheit, Performance und Ressourcen gewählt werden.

## Translation Blocks

Maschinencode darf in kontrollierte Translation Blocks zerlegt werden:

```text
Guest Code
   ↓
Decode
   ↓
Validate
   ↓
Intermediate Representation
   ↓
Optimize
   ↓
Generate Host Code
   ↓
Execute
```

Eine interne Zwischendarstellung darf zur Architekturentkopplung verwendet werden.

## Code Cache

Übersetzter Code darf wiederverwendet werden:

```text
Guest Code Identity
       ↓
Translation Cache
       ↓
Validated Host Code
```

Cache-Einträge müssen an relevante Eigenschaften wie Guest-Code-Version, Architektur, Translator-Version und Security Context gebunden sein.

Veränderter Guest-Code muss betroffene Übersetzungen invalidieren.

## Selbstmodifizierender Code

Selbstmodifizierender oder dynamisch erzeugter Code muss erkannt und kontrolliert behandelt werden.

Betroffene Translation Blocks dürfen nicht weiterverwendet werden, wenn ihre Ausgangsinstruktionen verändert wurden.

## Speicher

Guest- und Host-Adressräume bleiben logisch getrennt.

```text
Guest Address
      ↓
Address Translation
      ↓
Validated Mapping
      ↓
Nova Memory
```

Speicherzugriffe müssen weiterhin den NovaOS-Schutzmechanismen unterliegen.

## Systemgrenzen

Systemaufrufe und Plattformoperationen werden nicht allein durch Instruction Translation implementiert.

```text
Translated Code
      ↓
Foreign Syscall / API
      ↓
Compatibility Translation
      ↓
Nova System Interface
```

Binary Translation ergänzt damit ABI-, API- und Syscall-Kompatibilität, ersetzt diese jedoch nicht.

## Sicherheit

Generierter Host-Code wird wie nicht vertrauenswürdiger Compatibility-Code behandelt.

Er darf:

```text
keine Capability erzeugen
keine Policy umgehen
keinen fremden Host-Speicher erreichen
keine Kernelgrenze umgehen
```

Der Translator darf zusätzliche Isolation oder Instrumentierung einsetzen.

## Fehler

Nicht übersetzbare oder ungültige Instruktionen müssen kontrolliert behandelt werden.

Mögliche Zustände:

```text
Translated
PartiallyTranslated
UnsupportedInstruction
InvalidCode
TranslationFault
Unavailable
```

## Normative Anforderungen

1. Guest- und Host-Architektur MÜSSEN eindeutig bestimmt werden.
2. Binary Translation MUSS von ABI-, API- und Syscall-Translation getrennt bleiben.
3. Fremde Instruktionen MÜSSEN vor ihrer Ausführung dekodiert und validiert werden.
4. Übersetzter Code DARF keine zusätzliche Authority erhalten.
5. Guest-Speicherzugriffe MÜSSEN auf kontrollierte NovaOS-Speicherbereiche begrenzt bleiben.
6. Systemaufrufe MÜSSEN weiterhin über die Compatibility-Syscall-Schicht verarbeitet werden.
7. Selbstmodifizierender Code MUSS Translation-Cache-Einträge invalidieren können.
8. Translation Caches MÜSSEN gegen veralteten oder manipulierten Guest-Code abgesichert sein.
9. Nicht unterstützte Instruktionen MÜSSEN kontrolliert fehlschlagen oder emuliert werden.
10. Optimierungen DÜRFEN Sicherheits- und Isolationseigenschaften nicht verändern.
11. Binary Translation SOLL bei kompatibler Hardware vollständig umgangen werden können.
12. Translator, Translation Mode, Guest-/Host-Architektur und Fehlerzustände MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-ABITRANSLATION-0001`
- `NPSPEC-COMPAT-API-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann Binärprogramme fremder Prozessorarchitekturen durch kontrollierte Binary Translation ausführen. Fremde Maschinenbefehle werden auf die Host-Architektur übersetzt, während ABI-, API- und Syscall-Kompatibilität getrennte Schichten bleiben und NovaOS-Speicherschutz, Capabilities, Policies und Isolation weiterhin maßgeblich sind.
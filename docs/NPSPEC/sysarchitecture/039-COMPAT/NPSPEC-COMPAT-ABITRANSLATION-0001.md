# NPSPEC-COMPAT-ABITRANSLATION-0001 – Nova ABI Translation

## Status

Angenommen

## Kategorie

Compatibility / ABI Translation

## Zweck

NovaOS definiert die Übersetzung zwischen fremden binären Schnittstellen und der nativen NovaOS-ABI.

Die ABI Translation überführt Aufrufkonventionen, Registerbelegung, Stack-Layout, Datentypen und binäre Datenstrukturen kontrolliert zwischen unterschiedlichen ABI-Modellen.

## Grundprinzipien

```text
Foreign ABI ≠ Nova ABI
ABI Translation ≠ API Translation
ABI Translation ≠ Syscall Translation
Translation ≠ Authority
Binary Representation ≠ Semantic Identity
Fast Path ≠ Security Bypass
```

## Modell

```text
Foreign Binary
      ↓
Source ABI
      ↓
ABI Translation
      ↓
Target ABI
      ↓
Compatibility Provider / Nova Interface
```

Die Übersetzung darf bidirektional erfolgen:

```text
Foreign → Nova
Nova → Foreign
```

Dies ist insbesondere für Callbacks und fremde Bibliotheken erforderlich.

## Translation Contract

Eine Übersetzung wird durch einen Translation Contract beschrieben:

```text
ABITranslation
├── SourceABI
├── TargetABI
├── Architecture
├── CallingConvention
├── DataLayout
├── RegisterMapping
├── StackMapping
├── TypeMapping
└── ReturnMapping
```

## Calling Convention

Die Translation muss Unterschiede behandeln können bei:

```text
Parameter Passing
Return Values
Registers
Stack Usage
Stack Alignment
Caller/Callee Saved Registers
Variadic Calls
Structure Passing
Floating Point
Vector Registers
```

## Datenlayout

Binäre Datentypen können zwischen ABIs unterschiedlich repräsentiert sein.

Die Translation berücksichtigt insbesondere:

```text
Integer Width
Pointer Width
Alignment
Padding
Structure Layout
Union Layout
Endianness
Boolean Representation
Enumeration Size
```

Eine binäre Konvertierung darf die semantische Bedeutung der Daten nicht verändern.

## Thunks

Für häufige ABI-Grenzen dürfen optimierte Thunks erzeugt oder wiederverwendet werden:

```text
Foreign Call
    ↓
ABI Thunk
    ↓
Nova Call
```

Thunks müssen an ein konkretes Source-/Target-ABI-Paar gebunden sein.

## Callbacks

Callbacks in die fremde Umgebung benötigen eine Rückübersetzung:

```text
Nova Component
      ↓
Reverse ABI Thunk
      ↓
Foreign Callback
```

Lebensdauer und Ausführungscontext müssen dabei kontrolliert bleiben.

## Pointer und Speicher

Pointer dürfen nicht ungeprüft zwischen unterschiedlichen Schutzdomänen übertragen werden.

Vor einer Übergabe müssen insbesondere geprüft werden:

```text
Address Validity
Access Rights
Size
Alignment
Lifetime
Ownership
Protection Domain
```

Wo direkte gemeinsame Nutzung nicht sicher möglich ist, muss eine kontrollierte Kopie verwendet werden.

## Fehler und Exceptions

ABI-spezifische Fehler-, Exception- und Unwind-Mechanismen müssen an ABI-Grenzen kontrolliert behandelt werden.

Eine fremde Exception darf nicht unkontrolliert durch native NovaOS-Frames propagieren.

## Architekturwechsel

ABI Translation allein emuliert keine andere Prozessorarchitektur.

```text
Same Architecture
      ↓
ABI Translation

Different Architecture
      ↓
Binary Translation / Emulation
      ↓
ABI Translation
```

## Performance

Für validierte, häufig verwendete Übergänge dürfen Translation Paths gecacht und optimiert werden.

```text
Resolve
  ↓
Validate
  ↓
Generate / Select Thunk
  ↓
Cache
  ↓
Execute
```

Sicherheits-, Lifetime- und Authority-Prüfungen dürfen dadurch nicht entfallen.

## Normative Anforderungen

1. Source ABI und Target ABI MÜSSEN eindeutig bestimmt werden.
2. ABI Translation MUSS von API- und Syscall-Translation getrennt bleiben.
3. Calling Convention, Register- und Stack-Unterschiede MÜSSEN korrekt übersetzt werden.
4. ABI-spezifische Datenlayouts MÜSSEN berücksichtigt werden.
5. Pointer MÜSSEN vor domänenübergreifender Verwendung validiert werden.
6. Unsicher gemeinsam nutzbarer Speicher MUSS kopiert oder anderweitig isoliert werden.
7. Callbacks MÜSSEN eine kontrollierte Rückübersetzung unterstützen.
8. Fremde Exceptions DÜRFEN nicht unkontrolliert ABI-Grenzen überschreiten.
9. ABI Translation DARF keine zusätzliche Authority erzeugen.
10. Architekturübergreifende Ausführung MUSS von Binary Translation oder Emulation getrennt behandelt werden.
11. Optimierte Thunks DÜRFEN Sicherheitsprüfungen nicht umgehen.
12. Translation Contract, verwendete Thunks und Fehler MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-ABI-0001`
- `NPSPEC-COMPAT-API-0001`
- `NPSPEC-COMPAT-SYSCALL-0001`
- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-SYSTEM-RUNTIME-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann unterschiedliche binäre Schnittstellen über kontrollierte ABI Translation miteinander verbinden. Calling Conventions, Register, Stack, Datenlayouts, Pointer und Callbacks werden übersetzt, ohne fremde ABIs in die native NovaOS-ABI zu übernehmen oder Sicherheits- und Authority-Grenzen aufzuweichen.
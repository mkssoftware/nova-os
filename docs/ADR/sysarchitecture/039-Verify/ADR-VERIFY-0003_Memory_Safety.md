# ADR-VERIFY-0003 – Memory Safety

## Status

Angenommen

## Kategorie

Verification / Memory Safety / Kernel / Security

## Kontext

Speicherfehler gehören zu den kritischsten Fehlerklassen eines Betriebssystems.

Dazu gehören insbesondere:

```text
Use-after-free
Buffer Overflow
Out-of-Bounds Access
Double Free
Null / Invalid Pointer Access
Uninitialized Memory
Integer Overflow bei Größenberechnungen
```

Im Kernel oder in privilegierten Komponenten können solche Fehler zu Datenkorruption, Abstürzen oder Sicherheitsverletzungen führen.

## Entscheidung

NovaOS behandelt **Memory Safety als grundlegende Systemeigenschaft**.

```text
Memory-Safe Design
      +
Static Verification
      +
Runtime Protection
      +
Testing
```

Speichersicherheit wird bevorzugt konstruktiv erreicht und nicht ausschließlich durch nachträgliche Fehlererkennung.

## Memory-Safe Implementierung

Für neue Komponenten sollen, soweit technisch sinnvoll, Mechanismen verwendet werden, die typische Speicherfehler bereits durch Sprache, Typsystem oder API-Design verhindern.

Unsichere Speicheroperationen bleiben für Low-Level-Bereiche zulässig, beispielsweise:

```text
Boot
HAL
MMU
Interrupt Handling
Context Switching
Device Drivers
Hardware Access
```

Diese Bereiche müssen klar begrenzt werden.

## Unsafe Boundaries

Unsicherer Code wird als explizite Vertrauensgrenze behandelt.

```text
Safe Component
     ↓
Validated Interface
     ↓
Unsafe Low-Level Code
     ↓
Hardware
```

Unsafe Code soll möglichst klein, isoliert und überprüfbar bleiben.

## Speicherzugriffe

Kritische Speicheroperationen müssen insbesondere folgende Eigenschaften berücksichtigen:

```text
Bounds
Lifetime
Ownership
Alignment
Initialization
Access Rights
Integer Safety
```

Sicherheitskritische Annahmen sollen explizit prüfbar sein.

## Runtime Protection

Hardware- und Laufzeitschutz ergänzen die Implementierung.

Dazu gehören je nach Plattform:

```text
Virtual Memory Isolation
NX / Execute Protection
Guard Pages
Stack Protection
Privilege Separation
Read-Only Mappings
```

Diese Mechanismen ersetzen keine speichersichere Implementierung.

## Verification

Kritische Speicherkomponenten sollen durch geeignete Verfahren geprüft werden:

```text
Static Analysis
Assertions
Sanitizers in Test Builds
Fuzzing
Property Tests
Formal Verification where justified
```

Besondere Priorität besitzen Kernel-Grenzen und Parser für nicht vertrauenswürdige Daten.

## Fehlerverhalten

Erkannte Memory-Safety-Verletzungen dürfen nicht stillschweigend ignoriert werden.

Je nach Kontext muss NovaOS:

```text
Reject
Terminate
Isolate
Recover
Panic
```

Ein unsicherer Weiterbetrieb ist nur zulässig, wenn die Integrität nachweislich erhalten bleibt.

## Normative Anforderungen

1. Memory Safety MUSS als grundlegende Sicherheits- und Korrektheitseigenschaft behandelt werden.
2. Neue Komponenten SOLLEN speichersichere Mechanismen bevorzugen.
3. Unsafe Code MUSS explizit identifizierbar und begrenzt sein.
4. Übergänge zwischen sicheren und unsicheren Bereichen MÜSSEN validierte Schnittstellen verwenden.
5. Speicherzugriffe MÜSSEN Bounds, Lifetime und Ownership berücksichtigen.
6. Integerfehler bei Speichergrößen und Adressberechnungen MÜSSEN berücksichtigt werden.
7. Hardwarebasierter Speicherschutz MUSS genutzt werden, soweit verfügbar.
8. Kritischer Low-Level-Code SOLL verstärkt statisch und dynamisch geprüft werden.
9. Erkannte Memory-Safety-Verletzungen DÜRFEN nicht unkontrolliert weiterlaufen.

## Konsequenzen

### Positiv

- reduziert zentrale Kernel- und Security-Fehlerklassen,
- begrenzt die Angriffsfläche unsicherer Speicheroperationen,
- verbessert Stabilität und Fehlereingrenzung,
- schafft klare Grenzen für notwendigen Low-Level-Code.

### Negativ

- Low-Level-Hardwarezugriffe bleiben teilweise unsafe,
- zusätzliche Prüfungen können Entwicklungsaufwand verursachen,
- bestehender C/C++-Code benötigt besondere Prüfung.

## Abhängigkeiten

- `ADR-VERIFY-0001_Formale_Spezifikation_für_kritische_Komponenten`
- `ADR-ARCH-0014_Explizite_Contracts`
- `ADR-ARCH-0016_Fail_Safe_Defaults`
- `ADR-SECURITY-0001`
- `ADR-MEMORY-0001`

## Ergebnis

NovaOS verfolgt das Prinzip:

```text
Safe by default
     ↓
Unsafe only where necessary
     ↓
Small explicit boundary
     ↓
Verify aggressively
```

Unsicherer Speicherzugriff bleibt für hardwarenahe Aufgaben möglich, wird jedoch auf klar definierte und besonders überprüfte Bereiche beschränkt.
# NPSPEC-NAMESPACE-VIRTUAL-0001 – Nova Virtual Namespace

## Status

Angenommen

## Kategorie

Namespace / Virtual

## Zweck

NovaOS definiert virtuelle Namespace-Einträge für Ressourcen, die keine direkte physische Datei oder Verzeichnisstruktur besitzen.

Damit können dynamische Systemressourcen, Geräte, Services, Zustände und generierte Sichten über das einheitliche Namespace-Modell erreichbar sein.

## Grundprinzipien

```text
Virtual ≠ Physical
Virtual Path ≠ Storage Location
Virtual Resource ≠ File
Visibility ≠ Authority
Projection ≠ Copy
Virtual Object ≠ Temporary Object
```

## Modell

```text
VirtualNamespaceEntry
├── VirtualID
├── NamespaceID
├── ProviderID
├── ResourceType
├── ObjectID / ResourceID
├── Scope
├── Operations
├── State
└── Version
```

Ein virtueller Eintrag wird durch einen Provider erzeugt und in einen Namespace projiziert.

## Ressourcen

Virtuelle Einträge können insbesondere repräsentieren:

```text
Devices
Services
System State
Runtime Information
Generated Views
Remote Resources
Diagnostics
IPC Resources
Dynamic Collections
Semantic Projections
```

## Auflösung

```text
Virtual Path
     ↓
Namespace Resolution
     ↓
Virtual Entry
     ↓
Provider
     ↓
ObjectID / ResourceID
     ↓
Capability Check
     ↓
Authorized Handle
```

Der Provider stellt die konkrete Operation bereit.

## Provider

Provider und virtuelle Ressource bleiben getrennte Identitäten.

```text
Virtual Resource
      ↓
Provider A

oder

Virtual Resource
      ↓
Provider B
```

Ein Providerwechsel darf die stabile Ressourcenidentität nicht verändern, sofern weiterhin dieselbe logische Ressource dargestellt wird.

## Operationen

Virtuelle Ressourcen können unterschiedliche Operationen unterstützen:

```text
Read
Write
Query
Execute
Subscribe
Control
Enumerate
```

Nicht unterstützte Operationen müssen eindeutig abgelehnt werden.

## Dynamische Inhalte

Virtuelle Namespace-Inhalte dürfen dynamisch erzeugt werden.

```text
Query Namespace
      ↓
Provider
      ↓
Current System State
      ↓
Generated Entries
```

Die sichtbare Struktur muss daher nicht dauerhaft gespeichert sein.

## Scopes

Virtuelle Einträge können begrenzt werden auf:

```text
System
User
Process
Program
Solution
Workspace
Recovery
```

Unterschiedliche Kontexte dürfen unterschiedliche virtuelle Sichten erhalten.

## Objektidentität

Besitzt eine virtuelle Ressource eine stabile logische Identität, muss diese unabhängig vom sichtbaren Pfad bleiben.

Kurzlebige dynamische Ressourcen dürfen stattdessen eine passende `ResourceID` verwenden.

## Sicherheit

Virtuelle Namespaces erzeugen keine zusätzliche Authority.

Insbesondere gilt:

```text
Visible Device ≠ Device Access
Visible Service ≠ Service Authority
Visible Resource ≠ Permission
```

Jede geschützte Operation bleibt capability- und policy-basiert.

## Fehler und Provider-Ausfall

Ist ein Provider nicht verfügbar:

```text
Virtual Entry
     ↓
Provider Unavailable
     ↓
Unavailable / Degraded
```

Ein Provider-Ausfall darf nicht zu einer stillen Umleitung auf eine andere Ressource führen.

## Normative Anforderungen

1. NovaOS MUSS virtuelle Namespace-Einträge unterstützen.
2. Virtuelle Ressourcen DÜRFEN ohne physische Datei existieren.
3. Virtuelle Pfade DÜRFEN nicht als physische Speicherorte interpretiert werden.
4. Virtuelle Einträge MÜSSEN einem verantwortlichen Provider zuordenbar sein.
5. Provider und Ressourcenidentität MÜSSEN getrennt bleiben.
6. Dynamisch generierte Namespace-Inhalte MÜSSEN unterstützt werden.
7. Virtuelle Ressourcen MÜSSEN definierte unterstützte Operationen besitzen.
8. Virtuelle Einträge MÜSSEN auf Namespace-Scopes begrenzbar sein.
9. Sichtbarkeit DARF keine Authority erzeugen.
10. Geschützte Operationen MÜSSEN capability-basiert autorisiert werden.
11. Provider-Ausfälle MÜSSEN kontrolliert als nicht verfügbar oder degradiert darstellbar sein.
12. Virtuelle Ressource, Provider, Zustand und unterstützte Operationen MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-FILESYSTEM-NAMESPACE-0002`
- `NPSPEC-FILESYSTEM-PROJECTION-0002`
- `NPSPEC-NAMESPACE-SYSTEM-0001`
- `NPSPEC-NAMESPACE-OVERLAY-0002`
- `NPSPEC-POLICY-NAMESPACE-0001`
- `NPSPEC-REGISTRY-SERVICE-0001`
- `NPSPEC-REGISTRY-DEVICE-0001`
- `NPSPEC-POLICY-CAPABILITY-0001`
- `NPSPEC-ARCH-INTROSPECTION-0001`

## Ergebnis

NovaOS kann physische und nichtphysische Ressourcen über dasselbe Namespace-Modell darstellen. Geräte, Services, Systemzustände und dynamisch generierte Ressourcen können dadurch einheitlich adressiert werden, während Provider, Ressourcenidentität, physischer Speicher und Authority voneinander getrennt bleiben.
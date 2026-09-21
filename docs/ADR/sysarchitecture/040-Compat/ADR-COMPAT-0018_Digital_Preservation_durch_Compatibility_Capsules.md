# ADR-COMPAT-0018 – Digital Preservation durch Compatibility Capsules

## Status

Angenommen

## Kategorie

Compatibility / Digital Preservation / Reproducibility

## Kontext

Software kann langfristig unbenutzbar werden, obwohl die eigentlichen Programmdaten noch vorhanden sind.

Ursachen sind beispielsweise:

```text
Nicht mehr verfügbare Runtimes
Veraltete Betriebssystemumgebungen
Entfernte Compatibility Profiles
Fremde CPU-Architekturen
Historische Bibliotheken
Anwendungsspezifische Quirks
```

Für langfristige digitale Erhaltung reicht es daher nicht aus, nur das Anwendungsbinary zu archivieren.

## Entscheidung

NovaOS unterstützt **Compatibility Capsules** als portable, versionierte Beschreibung einer reproduzierbaren Legacy-Ausführungsumgebung.

```text
Application
    +
Compatibility Metadata
    +
Runtime References
    +
Profile / Quirks
    +
Execution Requirements
    ↓
Compatibility Capsule
```

Eine Capsule beschreibt, was benötigt wird, um eine Anwendung später möglichst reproduzierbar auszuführen.

## Inhalt

Eine Compatibility Capsule kann referenzieren:

```text
Application Version
Binary / Object IDs
Required ISA
Compatibility Personality
Compatibility Profile
Quirk Set
Runtime Versions
Library Versions
Filesystem View
Registry / Configuration View
Execution Requirements
Integrity / Provenance Metadata
```

Große oder gemeinsam genutzte Komponenten müssen nicht dupliziert werden und können über stabile IDs und Versionen referenziert werden.

## Ausführung

Beim Start wird die Capsule durch die Compatibility-Infrastruktur aufgelöst.

```text
Compatibility Capsule
        ↓
Resolve Dependencies
        ↓
Select Execution Method
        ↓
Compatibility Domain
        ↓
Legacy Application
```

Die Execution Compatibility Ladder entscheidet, ob beispielsweise Translation, Runtime, VM oder Emulation erforderlich ist.

## Unveränderliche Beschreibung

Eine archivierte Capsule beschreibt eine konkrete historische Umgebung.

Updates dürfen diese Beschreibung nicht stillschweigend verändern.

Neue oder migrierte Varianten erhalten eine neue Version.

## Sicherheit

Eine Capsule archiviert Kompatibilitätsanforderungen, aber keine historischen Sicherheitsprivilegien.

```text
Historical Environment
        ↓
Current NovaOS Security
        ↓
Execution
```

Beim späteren Start gelten weiterhin die aktuellen NovaOS-Sicherheits-, Capability-, Trust- und Sovereignty-Regeln.

## Reproduzierbarkeit

NovaOS soll erkennen können, ob die ursprüngliche Umgebung:

```text
Fully Reproducible
Partially Reproducible
Substituted
Unavailable
```

ist.

Eine ersetzte Runtime oder ein anderer Emulator darf nicht stillschweigend als identische Umgebung dargestellt werden.

## Portabilität

Compatibility Capsules sollen unabhängig von der aktuellen Hardwaregeneration bleiben.

Eine Capsule für eine fremde ISA kann beispielsweise später über Binary Translation oder Emulation ausgeführt werden.

## Normative Anforderungen

1. NovaOS SOLL Legacy-Ausführungsumgebungen als Compatibility Capsules archivieren können.
2. Capsules MÜSSEN ihre relevanten Compatibility-Abhängigkeiten versioniert beschreiben.
3. Komponenten SOLLEN über stabile Identitäten und Versionen referenzierbar sein.
4. Archivierte Capsules DÜRFEN nicht stillschweigend semantisch verändert werden.
5. Die Ausführungsstrategie MUSS von der archivierten Umgebung getrennt bleiben.
6. Fremde ISAs MÜSSEN über Translation oder Emulation rekonstruierbar sein können.
7. Capsules DÜRFEN keine historischen Sicherheitsprivilegien konservieren.
8. Aktuelle NovaOS-Sicherheitsregeln MÜSSEN auch bei archivierter Software gelten.
9. Abweichungen von der ursprünglichen Umgebung MÜSSEN erkennbar sein.
10. Capsules SOLLEN langfristig exportierbar und portierbar sein.

## Konsequenzen

### Positiv

- historische Software bleibt langfristig ausführbar,
- Abhängigkeiten und Compatibility-Zustände werden reproduzierbar,
- Hardware- und Architekturwechsel verlieren an Bedeutung,
- Legacy-Umgebungen können archiviert statt dauerhaft systemweit gepflegt werden.

### Negativ

- langfristige Erhaltung benötigt Metadaten und archivierte Komponenten,
- vollständige Reproduzierbarkeit kann bei externer Hardware oder Diensten unmöglich sein,
- alte Umgebungen benötigen weiterhin sichere Isolation.

## Abhängigkeiten

- `ADR-COMPAT-0007_Execution_Compatibility_Ladder`
- `ADR-COMPAT-0008_Legacy_Software_in_Compatibility_Domains`
- `ADR-COMPAT-0010_Legacy_Runtime_side_by_side`
- `ADR-COMPAT-0011_Binary_Translation_für_Cross_ISA`
- `ADR-COMPAT-0017_Versionierte_Compatibility_Profiles_und_Quirks`
- `ADR-VERIFY-0010_Reproducible_Verification`
- `ADR-TRUST-0006_Software_und_Supply_Chain_Provenance`
- `ADR-ARCH-0019_Evolvierbar`

## Ergebnis

NovaOS behandelt langfristige Softwareerhaltung als reproduzierbare Compatibility-Umgebung:

```text
Application
   +
Environment Description
   +
Versioned Dependencies
   ↓
Compatibility Capsule
   ↓
Future NovaOS
   ↓
Translation / VM / Emulation
```

Damit soll Legacy-Software auch über zukünftige NovaOS- und Hardwaregenerationen hinweg nachvollziehbar und möglichst reproduzierbar ausführbar bleiben.
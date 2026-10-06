# NPSPEC-COMPAT-PROTOCOL-0001 – Nova Protocol Compatibility

## Status

Angenommen

## Kategorie

Compatibility / Protocol

## Zweck

NovaOS definiert eine Kompatibilitätsschicht für fremde, ältere und nicht-native Kommunikationsprotokolle.

Protocol Compatibility ermöglicht die Nutzung bestehender Protokolle, ohne deren internes Modell, Sicherheitskonzept oder Implementierung zum Bestandteil der nativen NovaOS-Architektur zu machen.

## Grundprinzipien

```text
Protocol ≠ Transport
Protocol Compatibility ≠ Native Protocol
Protocol Support ≠ Network Authority
Protocol Identity ≠ Port Number
Protocol Translation ≠ Trust
Remote Identity ≠ Local Authority
```

## Modell

```text
Application / Capability
        ↓
Protocol Interface
        ↓
Protocol Provider
        ↓
Encode / Decode / Translate
        ↓
Transport
        ↓
Network / IPC / Device
```

## Protokollidentität

Ein unterstütztes Protokoll wird mindestens beschrieben durch:

```text
Protocol
├── ProtocolID
├── Version
├── MessageModel
├── TransportRequirements
├── SecurityRequirements
├── CompatibilityProfile
└── Provider
```

`ProtocolID` und Version bleiben von konkreten Providern getrennt.

## Protokolltypen

Die Architektur darf unterschiedliche Protokollklassen unterstützen:

```text
Network Protocols
Application Protocols
IPC Protocols
Device Protocols
Legacy Protocols
File Transfer Protocols
Service Protocols
```

## Provider

Mehrere Provider dürfen dasselbe Protokoll implementieren:

```text
ProtocolID
├── Native Provider
├── Compatibility Provider
├── Legacy Provider
└── Remote Provider
```

Die Auswahl erfolgt anhand von Compatibility, Trust, Policy und Execution Contract.

## Übersetzung

Falls NovaOS intern ein anderes Kommunikationsmodell verwendet, darf ein Protocol Provider übersetzen:

```text
Foreign Protocol
       ↓
Parse / Validate
       ↓
Semantic Translation
       ↓
Nova Interface
```

Die Übersetzung darf zustandsbehaftet sein und Sessions, Sequenzen oder Verbindungszustände verwalten.

## Versionierung

Mehrere Protokollversionen dürfen parallel unterstützt werden.

```text
ProtocolID
├── Version 1
├── Version 2
└── Version 3
```

Versionsaushandlung darf verwendet werden, sofern das jeweilige Protokoll dies unterstützt.

## Sicherheit

Eingehende Protokolldaten gelten grundsätzlich als nicht vertrauenswürdig.

Vor ihrer Verarbeitung müssen relevante:

```text
Headers
Lengths
Message Types
Payloads
State Transitions
Authentication Data
```

validiert werden.

Protokollzugriff erzeugt keine Netzwerk-, Geräte- oder Ressourcen-Authority.

## Isolation

Riskante, komplexe oder veraltete Protocol Provider sollen isoliert ausgeführt werden können.

Fehler eines Providers dürfen nicht unnötig den Netzwerkstack, Kernel oder andere Protokolle beeinträchtigen.

## Verschlüsselung und Authentifizierung

Protokolle dürfen eigene Sicherheitsmechanismen besitzen.

Diese ersetzen jedoch nicht automatisch NovaOS-Trust, Identity, Capability oder Policy.

Unsichere Legacy-Protokolle dürfen durch Policy eingeschränkt oder blockiert werden.

## Fehler und Degradation

Protocol Compatibility kann folgende Zustände melden:

```text
Supported
Translated
PartiallySupported
Deprecated
Unsupported
Unavailable
Blocked
```

Nicht unterstützte Nachrichten oder Zustände müssen kontrolliert behandelt werden.

## Normative Anforderungen

1. Protokollidentität MUSS von Transport, Port und Provider getrennt bleiben.
2. Unterstützte Protokolle MÜSSEN eindeutig identifizierbar und versionierbar sein.
3. Mehrere Provider und Protokollversionen MÜSSEN parallel unterstützt werden können.
4. Eingehende Protokolldaten MÜSSEN vor Verarbeitung validiert werden.
5. Protocol Compatibility DARF keine zusätzliche Authority erzeugen.
6. Netzwerk- und Gerätezugriffe MÜSSEN weiterhin Capability- und Policy-Prüfungen unterliegen.
7. Fremde Sicherheitsmodelle DÜRFEN NovaOS-Sicherheitsgrenzen nicht umgehen.
8. Nicht unterstützte Nachrichten MÜSSEN kontrolliert fehlschlagen.
9. Unsichere Legacy-Protokolle MÜSSEN einschränkbar oder blockierbar sein.
10. Protocol Provider SOLLEN isoliert ausführbar sein, wenn ihr Risikoprofil dies erfordert.
11. Versionsaushandlung DARF Sicherheitsanforderungen nicht herabsetzen.
12. ProtocolID, Version, Provider, Trust und Compatibility-Zustand MÜSSEN introspektierbar sein.

## Abhängigkeiten

- `NPSPEC-COMPAT-PERSONALITY-0001`
- `NPSPEC-COMPAT-API-0001`
- `NPSPEC-CAPABILITY-EXECUTIONCONTRACT-0001`
- `NPSPEC-CAPABILITY-TRUST-0001`
- `NPSPEC-CAPABILITY-PERMISSION-0001`
- `NPSPEC-SYSTEM-SECURITY-0001`

## Ergebnis

NovaOS kann fremde und ältere Kommunikationsprotokolle über austauschbare Compatibility Provider nutzen. Protokollidentität, Transport, Provider, Trust und Authority bleiben getrennt, während Validierung, Isolation und das native NovaOS-Capability- und Sicherheitsmodell für jede Kommunikation maßgeblich bleiben.
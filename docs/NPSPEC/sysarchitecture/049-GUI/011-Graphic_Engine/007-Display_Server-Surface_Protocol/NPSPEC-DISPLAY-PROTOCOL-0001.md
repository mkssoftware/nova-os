# NPSPEC-DISPLAY-PROTOCOL-0001 – Nova Display Protocol

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Display Protocol definiert die standardisierte Kommunikation zwischen Anwendungen, Systemdiensten und dem Nova Display Server.

Ziel ist die sichere, effiziente und hardwareunabhängige Verwaltung grafischer Surfaces, Buffer und Präsentationsvorgänge.

Das Protokoll bildet die Grundlage für die Kommunikation zwischen Ring-3-Prozessen und dem Display Server.

## 2. Architekturprinzipien

- **IPC-Based:** Kommunikation erfolgt über Nova IPC.
- **Capability-Based:** Zugriffe werden durch explizite Berechtigungen kontrolliert.
- **Asynchronous:** Nicht blockierende Kommunikation wird bevorzugt.
- **Versioned:** Protokollversionen und Erweiterungen werden unterstützt.
- **Zero-Copy:** Große Grafikdaten werden über gemeinsame Buffer übertragen.
- **Transactional:** Zusammengehörige Zustandsänderungen werden atomar übernommen.
- **Event-Driven:** Zustandsänderungen und Ergebnisse werden über Ereignisse gemeldet.
- **Backend-Independent:** Das Protokoll bleibt unabhängig von GPU- und Display-Treibern.

## 3. Protokollarchitektur

Das Nova Display Protocol besteht aus drei Kommunikationsbereichen:

| Bereich | Verantwortung |
|---|---|
| Control | Verbindungen, Objekte und Berechtigungen |
| Surface | Surface-Zustände, Buffer und Commits |
| Events | Rückmeldungen, Freigaben und Fehler |

Grafische Pixeldaten werden nicht direkt innerhalb gewöhnlicher IPC-Nachrichten übertragen.

Stattdessen werden kontrollierte Referenzen auf gemeinsam nutzbare Buffer verwendet.

## 4. Connection Model

Jeder Client MUSS eine Verbindung zum Display Server herstellen können.

Eine Verbindung enthält mindestens:

- **Connection ID:** Eindeutige Verbindungsidentifikation.
- **Protocol Version:** Ausgehandelte Protokollversion.
- **Client Capabilities:** Autorisierte Funktionen.
- **Object Namespace:** Verbindungsspezifische Objektidentifikatoren.
- **Connection State:** Aktueller Verbindungszustand.

Beim Verbindungsaufbau MÜSSEN Protokollversion und unterstützte Erweiterungen ausgehandelt werden.

Nicht unterstützte Funktionen DÜRFEN nicht verwendet werden.

## 5. Protocol Objects

Nova Display Protocol MUSS folgende grundlegende Objekttypen unterstützen:

| Objekt | Funktion |
|---|---|
| Connection | Client-Verbindung |
| Surface | Grafische Oberfläche |
| Buffer | Grafischer Speicher |
| Output | Bildschirmausgabe |
| Commit | Atomare Surface-Aktualisierung |
| Fence | Synchronisationsobjekt |
| Callback | Asynchrone Rückmeldung |

Objekte MÜSSEN eindeutige Identifikatoren innerhalb ihres gültigen Namensraums besitzen.

Objektreferenzen MÜSSEN vor ihrer Verwendung validiert werden.

## 6. Message Model

Jede Protokollnachricht MUSS eindeutig interpretierbar sein.

Eine Nachricht enthält mindestens:

- **Message Type:** Art der Operation.
- **Object ID:** Betroffenes Protokollobjekt.
- **Sequence ID:** Zuordnung von Anfragen und Antworten.
- **Payload Length:** Länge der Nutzdaten.
- **Payload:** Operationsabhängige Daten.

Die Nachrichtenkodierung MUSS Byte-Reihenfolge, Feldgrößen, Ausrichtung und zulässige Werte eindeutig definieren.

Unbekannte oder ungültige Nachrichten MÜSSEN sicher behandelt werden.

## 7. Surface Operations

Das Protokoll MUSS mindestens folgende Operationen ermöglichen:

| Operation | Beschreibung |
|---|---|
| CreateSurface | Erstellt eine Surface |
| DestroySurface | Entfernt eine Surface |
| AttachBuffer | Verknüpft einen Buffer |
| SetDamage | Meldet veränderte Bereiche |
| SetProperties | Aktualisiert Surface-Eigenschaften |
| CommitSurface | Übernimmt ausstehende Änderungen atomar |
| RequestFrame | Fordert eine Frame-Rückmeldung an |

Surface-Eigenschaften werden zunächst als ausstehender Zustand verwaltet.

Erst ein gültiger Commit veröffentlicht die zusammengehörigen Änderungen.

## 8. Buffer Sharing

Nova Display Protocol MUSS die Übergabe autorisierter Buffer-Referenzen unterstützen.

Dabei gilt:

1. Buffer werden über Nova Memory Management bereitgestellt.
2. Zugriffsrechte werden über Nova Capabilities kontrolliert.
3. Der Display Server validiert Format, Größe und Speicherlayout.
4. Die Nutzung wird über Synchronisationsobjekte koordiniert.
5. Die Freigabe erfolgt erst nach Abschluss aller relevanten Zugriffe.

Buffer-Handles DÜRFEN keine unkontrollierten physischen Speicheradressen offenlegen.

## 9. Atomic Surface Commit

Zusammengehörige Surface-Änderungen MÜSSEN atomar übernommen werden.

Ein Commit kann enthalten:

- Neuen Buffer
- Damage Regions
- Surface-Eigenschaften
- Synchronisationsinformationen
- Zugehörige Zustandsversion

Ein ungültiger Commit MUSS abgewiesen werden, ohne einen teilweise veröffentlichten Surface-Zustand zu hinterlassen.

Die Annahme eines Commits bedeutet nicht automatisch, dass dessen Inhalt bereits dargestellt wurde.

## 10. Events und Feedback

Nova Display Protocol MUSS asynchrone Ereignisse unterstützen.

Mindestens erforderlich sind:

| Event | Bedeutung |
|---|---|
| BufferReleased | Buffer darf entsprechend seiner Rechte wiederverwendet werden |
| FrameReady | Angeforderte Frame-Rückmeldung |
| CommitAccepted | Commit wurde angenommen |
| CommitRejected | Commit wurde abgewiesen |
| OutputChanged | Ausgabezustand wurde geändert |
| ProtocolError | Protokollfehler wurde erkannt |

Präsentationsbestätigungen MÜSSEN von der bloßen Annahme eines Surface-Commits unterscheidbar sein.

## 11. Synchronization

Das Protokoll MUSS explizite Synchronisation unterstützen können.

Dazu gehören:

- Acquire Fences
- Release Fences
- Buffer Ownership
- Commit Ordering
- Presentation Feedback

Nachrichtenreihenfolgen MÜSSEN innerhalb der definierten Ordnungsdomänen eindeutig sein.

Asynchrone Verarbeitung DARF keine unzulässigen Speicherzugriffe oder inkonsistenten Surface-Zustände erzeugen.

## 12. Sicherheit und Isolation

Nova Display Protocol MUSS die Sicherheitsarchitektur von NovaOS einhalten.

Dabei gilt:

- Jeder Client besitzt ausschließlich autorisierte Objektrechte.
- Fremde Surfaces dürfen nicht unberechtigt gelesen oder verändert werden.
- Privilegierte Output-Operationen erfordern gesonderte Capabilities.
- Eingaben und Ressourcenreferenzen müssen validiert werden.
- Ressourcenverbrauch muss begrenzbar sein.
- Fehlerhafte Clients dürfen den Display Server nicht kompromittieren.

Der Display Server MUSS Client-Verbindungen kontrolliert beenden und zugehörige Ressourcen sicher bereinigen können.

## 13. Versionierung und Erweiterbarkeit

Nova Display Protocol MUSS versioniert sein.

Dabei gilt:

- Basisfunktionen besitzen stabile Semantik.
- Optionale Funktionen werden über Erweiterungen bereitgestellt.
- Clients dürfen nur ausgehandelte Funktionen verwenden.
- Neue Protokollversionen sollen bestehende Anwendungen möglichst kompatibel halten.
- Inkompatible Änderungen erfordern eine eindeutig unterscheidbare Version.

Hardwareabhängige Funktionen DÜRFEN über optionale Protokollerweiterungen bereitgestellt werden.

## 14. Fehlerbehandlung

NovaOS MUSS Protokollfehler eindeutig erkennen und melden können.

Dazu gehören:

- Ungültige Nachrichten
- Unbekannte Objektidentifikatoren
- Fehlende Berechtigungen
- Ungültige Buffer
- Nicht unterstützte Operationen
- Fehlgeschlagene Commits
- Ressourcenüberschreitungen

Fehler MÜSSEN einem definierten Fehlercode zugeordnet werden.

Ein einzelner fehlerhafter Client DARF andere Verbindungen nicht beeinträchtigen.

## 15. Verbindliche Anforderungen

1. Nova Display Protocol MUSS auf Nova IPC aufbauen.
2. Die Kommunikation MUSS capability-basiert abgesichert sein.
3. Protokollversionen MÜSSEN ausgehandelt werden.
4. Nachrichtenformate MÜSSEN eindeutig definiert sein.
5. Surface- und Buffer-Objekte MÜSSEN unterstützt werden.
6. Große Grafikdaten SOLLEN über Shared Buffers übertragen werden.
7. Surface-Änderungen MÜSSEN atomar veröffentlicht werden können.
8. Asynchrone Ereignisse MÜSSEN unterstützt werden.
9. Commit-Annahme und tatsächliche Präsentation MÜSSEN unterscheidbar sein.
10. Buffer-Zugriffe MÜSSEN synchronisiert werden.
11. Objekt- und Ressourcenreferenzen MÜSSEN validiert werden.
12. Nicht autorisierte Operationen MÜSSEN abgewiesen werden.
13. Ressourcenverbrauch MUSS begrenzbar sein.
14. Fehlerhafte Clients DÜRFEN andere Clients nicht beeinträchtigen.
15. Optionale Protokollerweiterungen MÜSSEN möglich sein.
16. Das Protokoll MUSS unabhängig von einer bestimmten GPU-API bleiben.
17. Der deterministische Modus MUSS reproduzierbare Protokollzustandsübergänge ermöglichen.

## 16. Ergebnis

Nova Display Protocol stellt die einheitliche Kommunikationsschnittstelle zwischen Anwendungen, Systemdiensten und dem Nova Display Server bereit.

Durch capability-basierte IPC, Shared Buffers, atomare Surface-Commits und asynchrone Ereignisse wird eine sichere und effiziente Grafikkommunikation ermöglicht.

Damit bildet das Protokoll die Grundlage für die isolierte Ring-3-Grafikarchitektur von NovaOS und unterstützt Zero-Copy, Incremental Rendering sowie hardwareunabhängige Frame-Präsentation.
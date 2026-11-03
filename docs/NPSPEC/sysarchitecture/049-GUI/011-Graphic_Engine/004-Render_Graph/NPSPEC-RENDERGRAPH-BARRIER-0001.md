# NPSPEC-RENDERGRAPH-BARRIER-0001 – Nova Render Resource Barriers

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Resource Barriers definiert die Planung und Optimierung von Synchronisationsbarrieren zwischen Rendering-Operationen innerhalb des Nova Render Graph.

Ziel ist die korrekte Reihenfolge und Sichtbarkeit von Ressourcenzugriffen bei möglichst geringem Synchronisationsaufwand.

Die logische Planung erfolgt im Render Graph. Die hardwareabhängige Umsetzung übernimmt das jeweilige Rendering-Backend.

## 2. Architekturprinzipien

- **Dependency-Driven:** Barriers werden aus tatsächlichen Ressourcenabhängigkeiten abgeleitet.
- **Hazard-Aware:** Konflikte zwischen Lese- und Schreibzugriffen werden erkannt.
- **Minimal:** Unnötige Synchronisationsoperationen werden vermieden.
- **Resource-Aware:** Ressourcen und Subressourcen werden getrennt betrachtet.
- **Queue-Aware:** Queue-übergreifende Zugriffe werden berücksichtigt.
- **Backend-Independent:** Die logische Barrier-Beschreibung bleibt hardwareunabhängig.
- **Incremental:** Unveränderte Synchronisationspläne können wiederverwendet werden.
- **Deterministic:** Identische Eingaben ermöglichen reproduzierbare Barrier-Pläne.

## 3. Barrier Model

Eine logische Resource Barrier beschreibt einen erforderlichen Synchronisations- oder Zustandsübergang.

Sie enthält mindestens:

- **Resource ID:** Betroffene Ressource.
- **Resource Range:** Betroffene Subressourcen oder Speicherbereiche.
- **Source Access:** Vorherige Zugriffsart.
- **Destination Access:** Nachfolgende Zugriffsart.
- **Source Stage:** Vorherige Verarbeitungsphase.
- **Destination Stage:** Nachfolgende Verarbeitungsphase.
- **Queue Context:** Beteiligte Ausführungsqueues.
- **Barrier Type:** Erforderliche Synchronisationsart.

Die konkrete Darstellung DARF vom GPU-Backend abweichen.

## 4. Barrier Types

NovaOS SOLL folgende logische Barrier-Arten unterstützen:

| Barrier Type | Aufgabe |
|---|---|
| Memory Barrier | Gewährleistet erforderliche Speicher-Sichtbarkeit und Zugriffsordnung |
| Buffer Barrier | Synchronisiert definierte Buffer-Bereiche |
| Image Barrier | Synchronisiert Texturen und Image-Subressourcen |
| Layout Transition | Überführt Ressourcen in erforderliche Nutzungszustände |
| Queue Transfer | Koordiniert Ressourcenübergaben zwischen Queues |
| Aliasing Barrier | Sichert die Wiederverwendung überlagerter Speicherbereiche |

Ein logischer Übergang DARF mehrere native Synchronisationsoperationen erfordern.

## 5. Hazard Detection

Nova Render Pass Dependencies stellt Informationen über Ressourcenkonflikte bereit.

Berücksichtigt werden:

- **RAW:** Read After Write
- **WAR:** Write After Read
- **WAW:** Write After Write
- **RAR:** Read After Read

RAR benötigt bei unveränderten kompatiblen Ressourcenzuständen grundsätzlich keine zusätzliche Speicherbarriere.

WAR kann eine Ausführungsordnung erfordern, ohne zwangsläufig eine Speicher-Sichtbarmachung zu benötigen.

Die tatsächlichen Anforderungen MÜSSEN anhand der Zugriffsarten, Verarbeitungsphasen und Backend-Regeln bestimmt werden.

## 6. Barrier Planning

Die Planung erfolgt grundsätzlich in folgenden Schritten:

1. Validierten Render Graph übernehmen.
2. Ressourcenzugriffe und Abhängigkeiten analysieren.
3. Erforderliche Zugriffs- und Zustandsübergänge bestimmen.
4. Queue-übergreifende Anforderungen berücksichtigen.
5. Synchronisationsoperationen zusammenfassen.
6. Redundante Barriers entfernen.
7. Barrier-Plan validieren.
8. Backendgerechte Umsetzung vorbereiten.

Die Planung MUSS vor der betroffenen GPU-Ausführung abgeschlossen sein.

## 7. Resource State Tracking

NovaOS MUSS relevante Ressourcenzustände verfolgen können.

Dazu gehören:

- Aktuelle Zugriffsarten
- Erforderliche Verarbeitungsphasen
- Image Layouts
- Queue Ownership
- Gültige Ressourceninhalte
- Ausstehende Zugriffe

Die Zustandsverwaltung SOLL auf Subressourcenebene erfolgen können.

Unabhängige Ressourcenbereiche DÜRFEN getrennte Zustände besitzen.

## 8. Barrier Optimization

NovaOS SOLL Synchronisationsoperationen optimieren.

Mögliche Optimierungen:

- Zusammenfassen kompatibler Barriers
- Entfernen redundanter Barriers
- Begrenzen auf betroffene Subressourcen
- Vermeiden unnötiger Queue Transfers
- Zusammenführen kompatibler Ressourcenübergänge

Optimierungen DÜRFEN keine erforderlichen Ausführungs- oder Speicherabhängigkeiten entfernen.

Eine geringere Anzahl von Barriers garantiert nicht automatisch bessere Performance.

## 9. Queue Synchronization

Bei Zugriffen über mehrere GPU Queues MUSS NovaOS die erforderliche Ausführungs- und Speichersynchronisation gewährleisten.

Dabei können erforderlich sein:

- Queue Ownership Transfers
- GPU Semaphores
- Timeline Semaphores
- Queue-spezifische Resource Barriers

Resource Barriers allein ersetzen nicht grundsätzlich die erforderliche Queue-übergreifende Synchronisation.

Die konkrete Umsetzung erfolgt in Zusammenarbeit mit Nova GPU Synchronization.

## 10. Resource Aliasing

Beim Transient Resource Aliasing können mehrere logische Ressourcen denselben physischen Speicher verwenden.

NovaOS MUSS sicherstellen, dass:

- Vorherige Speicherzugriffe korrekt abgeschlossen oder geordnet sind.
- Keine unzulässigen parallelen Zugriffe stattfinden.
- Erforderliche Aliasing Barriers gesetzt werden.
- Neue Ressourceninhalte vor lesender Verwendung initialisiert sind.

Ein Wechsel der logischen Ressourcenidentität hebt physische Speicherabhängigkeiten nicht auf.

## 11. Incremental Barrier Planning

NovaOS SOLL gültige Barrier-Pläne zwischen Graph-Aktualisierungen wiederverwenden.

Eine erneute Prüfung ist insbesondere erforderlich bei:

- Veränderten Ressourcenzugriffen
- Geänderten Pass-Abhängigkeiten
- Veränderten Queue-Zuweisungen
- Neuen Ressourcenbindungen
- Geänderten Aliasing-Zuordnungen

Wiederverwendete Barrier-Pläne MÜSSEN mit den tatsächlichen Ressourcen- und Ausführungszuständen kompatibel bleiben.

## 12. Backend Integration

Der Nova Render Graph beschreibt Synchronisationsanforderungen unabhängig von der GPU-API.

Das Rendering-Backend übernimmt:

- Übersetzung logischer Barriers
- Auswahl unterstützter Synchronisationsmechanismen
- Umsetzung erforderlicher Layout-Übergänge
- Queue-spezifische Synchronisation
- Validierung hardwareabhängiger Einschränkungen

Ein Backend DARF konservativere Synchronisationsoperationen verwenden, wenn präzisere Mechanismen nicht verfügbar sind.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Synchronisationsbedingungen erkennen können.

Dazu gehören:

- Fehlende Ressourcenübergänge
- Nicht unterstützte Ressourcenzustände
- Ungültige Queue Ownership
- Widersprüchliche Zugriffsanforderungen
- Nicht erfüllbare Synchronisationsbedingungen

Bei fehlgeschlagener Optimierung MUSS eine konservative, korrekte Synchronisationsstrategie verfügbar sein.

Ungültige Barrier-Pläne DÜRFEN nicht ausgeführt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Resource Barriers aus den Render-Graph-Abhängigkeiten ableiten können.
2. RAW-, WAR- und WAW-Konflikte MÜSSEN korrekt behandelt werden.
3. Ressourcen- und Subressourcenzugriffe MÜSSEN berücksichtigt werden.
4. Erforderliche Ausführungs- und Speicherabhängigkeiten MÜSSEN erhalten bleiben.
5. Ressourcenlayouts und Zugriffsarten MÜSSEN korrekt verwaltet werden.
6. Queue-übergreifende Synchronisation MUSS unterstützt werden können.
7. Resource Aliasing MUSS bei der Barrier-Planung berücksichtigt werden.
8. Redundante Barriers SOLLEN entfernt werden.
9. Kompatible Barriers SOLLEN zusammengefasst werden.
10. Unveränderte Barrier-Pläne SOLLEN wiederverwendet werden.
11. Wiederverwendete Pläne MÜSSEN erneut validierbar sein.
12. Die logische Barrier-Beschreibung MUSS backendunabhängig bleiben.
13. Die konkrete Synchronisation MUSS den Regeln des jeweiligen Backends entsprechen.
14. Ein deterministischer Barrier-Plan MUSS bei identischen Eingaben erzeugbar sein.
15. Ungültige Synchronisationspläne DÜRFEN nicht ausgeführt werden.
16. Ein konservativer Synchronisations-Fallback MUSS verfügbar sein.

## 15. Ergebnis

Nova Render Resource Barriers gewährleistet die korrekte Synchronisation von Ressourcenzugriffen innerhalb des Nova Render Graph.

Durch präzise Hazard-Erkennung, Subresource Tracking und optimierte Barrier-Planung werden unnötige GPU-Wartezeiten und Synchronisationsoperationen reduziert.

Damit ermöglicht das Subsystem eine sichere und effiziente parallele Rendering-Ausführung bei vollständiger Kontrolle über Ressourcen- und Speicherabhängigkeiten.
# NPSPEC-GPUSYNC-HAZARD-0001 – Nova GPU Resource Hazard Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova GPU Resource Hazard Tracking definiert die Erkennung und Verwaltung konkurrierender GPU-Ressourcenzugriffe.

Ziel ist die automatische Identifikation notwendiger Synchronisationsabhängigkeiten, ohne unabhängige GPU-Operationen unnötig zu serialisieren.

Das System MUSS Ressourcenabhängigkeiten über Render Passes, Command Buffers, GPU-Queues und Submissions hinweg berücksichtigen.

## 2. Architekturprinzipien

- **Access-Aware:** Lese- und Schreibzugriffe werden unterschieden.
- **Dependency-Driven:** Synchronisation entsteht aus tatsächlichen Abhängigkeiten.
- **Fine-Grained:** Ressourcenbereiche werden möglichst präzise verfolgt.
- **Cross-Queue:** Queueübergreifende Zugriffe werden berücksichtigt.
- **State-Aware:** Ressourcenlayouts und Zugriffsstatus werden verwaltet.
- **Incremental:** Nur veränderte Zugriffsbeziehungen werden neu bewertet.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Treiber.
- **Deterministic:** Identische Zugriffsfolgen erzeugen identische logische Abhängigkeiten.

## 3. Resource Access Model

Jeder relevante GPU-Ressourcenzugriff MUSS durch einen Access Record beschrieben werden können.

| Eigenschaft | Beschreibung |
|---|---|
| Resource ID | Eindeutige Ressourcenidentifikation |
| Resource Range | Betroffener Speicher- oder Subresource-Bereich |
| Access Type | Read, Write oder Read-Write |
| Pipeline Stage | Verwendete GPU-Pipeline-Phase |
| Resource State | Erforderlicher Ressourcenzustand |
| Queue ID | Zugehörige GPU-Queue |
| Operation ID | Zugehöriger Auftrag oder Render Pass |
| Resource Version | Version der Ressourcenbelegung |

Access Records MÜSSEN vor der Ausführungsplanung auswertbar sein.

## 4. Hazard Types

NovaOS MUSS folgende Datenabhängigkeiten erkennen:

| Hazard | Bedeutung | Erforderliche Behandlung |
|---|---|---|
| RAW | Read-after-Write | Schreibresultat vor dem Lesen verfügbar machen |
| WAR | Write-after-Read | Vorherigen Lesezugriff vor dem Überschreiben abschließen |
| WAW | Write-after-Write | Schreibzugriffe korrekt ordnen |

Dabei gilt:

- Überlappende Ressourcenbereiche MÜSSEN berücksichtigt werden.
- Unabhängige Lesezugriffe erzeugen grundsätzlich keinen Daten-Hazard.
- Zugriffe auf getrennte Ressourcenbereiche SOLLEN unabhängig bleiben.
- Fehlende Informationen MÜSSEN konservativ behandelt werden.

## 5. Resource State Tracking

NovaOS MUSS relevante Ressourcen- und Zugriffsstatus verfolgen.

Dazu gehören:

- Aktuelle und geplante Zugriffsarten.
- Erforderliche Ressourcenlayouts.
- Zugehörige GPU-Queues.
- Noch ausstehende Zugriffe.
- Letzte relevante Schreiboperationen.
- Aktive Leseoperationen.

Geplante und tatsächlich abgeschlossene Ressourcenverwendungen MÜSSEN unterscheidbar bleiben.

Ein eingereichter GPU-Auftrag DARF nicht automatisch als abgeschlossen betrachtet werden.

## 6. Fine-Grained Hazard Tracking

Hazards SOLLEN auf möglichst kleine Ressourcenbereiche begrenzt werden.

Unterstützt werden sollen:

- Buffer-Ranges.
- Texture-Mip-Level.
- Array-Layer.
- Image-Aspects.
- Überlappende Speicherbelegungen.

Bei nicht ausreichend präziser Nachverfolgung MUSS eine konservative Synchronisation erfolgen.

## 7. Cross-Queue Hazard Tracking

Nova GPU Resource Hazard Tracking MUSS queueübergreifende Ressourcenzugriffe erkennen.

Dabei gilt:

1. Zugriffe unterschiedlicher Queues werden gemeinsam analysiert.
2. Erforderliche Ausführungsabhängigkeiten werden erzeugt.
3. Queue Ownership Transfers werden bei Bedarf berücksichtigt.
4. Speicher- und Cache-Sichtbarkeit wird sichergestellt.
5. Unabhängige GPU-Aufträge bleiben möglichst parallel ausführbar.

Die Umsetzung erfolgt durch Nova GPU Resource Barriers und Nova Cross-Queue Synchronization.

## 8. Render Graph Integration

Nova GPU Resource Hazard Tracking MUSS mit Nova Render Graph zusammenarbeiten.

Dabei gilt:

- Render Passes deklarieren ihre Ressourcenverwendungen.
- Der Hazard Tracker erkennt daraus notwendige Abhängigkeiten.
- Der Render Graph integriert diese in seinen Dependency Graph.
- Resource Barriers werden anhand der erkannten Hazards geplant.
- Änderungen an Ressourcenverwendungen lösen eine erneute Bewertung betroffener Abhängigkeiten aus.

Die Hazard-Erkennung DARF keine zyklischen Ausführungsabhängigkeiten erzeugen.

## 9. Command Buffer Integration

NovaOS MUSS Ressourcenverwendungen innerhalb von Command Buffers berücksichtigen.

Dabei gilt:

- Deklarierte Ressourcenverwendungen werden vor der Submission validiert.
- Wiederverwendete Command Buffers MÜSSEN mit dem aktuellen Ressourcenzustand kompatibel sein.
- Parallele Command-Aufzeichnung DARF keine widersprüchlichen Ressourcenannahmen erzeugen.
- Nicht vollständig statisch bekannte Zugriffe MÜSSEN durch geeignete Synchronisationsregeln abgesichert werden.

## 10. Resource Aliasing

Nova GPU Resource Hazard Tracking MUSS überlappende GPU-Speicherbelegungen berücksichtigen.

Dabei gilt:

- Unterschiedliche Resource IDs können denselben physischen Speicher verwenden.
- Aliasing-Abhängigkeiten MÜSSEN erkannt werden.
- Erforderliche Aliasing Barriers werden erzeugt.
- Ressourcenlebensdauern MÜSSEN korrekt berücksichtigt werden.
- Gleichzeitige inkompatible Zugriffe auf überlappenden Speicher MÜSSEN verhindert werden.

## 11. Performance und Optimierung

NovaOS SOLL die Hazard-Verwaltung mit geringem CPU- und Speicheraufwand durchführen.

Dazu gehören:

- Inkrementelle Aktualisierung von Access Records.
- Zusammenfassung kompatibler Lesezugriffe.
- Entfernung abgeschlossener Zugriffsabhängigkeiten.
- Wiederverwendung gültiger Analyseergebnisse.
- Vermeidung unnötiger globaler GPU-Barriers.

Optimierungen DÜRFEN erforderliche Synchronisationsabhängigkeiten nicht entfernen.

## 12. Sicherheit und Fehlerbehandlung

Ressourcenzugriffe MÜSSEN vor ihrer Verwendung validiert werden.

Dabei gilt:

- Ressourcenhandles MÜSSEN gültig und autorisiert sein.
- Zugriffsrechte MÜSSEN capability-basiert geprüft werden.
- Ungültige oder widersprüchliche Ressourcenverwendungen MÜSSEN zurückgewiesen werden.
- Fehlgeschlagene Submissions DÜRFEN keine erfolgreichen Abschlusszustände erzeugen.
- Bei Geräteverlust MÜSSEN betroffene Tracking-Zustände invalidiert oder kontrolliert wiederhergestellt werden.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS GPU Resource Hazard Tracking bereitstellen.
2. RAW-, WAR- und WAW-Abhängigkeiten MÜSSEN erkannt werden.
3. Ressourcenbereiche MÜSSEN bei der Analyse berücksichtigt werden.
4. Geplante und abgeschlossene Zugriffe MÜSSEN unterscheidbar sein.
5. Cross-Queue Hazards MÜSSEN unterstützt werden.
6. Resource Aliasing MUSS berücksichtigt werden.
7. Render Graph MUSS erkannte Abhängigkeiten integrieren können.
8. GPU Resource Barriers MÜSSEN aus erkannten Hazards ableitbar sein.
9. Command Buffer Reuse MUSS berücksichtigt werden.
10. Unabhängige Zugriffe SOLLEN ohne unnötige Synchronisation möglich bleiben.
11. Hazard Tracking SOLL inkrementell erfolgen.
12. Unbekannte Zugriffsbedingungen MÜSSEN konservativ behandelt werden.
13. Ressourcenhandles und Zugriffsrechte MÜSSEN validiert werden.
14. Fehlerhafte Tracking-Zustände MÜSSEN kontrolliert behandelt werden.
15. Im deterministischen Modus MÜSSEN identische Ressourcenverwendungen dieselben logischen Abhängigkeiten erzeugen.

## 14. Ergebnis

Nova GPU Resource Hazard Tracking erkennt kritische Zugriffsabhängigkeiten zwischen GPU-Operationen und stellt die notwendigen Informationen für Synchronisation und Ausführungsplanung bereit.

Durch feingranulare Ressourcenanalyse, Cross-Queue-Unterstützung und inkrementelle Aktualisierung werden Datenkonflikte verhindert, während unabhängige GPU-Arbeit möglichst parallel ausführbar bleibt.

Damit bildet das Subsystem die Grundlage für eine korrekte und effiziente automatische GPU-Synchronisation innerhalb von NovaOS.
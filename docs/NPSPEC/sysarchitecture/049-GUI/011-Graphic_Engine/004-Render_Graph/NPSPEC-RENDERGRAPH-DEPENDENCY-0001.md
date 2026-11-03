# NPSPEC-RENDERGRAPH-DEPENDENCY-0001 – Nova Render Pass Dependencies

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Pass Dependencies definiert die Erkennung, Verwaltung und Validierung von Abhängigkeiten zwischen Render Passes innerhalb des Nova Render Graph.

Ziel ist die korrekte Ausführungsreihenfolge bei maximal möglicher Parallelisierung unabhängiger Rendering-Operationen.

Abhängigkeiten werden aus Ressourcenzugriffen, Datenflüssen und expliziten Ausführungsbedingungen abgeleitet.

## 2. Architekturprinzipien

- **Dependency-Driven:** Tatsächliche Abhängigkeiten bestimmen die Ausführungsreihenfolge.
- **Resource-Aware:** Ressourcen- und Subressourcenzugriffe werden berücksichtigt.
- **Hazard-Aware:** Konflikte zwischen Lese- und Schreiboperationen werden erkannt.
- **Incremental:** Unveränderte Abhängigkeiten können erhalten bleiben.
- **Parallel:** Unabhängige Render Passes dürfen gleichzeitig ausgeführt werden.
- **Minimal:** Unnötige Abhängigkeiten sollen vermieden werden.
- **Deterministic:** Identische Eingaben erzeugen reproduzierbare Abhängigkeitsgraphen.
- **Backend-Independent:** Die logische Abhängigkeitsanalyse bleibt hardwareunabhängig.

## 3. Dependency Model

Eine Render-Pass-Abhängigkeit beschreibt eine notwendige Ausführungsbeziehung zwischen zwei Rendering-Operationen.

Sie besitzt mindestens:

- **Source Pass:** Vorgelagerter Render Pass.
- **Target Pass:** Abhängiger Render Pass.
- **Dependency Type:** Art der Abhängigkeit.
- **Resource Reference:** Betroffene Ressource, sofern vorhanden.
- **Access Range:** Betroffener Ressourcenbereich.
- **Access Mode:** Relevante Lese- und Schreibzugriffe.

Abhängigkeiten bilden gerichtete Kanten innerhalb des Render Graph.

## 4. Dependency Types

NovaOS MUSS folgende Ressourcenabhängigkeiten berücksichtigen:

| Typ | Beschreibung |
|---|---|
| RAW – Read After Write | Ein Pass liest Daten, die zuvor geschrieben wurden. |
| WAR – Write After Read | Ein Pass überschreibt Daten, die zuvor gelesen werden müssen. |
| WAW – Write After Write | Mehrere Passes schreiben dieselbe Ressource. |
| Explicit Dependency | Eine ausdrücklich definierte Ausführungsbedingung. |

Read-after-Read (RAR) erzeugt grundsätzlich keine Schreibkonflikte.

Eine Abhängigkeit MUSS nur dann eine Ausführungsordnung erzwingen, wenn diese für die korrekte Verarbeitung erforderlich ist.

## 5. Resource Access Tracking

Der Dependency Resolver MUSS Ressourcenzugriffe anhand ihrer tatsächlichen Zugriffsbereiche analysieren.

Berücksichtigt werden:

- Buffer-Bereiche
- Textur-Mip-Levels
- Array-Layer
- Render-Target-Bereiche
- Depth- und Stencil-Aspekte
- Überlappende Speicherzuordnungen

Unabhängige Subressourcen DÜRFEN getrennt betrachtet werden.

Überlappende physische Speicherbereiche MÜSSEN auch bei unterschiedlichen logischen Ressourcenidentitäten berücksichtigt werden.

## 6. Dependency Resolution

Die Abhängigkeitsauflösung erfolgt grundsätzlich in folgenden Schritten:

1. Ressourcenzugriffe der Render Passes erfassen.
2. Überlappende Zugriffsbereiche bestimmen.
3. Daten- und Zugriffskonflikte identifizieren.
4. Erforderliche Ausführungsabhängigkeiten erzeugen.
5. Explizite Abhängigkeiten ergänzen.
6. Redundante Abhängigkeiten reduzieren.
7. Den resultierenden Graph validieren.

Die Auflösung MUSS vor der Erstellung des ausführbaren Render-Plans abgeschlossen sein.

## 7. Dependency Graph

Die ermittelten Abhängigkeiten bilden einen gerichteten azyklischen Graphen (DAG).

Dabei gilt:

- Knoten repräsentieren Render Passes.
- Kanten repräsentieren notwendige Ausführungsbedingungen.
- Unabhängige Knoten können parallel verarbeitet werden.
- Abhängige Knoten müssen ihre Voraussetzungen erfüllen.

Zyklische Abhängigkeiten MÜSSEN erkannt werden.

Ein Graph mit nicht auflösbaren Zyklen DARF nicht ausgeführt werden.

## 8. Dependency Minimization

NovaOS SOLL unnötige Abhängigkeiten vermeiden.

Dazu gehören:

- Redundante direkte Abhängigkeiten
- Nicht überlappende Ressourcenzugriffe
- Unabhängige Subressourcen
- Bereits durch andere Abhängigkeiten abgesicherte Ausführungsbedingungen

Die Reduzierung DARF keine erforderliche Ausführungsordnung entfernen.

Eine möglichst geringe Anzahl von Abhängigkeiten SOLL die Parallelisierung verbessern.

## 9. Incremental Dependency Tracking

NovaOS SOLL Abhängigkeiten zwischen Graph-Aktualisierungen wiederverwenden.

Bei Änderungen werden insbesondere überprüft:

- Neue oder entfernte Render Passes
- Veränderte Ressourcenzugriffe
- Geänderte Subressourcenbereiche
- Neue Ressourcenidentitäten
- Veränderte explizite Abhängigkeiten

Unveränderte Graphbereiche SOLLEN nicht unnötig erneut analysiert werden.

## 10. Synchronization Integration

Logische Render-Pass-Abhängigkeiten werden durch Nova Render Resource Barriers und Nova GPU Synchronization in konkrete Synchronisationsoperationen übersetzt.

Dabei MUSS zwischen folgenden Aufgaben unterschieden werden:

- **Dependency Resolution:** Bestimmt notwendige Ausführungsbeziehungen.
- **Synchronization Planning:** Bestimmt erforderliche Speicher- und Ausführungssynchronisation.
- **GPU Backend:** Setzt die Synchronisation hardwaregerecht um.

Eine logische Abhängigkeit MUSS nicht zwangsläufig einer einzelnen GPU-Barrier entsprechen.

Ausführungsreihenfolge allein DARF nicht als Ersatz für erforderliche Speicher-Sichtbarkeit verwendet werden.

## 11. Parallel Execution

Render Passes ohne gegenseitige Abhängigkeiten DÜRFEN parallel vorbereitet und ausgeführt werden.

Der Graph Scheduler MUSS:

- Abhängigkeiten respektieren.
- Ressourcenverfügbarkeit berücksichtigen.
- Synchronisationsanforderungen einhalten.
- Backend-Beschränkungen beachten.

Parallelisierung DARF keine Datenrennen oder ungültigen Rendering-Ergebnisse verursachen.

## 12. External Resources

Externe Ressourcen können zusätzliche Synchronisationsanforderungen besitzen.

Dazu gehören:

- Importierte Surface-Buffer
- Shared GPU Resources
- Externe Rendering-Ergebnisse
- Display- und Compositor-Buffer

NovaOS MUSS erforderliche Übergaben und externe Synchronisationsbedingungen berücksichtigen.

Die Verfügbarkeit einer externen Ressource DARF nicht ohne gültige Synchronisationsinformationen vorausgesetzt werden.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Abhängigkeiten erkennen.

Dazu gehören:

- Zyklische Abhängigkeiten
- Fehlende Ressourcenproduzenten
- Ungültige Ressourcenzugriffe
- Widersprüchliche Ausführungsbedingungen
- Nicht erfüllbare externe Synchronisationsbedingungen

Bei fehlgeschlagener Abhängigkeitsauflösung DARF kein unsicherer Ausführungsplan erzeugt werden.

Ein konservativer Ausführungsplan DARF verwendet werden, sofern sämtliche Daten- und Synchronisationsanforderungen erfüllt werden können.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Render-Pass-Abhängigkeiten automatisch ermitteln können.
2. RAW-, WAR- und WAW-Abhängigkeiten MÜSSEN berücksichtigt werden.
3. Explizite Abhängigkeiten MÜSSEN unterstützt werden.
4. Ressourcenzugriffe MÜSSEN hinsichtlich ihrer Überlappung analysiert werden.
5. Unabhängige Subressourcen SOLLEN getrennt behandelt werden.
6. Der Abhängigkeitsgraph MUSS azyklisch sein.
7. Zyklische Abhängigkeiten MÜSSEN erkannt werden.
8. Unnötige Abhängigkeiten SOLLEN reduziert werden.
9. Unveränderte Abhängigkeiten SOLLEN wiederverwendet werden.
10. Unabhängige Render Passes DÜRFEN parallel ausgeführt werden.
11. Logische Abhängigkeiten MÜSSEN in erforderliche Synchronisationsmaßnahmen übersetzbar sein.
12. Externe Ressourcenabhängigkeiten MÜSSEN berücksichtigt werden.
13. Die Abhängigkeitsanalyse MUSS unabhängig vom konkreten GPU-Backend bleiben.
14. Identische Eingaben MÜSSEN im deterministischen Modus identische Abhängigkeitsgraphen erzeugen.
15. Ungültige Abhängigkeitsgraphen DÜRFEN nicht ausgeführt werden.

## 15. Ergebnis

Nova Render Pass Dependencies stellt die korrekte Daten- und Ausführungsordnung innerhalb des Nova Render Graph sicher.

Durch präzise Ressourcenzugriffsanalyse, inkrementelle Abhängigkeitsverwaltung und die Reduzierung unnötiger Ausführungsbedingungen ermöglicht NovaOS eine effiziente Parallelisierung von Rendering-Operationen.

Damit bildet das Subsystem die Grundlage für sicheres Render Graph Scheduling, GPU Synchronization und die Optimierung grafischer Verarbeitung.
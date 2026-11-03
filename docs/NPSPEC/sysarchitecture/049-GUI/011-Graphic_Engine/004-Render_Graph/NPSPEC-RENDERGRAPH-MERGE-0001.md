# NPSPEC-RENDERGRAPH-MERGE-0001 – Nova Render Pass Merging

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Pass Merging definiert die automatische Zusammenfassung kompatibler Render Passes innerhalb des Nova Render Graph.

Ziel ist die Reduzierung von Rendering-Overhead, Ressourcenwechseln, Speicheroperationen und GPU-Synchronisation.

Die Zusammenfassung erfolgt ausschließlich, wenn die korrekte Darstellung und sämtliche Ressourcenabhängigkeiten erhalten bleiben.

## 2. Architekturprinzipien

- **Dependency-Aware:** Zusammenfassungen berücksichtigen sämtliche Ausführungsabhängigkeiten.
- **Compatibility-Based:** Nur kompatible Rendering-Operationen werden zusammengeführt.
- **Cost-Aware:** Merging erfolgt anhand des erwarteten Gesamtaufwands.
- **Resource-Aware:** Ressourcenformate, Zugriffe und Lebenszeiten werden berücksichtigt.
- **Incremental:** Unveränderte Merge-Entscheidungen können wiederverwendet werden.
- **Backend-Aware:** Hardwareabhängige Optimierungsmöglichkeiten werden genutzt.
- **Deterministic:** Identische Eingaben ermöglichen reproduzierbare Merge-Entscheidungen.
- **Fallback-Safe:** Einzelne Render Passes bleiben jederzeit ausführbar.

## 3. Merge Model

Eine Merge Group beschreibt mehrere logisch eigenständige Render Passes, die gemeinsam ausgeführt werden können.

Sie enthält mindestens:

- **Merge Group ID:** Eindeutige Identifikation.
- **Pass IDs:** Zugeordnete Render Passes.
- **Execution Order:** Erforderliche interne Ausführungsreihenfolge.
- **Resource Requirements:** Gemeinsame Ressourcenanforderungen.
- **Dependency Signature:** Relevante Abhängigkeiten.
- **Backend Requirements:** Voraussetzungen für die gemeinsame Ausführung.

Die ursprünglichen logischen Pass-Identitäten MÜSSEN erhalten bleiben.

## 4. Merge Compatibility

Render Passes DÜRFEN zusammengefasst werden, wenn:

1. Ihre Ausführungsanforderungen kompatibel sind.
2. Sämtliche Ressourcenabhängigkeiten erhalten bleiben.
3. Ihre Rendering-Ziele kompatibel sind.
4. Erforderliche Synchronisation weiterhin möglich ist.
5. Keine unzulässigen Ressourcenkonflikte entstehen.
6. Die Zusammenfassung vom Backend unterstützt wird.

Eine gemeinsame Nutzung desselben Render Targets allein reicht nicht zur Feststellung der Kompatibilität aus.

## 5. Merge Strategies

NovaOS SOLL folgende Strategien unterstützen können:

| Strategie | Beschreibung |
|---|---|
| Pass Fusion | Zusammenfassung kompatibler Rendering-Operationen |
| Subpass Merging | Gemeinsame Verarbeitung kompatibler Attachment-Zugriffe |
| Command Consolidation | Zusammenfassung geeigneter Befehlsfolgen |
| Effect Pass Merging | Kombination kompatibler Effektoperationen |
| Region Merging | Gemeinsame Verarbeitung kompatibler Rendering-Regionen |

Die Strategien DÜRFEN kombiniert werden.

Eine logische Merge Group MUSS nicht zwingend einem einzelnen nativen GPU-Render-Pass entsprechen.

## 6. Dependency Preservation

Der Merge Planner MUSS sämtliche relevanten Abhängigkeiten prüfen.

Dabei gilt:

- RAW-, WAR- und WAW-Abhängigkeiten müssen erhalten bleiben.
- Explizite Ausführungsbedingungen müssen berücksichtigt werden.
- Externe Abhängigkeiten dürfen nicht verloren gehen.
- Erforderliche Speicherbarrieren müssen weiterhin ausführbar sein.

Eine Zusammenfassung DARF keine zusätzlichen Datenrennen erzeugen.

## 7. Resource Compatibility

Beim Zusammenführen werden insbesondere geprüft:

- Render-Target-Formate
- Attachment-Konfigurationen
- Sample Counts
- Load- und Store-Operationen
- Ressourcenlayouts
- Subressourcenzugriffe
- Ressourcenlebenszeiten

Merging DARF keine erforderlichen Zwischenergebnisse überschreiben oder ungültig machen.

## 8. Cost-Based Merging

Nova Rendering Cost Estimation SOLL die Wirtschaftlichkeit einer Zusammenfassung bewerten.

Berücksichtigt werden:

- Eingesparte Pass-Wechsel
- Reduzierte Command-Submission-Kosten
- Vermiedene Speicheroperationen
- Zusätzliche Synchronisation
- Veränderter Ressourcenbedarf
- Auswirkungen auf parallele Ausführung

Eine Zusammenfassung SOLL unterbleiben, wenn ihre erwarteten Gesamtkosten höher sind als die getrennte Ausführung.

## 9. Parallelism Preservation

Render Pass Merging kann die mögliche Parallelisierung reduzieren.

NovaOS MUSS deshalb prüfen, ob durch eine Zusammenfassung unabhängige Rendering-Operationen unnötig serialisiert werden.

Die Einsparung von Pass-Overhead SOLL gegen mögliche Verluste an Parallelität abgewogen werden.

## 10. Incremental Merging

NovaOS SOLL gültige Merge Groups zwischen Graph-Aktualisierungen erhalten.

Eine erneute Prüfung ist erforderlich bei:

- Veränderten Render-Pass-Abhängigkeiten
- Geänderten Ressourcenanforderungen
- Neuen oder entfernten Passes
- Veränderten Rendering-Zielen
- Geänderten Backend-Funktionen

Unveränderte Merge Groups SOLLEN nicht unnötig neu erstellt werden.

## 11. Damage-Aware Merging

Nova Damage Tracking stellt Informationen über betroffene Rendering-Regionen bereit.

Der Merge Planner SOLL diese Informationen berücksichtigen.

Dabei können kompatible regionale Rendering-Operationen zusammengefasst werden.

Eine Zusammenfassung DARF jedoch keine unnötige vollständige Neuberechnung erzwingen, wenn getrennte Partial-Rendering-Operationen effizienter sind.

## 12. Backend Integration

Der Nova Render Graph entscheidet über die logische Zusammenfassung.

Die Nova Render Engine setzt diese entsprechend den Fähigkeiten des jeweiligen Backends um.

Dabei gilt:

- Native Subpasses DÜRFEN verwendet werden.
- Dynamic Rendering DARF verwendet werden.
- Geeignete Command Buffers DÜRFEN gemeinsam aufgezeichnet werden.
- Nicht unterstützte Merge-Verfahren MÜSSEN durch getrennte Pass-Ausführung ersetzt werden können.

Die logische Graphstruktur MUSS unabhängig von einer bestimmten GPU-API bleiben.

## 13. Fehlerbehandlung

NovaOS MUSS ungültige Merge-Konfigurationen erkennen.

Dazu gehören:

- Inkompatible Ressourcen
- Verletzte Ausführungsabhängigkeiten
- Nicht unterstützte Backend-Funktionen
- Ungültige Synchronisationsbedingungen
- Widersprüchliche Attachment-Anforderungen

Bei fehlgeschlagener Zusammenfassung MÜSSEN die ursprünglichen Render Passes weiterhin einzeln ausführbar sein.

Ungültige Merge Groups DÜRFEN nicht ausgeführt werden.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS kompatible Render Passes zusammenfassen können.
2. Sämtliche Ressourcen- und Ausführungsabhängigkeiten MÜSSEN erhalten bleiben.
3. Die Kompatibilität MUSS vor einer Zusammenfassung geprüft werden.
4. Logische Pass-Identitäten MÜSSEN nachvollziehbar bleiben.
5. Ressourcenformate und Attachment-Anforderungen MÜSSEN berücksichtigt werden.
6. Erforderliche Synchronisation DARF nicht entfernt werden.
7. Merging SOLL anhand der erwarteten Gesamtkosten bewertet werden.
8. Auswirkungen auf parallele Ausführung MÜSSEN berücksichtigt werden.
9. Gültige Merge Groups SOLLEN inkrementell wiederverwendet werden.
10. Damage Regions SOLLEN bei der Optimierung berücksichtigt werden.
11. Backendabhängige Merge-Verfahren DÜRFEN verwendet werden.
12. Die Architektur MUSS unterschiedliche Rendering-Backends unterstützen.
13. Ein deterministischer Merge-Modus MUSS verfügbar sein.
14. Ungültige Merge Groups DÜRFEN nicht ausgeführt werden.
15. Die getrennte Ausführung ursprünglicher Render Passes MUSS als Fallback verfügbar bleiben.

## 15. Ergebnis

Nova Render Pass Merging reduziert den Verwaltungs- und Ausführungsaufwand des Nova Render Graph durch die Zusammenfassung kompatibler Rendering-Operationen.

Durch Abhängigkeitsanalyse, Ressourcenprüfung und kostenbasierte Optimierung können unnötige Pass-Wechsel, Speicheroperationen und Synchronisationsschritte vermieden werden.

Dabei bleiben die logische Struktur, die korrekte Ausführung und die Möglichkeit zur parallelen Verarbeitung erhalten.
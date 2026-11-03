# NPSPEC-RENDERGRAPH-PASS-0001 – Nova Render Pass Management

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Nova Render Pass Management definiert die Erstellung, Verwaltung, Validierung und Wiederverwendung logischer Rendering-Aufgaben innerhalb des Nova Render Graph.

Ein Render Pass beschreibt eine abgegrenzte Grafikoperation mit definierten Eingaben, Ausgaben und Ressourcenanforderungen.

Ziel ist die effiziente Organisation von Rendering-Operationen bei minimalem Verwaltungsaufwand.

## 2. Architekturprinzipien

- **Declarative:** Render Passes beschreiben ihre Anforderungen und Operationen.
- **Dependency-Aware:** Ressourcenabhängigkeiten bestimmen die Ausführungsreihenfolge.
- **Incremental:** Unveränderte Pass-Beschreibungen können erhalten bleiben.
- **Cache-Aware:** Gültige Pass-Ergebnisse können wiederverwendet werden.
- **Resource-Aware:** Ressourcenlebenszeiten werden berücksichtigt.
- **Composable:** Kompatible Render Passes können zusammengefasst werden.
- **Parallel:** Unabhängige Render Passes können gleichzeitig ausgeführt werden.
- **Backend-Independent:** Logische Passes bleiben unabhängig von konkreten GPU-Schnittstellen.

## 3. Render Pass Model

Ein Render Pass besitzt mindestens:

- **Pass ID:** Eindeutige Identifikation innerhalb des Graphen.
- **Pass Type:** Art der Rendering-Operation.
- **Inputs:** Gelesene Ressourcen.
- **Outputs:** Geschriebene Ressourcen.
- **Resource Access:** Zugriffsarten und betroffene Ressourcenbereiche.
- **Execution Requirements:** Erforderliche Hardware- und Backend-Funktionen.
- **Execution Definition:** Beschreibung der auszuführenden Operation.

Optionale Eigenschaften:

- Damage Regions
- Rendering-Kosten
- Cache-Informationen
- Qualitätsparameter
- Parallelisierungsanforderungen

Ein logischer Render Pass MUSS nicht einem einzelnen nativen GPU-Render-Pass entsprechen.

## 4. Render Pass Types

NovaOS SOLL folgende Pass-Typen unterstützen:

| Pass Type | Aufgabe |
|---|---|
| Graphics Pass | Rasterisierung grafischer Inhalte |
| Compute Pass | GPU-basierte Berechnungen |
| Copy Pass | Kopieren von Grafikressourcen |
| Resolve Pass | Auflösung multisampelbasierter Ergebnisse |
| Clear Pass | Initialisierung definierter Ressourcenbereiche |
| Effect Pass | Verarbeitung grafischer Effekte |
| Composition Pass | Vorbereitung oder Berechnung von Kompositionsergebnissen |

Weitere Pass-Typen DÜRFEN ergänzt werden.

Spezialisierte Pass-Typen DÜRFEN intern auf grundlegende GPU-Operationen abgebildet werden.

## 5. Pass Lifecycle

Ein Render Pass durchläuft folgende logische Zustände:

1. **Declared:** Pass-Beschreibung wurde registriert.
2. **Validated:** Ressourcen und Anforderungen wurden geprüft.
3. **Scheduled:** Ausführungsposition wurde bestimmt.
4. **Executable:** Erforderliche Ressourcen und Befehle sind vorbereitet.
5. **Submitted:** Ausführung wurde übermittelt.
6. **Completed:** Ausführung wurde erfolgreich abgeschlossen.

Zusätzlich sind folgende Zustände möglich:

- **Culled:** Pass wird nicht benötigt.
- **Reused:** Ein gültiges Ergebnis ersetzt die erneute Ausführung.
- **Failed:** Pass konnte nicht erfolgreich abgeschlossen werden.

Ein Pass-Ergebnis DARF erst nach Abschluss erforderlicher Schreiboperationen als gültig veröffentlicht werden.

## 6. Pass Declaration

Render Passes MÜSSEN ihre Ressourcenanforderungen vor der Ausführungsplanung deklarieren.

Dabei sind insbesondere anzugeben:

- Gelesene Ressourcen
- Geschriebene Ressourcen
- Zugriffsarten
- Betroffene Subressourcen oder Regionen
- Erforderliche Ressourcenformate
- Initialisierungs- und Erhaltungsanforderungen

Nicht deklarierte Ressourcenzugriffe DÜRFEN die Sicherheit der Ausführungsplanung nicht umgehen.

## 7. Pass Validation

Der Render Pass Manager MUSS Pass-Beschreibungen vor ihrer Ausführung validieren.

Geprüft werden insbesondere:

- Gültige Ressourcenreferenzen
- Unterstützte Pass-Typen
- Kompatible Ressourcenformate
- Zulässige Zugriffsarten
- Erforderliche Hardwarefunktionen
- Vollständige Eingabe- und Ausgabeinformationen

Ungültige Render Passes DÜRFEN nicht ausgeführt werden.

## 8. Pass Dependencies

Abhängigkeiten entstehen durch Ressourcen- und Ausführungsbedingungen.

Der Render Pass Manager stellt die erforderlichen Informationen für Nova Render Pass Dependencies bereit.

Dabei gilt:

- Schreibende Zugriffe können nachfolgende Leser beeinflussen.
- Überlappende Schreibzugriffe benötigen eine definierte Ordnung.
- Unabhängige Passes können parallel verarbeitet werden.
- Explizite Abhängigkeiten müssen berücksichtigt werden.

Die endgültige Abhängigkeitsauflösung erfolgt durch den Dependency Resolver.

## 9. Incremental Pass Management

NovaOS SOLL unveränderte Pass-Beschreibungen zwischen Graph-Aktualisierungen erhalten.

Bei Änderungen werden ausschließlich betroffene Passes aktualisiert.

Dabei MUSS zwischen folgenden Fällen unterschieden werden:

- Pass-Beschreibung unverändert
- Pass-Parameter verändert
- Ressourcenabhängigkeiten verändert
- Pass-Ergebnis ungültig
- Pass nicht mehr erforderlich

Eine unveränderte Pass-Beschreibung garantiert nicht die Gültigkeit ihres Rendering-Ergebnisses.

## 10. Pass Culling und Result Reuse

Nicht benötigte Render Passes SOLLEN aus dem ausführbaren Graph entfernt werden.

Ein Pass kann entfallen, wenn:

- Sein Ergebnis nicht benötigt wird.
- Ein gültiges Cache-Ergebnis vorhanden ist.
- Seine Ausgabe vollständig irrelevant ist.
- Keine erforderlichen Seiteneffekte bestehen.

Passes mit notwendigen externen Seiteneffekten DÜRFEN nicht allein aufgrund ungenutzter Grafikausgaben entfernt werden.

## 11. Pass Merging

Kompatible Render Passes DÜRFEN zusammengefasst werden.

Voraussetzungen sind:

- Kompatible Rendering-Ziele
- Vereinbare Ressourcenanforderungen
- Erhaltene Ausführungsabhängigkeiten
- Korrekte Synchronisation
- Unveränderte Darstellungsergebnisse

Die Zusammenfassung erfolgt durch Nova Render Pass Merging.

Die logischen Pass-Identitäten SOLLEN für Diagnose und Profiling nachvollziehbar bleiben.

## 12. Parallel Execution

Unabhängige Render Passes DÜRFEN parallel vorbereitet oder ausgeführt werden.

Dabei MUSS NovaOS:

- Ressourcenkonflikte verhindern.
- Abhängigkeiten einhalten.
- Backend-Beschränkungen berücksichtigen.
- Synchronisationsanforderungen erfüllen.

Die konkrete Ausführungsplanung erfolgt durch Nova Render Graph Scheduling.

## 13. Resource Management

Der Render Pass Manager verwaltet logische Ressourcenreferenzen.

Die physische Speicherverwaltung erfolgt durch den Nova GPU Resource Manager.

Dabei MÜSSEN:

- Ressourcenlebenszeiten berücksichtigt werden.
- Temporäre Ressourcen korrekt zugeordnet werden.
- Schreib- und Lesezugriffe nachvollziehbar bleiben.
- Ressourcen bis zum Abschluss ihrer Verwendung gültig bleiben.

Ein Pass DARF keine Ressourcen vorzeitig freigeben, die von anderen Passes benötigt werden.

## 14. Damage-Aware Passes

Render Passes SOLLEN optionale Damage Regions unterstützen.

Dadurch können:

- Pass-Ausgaben räumlich begrenzt werden.
- Unveränderte Bereiche erhalten bleiben.
- Unnötige Rendering-Operationen entfallen.
- Regionale Cache-Ergebnisse wiederverwendet werden.

Effektabhängigkeiten außerhalb der unmittelbaren Damage Region MÜSSEN berücksichtigt werden.

## 15. Fehlerbehandlung

Fehlgeschlagene Render Passes DÜRFEN keine ungültigen Ergebnisse veröffentlichen.

NovaOS MUSS:

- Fehlerhafte Pass-Beschreibungen erkennen.
- Nicht unterstützte Operationen behandeln.
- Abhängige Passes bei fehlenden Eingaben kontrolliert behandeln.
- Fehlerzustände nachvollziehbar bereitstellen.
- Geeignete Fallback-Verfahren ermöglichen.

Ein fehlgeschlagener Pass DARF nicht automatisch als erfolgreich abgeschlossen gelten.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS Render Passes über definierte Schnittstellen verwalten.
2. Jeder Render Pass MUSS eindeutig identifizierbar sein.
3. Render Passes MÜSSEN ihre Ressourcen und Zugriffsarten deklarieren.
4. Pass-Beschreibungen MÜSSEN vor ihrer Ausführung validiert werden.
5. Logische Render Passes MÜSSEN unabhängig von nativen GPU-Render-Passes sein.
6. Ressourcenabhängigkeiten MÜSSEN ermittelbar sein.
7. Unveränderte Pass-Beschreibungen SOLLEN wiederverwendet werden.
8. Gültige Pass-Ergebnisse SOLLEN erneute Ausführungen vermeiden.
9. Nicht benötigte Render Passes SOLLEN entfernt werden.
10. Kompatible Passes DÜRFEN zusammengefasst werden.
11. Unabhängige Passes DÜRFEN parallel verarbeitet werden.
12. Ressourcenlebenszeiten MÜSSEN berücksichtigt werden.
13. Damage Regions SOLLEN zur Begrenzung der Verarbeitung verwendet werden können.
14. Unvollständige Pass-Ergebnisse DÜRFEN nicht als gültig veröffentlicht werden.
15. Fehlerhafte Passes MÜSSEN kontrolliert behandelt werden.
16. Die Verwaltung MUSS unterschiedliche Rendering-Backends unterstützen.

## 17. Ergebnis

Nova Render Pass Management stellt die zentrale Verwaltung logischer Rendering-Aufgaben innerhalb des Nova Render Graph bereit.

Durch deklarierte Ressourcenanforderungen, Validierung, inkrementelle Aktualisierung und Wiederverwendung können Rendering-Aufträge effizient organisiert werden.

Die klare Trennung zwischen Pass-Verwaltung, Abhängigkeitsauflösung, Ressourcenplanung und tatsächlicher Ausführung ermöglicht eine modulare und hardwareunabhängige Rendering-Architektur.
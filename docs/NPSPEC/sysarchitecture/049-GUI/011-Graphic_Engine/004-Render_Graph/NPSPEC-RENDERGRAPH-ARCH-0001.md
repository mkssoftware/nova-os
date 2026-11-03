# NPSPEC-RENDERGRAPH-ARCH-0001 – Nova Render Graph Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Graph

## 1. Zweck

Die Nova Render Graph Architecture definiert die Organisation, Abhängigkeitsverwaltung und Ausführungsplanung grafischer Rendering-Operationen innerhalb von NovaOS.

Der Render Graph beschreibt, welche Rendering-Operationen erforderlich sind, welche Ressourcen sie verwenden und in welcher Reihenfolge sie ausgeführt werden dürfen.

Ziel ist die automatische Optimierung von Rendering-Abläufen durch die Vermeidung unnötiger Operationen, effiziente Ressourcennutzung und Parallelisierung unabhängiger Aufgaben.

## 2. Architekturprinzipien

- **Graph-Based:** Rendering-Operationen werden als gerichteter Abhängigkeitsgraph organisiert.
- **Declarative:** Render Passes beschreiben ihre Ressourcen und Operationen.
- **Dependency-Aware:** Ausführungsreihenfolgen ergeben sich aus tatsächlichen Abhängigkeiten.
- **Incremental:** Unveränderte Graphstrukturen und Ergebnisse können wiederverwendet werden.
- **Resource-Aware:** Ressourcenlebenszeiten und Speicherbedarf werden berücksichtigt.
- **Parallel:** Unabhängige Rendering-Operationen können gleichzeitig ausgeführt werden.
- **Adaptive:** Die Ausführungsplanung berücksichtigt Hardware und Arbeitslast.
- **Backend-Independent:** Der Graph bleibt unabhängig von konkreten GPU-Schnittstellen.
- **Deterministic:** Identische Eingaben und Konfigurationen ermöglichen reproduzierbare Ausführungspläne.

## 3. Architekturmodell

Der Nova Render Graph besteht aus folgenden Komponenten:

- **Render Graph Builder:** Erstellt und aktualisiert den Graph.
- **Render Pass Manager:** Verwaltet Rendering-Operationen.
- **Dependency Resolver:** Ermittelt Ressourcen- und Ausführungsabhängigkeiten.
- **Resource Planner:** Plant Ressourcenlebenszeiten und Speicherverwendung.
- **Graph Optimizer:** Entfernt und kombiniert unnötige Operationen.
- **Graph Scheduler:** Bestimmt die Ausführungsreihenfolge.
- **Synchronization Planner:** Ermittelt erforderliche Synchronisationsoperationen.
- **Execution Backend:** Übergibt den Ausführungsplan an die Nova Render Engine.

Der Render Graph plant die Verarbeitung, führt GPU-Befehle jedoch nicht selbst aus.

## 4. Graph-Modell

Der Nova Render Graph wird als gerichteter azyklischer Graph (DAG) modelliert.

Ein Graph enthält:

- **Pass Nodes:** Auszuführende Rendering-Operationen.
- **Resource Nodes:** Verwendete logische Grafikressourcen.
- **Dependency Edges:** Daten- und Ausführungsabhängigkeiten.
- **Execution Metadata:** Informationen zur Planung und Optimierung.

Die Ausführungsabhängigkeiten zwischen Render Passes MÜSSEN eine azyklische Ordnung bilden.

Zyklische Abhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.

## 5. Render Pass Model

Ein Render Pass beschreibt eine logisch abgeschlossene Rendering-Aufgabe.

Er besitzt mindestens:

- **Pass ID:** Eindeutige Identifikation.
- **Pass Type:** Graphics, Compute, Copy oder unterstützter Spezialtyp.
- **Inputs:** Gelesene Ressourcen.
- **Outputs:** Geschriebene Ressourcen.
- **Access Types:** Art der Ressourcenzugriffe.
- **Execution Requirements:** Benötigte Backend-Funktionen.
- **Execution Callback:** Ausführbare Rendering-Operation.

Optionale Informationen umfassen:

- Damage Regions
- Rendering-Kosten
- Cache-Abhängigkeiten
- Parallelisierungsanforderungen
- Qualitätsparameter

Ein logischer Render Pass MUSS nicht zwingend einem einzelnen nativen GPU-Render-Pass entsprechen.

## 6. Ressourcenmodell

Der Render Graph verwaltet logische Ressourcenreferenzen.

Dazu gehören:

- Texturen
- Render Targets
- Storage Buffers
- Depth- und Stencil-Buffer
- Temporäre Zwischenergebnisse
- Externe Surface-Buffer

Ressourcen können als **Imported**, **Transient** oder **Persistent** klassifiziert werden.

Der Render Graph MUSS zwischen logischer Ressourcenidentität und physischer Speicherzuordnung unterscheiden.

Die eigentliche Speicherverwaltung erfolgt über den Nova GPU Resource Manager.

## 7. Dependency Management

Abhängigkeiten entstehen insbesondere durch:

- Read-after-Write (RAW)
- Write-after-Read (WAR)
- Write-after-Write (WAW)
- Explizite Ausführungsbedingungen

Der Dependency Resolver MUSS relevante Ressourcenzugriffe einschließlich überlappender Teilbereiche berücksichtigen.

Unabhängige Render Passes DÜRFEN parallel ausgeführt werden.

Unvollständige oder widersprüchliche Abhängigkeiten DÜRFEN nicht zu unkontrollierten Ressourcenzugriffen führen.

## 8. Graph Construction

Die Graph-Erstellung erfolgt grundsätzlich durch:

1. Übernahme der Rendering-Anforderungen.
2. Registrierung benötigter Render Passes.
3. Deklaration der Ressourcen und Zugriffsarten.
4. Ermittlung der Abhängigkeiten.
5. Validierung des Graphen.
6. Optimierung der Graphstruktur.
7. Planung von Ressourcen und Synchronisation.
8. Erstellung eines ausführbaren Plans.

Der Graph DARF bei Änderungen inkrementell aktualisiert werden.

## 9. Graph Optimization

Der Nova Render Graph SOLL folgende Optimierungen unterstützen:

- **Pass Culling:** Entfernen nicht benötigter Render Passes.
- **Pass Merging:** Zusammenfassen kompatibler Operationen.
- **Result Reuse:** Wiederverwendung gültiger Rendering-Ergebnisse.
- **Resource Aliasing:** Gemeinsame Speichernutzung bei nicht überlappenden Ressourcenlebenszeiten.
- **Parallel Scheduling:** Gleichzeitige Ausführung unabhängiger Operationen.
- **Barrier Optimization:** Reduzierung unnötiger Synchronisationsoperationen.

Optimierungen MÜSSEN sämtliche relevanten Ressourcen- und Ausführungsabhängigkeiten erhalten.

## 10. Incremental Render Graph

Der Render Graph MUSS inkrementelle Verarbeitung unterstützen können.

Bei Änderungen SOLL NovaOS:

- Unveränderte Pass-Beschreibungen erhalten.
- Nur betroffene Abhängigkeiten aktualisieren.
- Gültige Pass-Ergebnisse wiederverwenden.
- Unnötige Pass-Ausführungen vermeiden.
- Bestehende Ressourcenbelegungen nach Möglichkeit erhalten.

Eine Änderung des Scene Graph DARF nicht grundsätzlich die vollständige Neuerstellung des Render Graph erzwingen.

Die Gültigkeit wiederverwendeter Graphstrukturen und Ergebnisse MUSS überprüfbar sein.

## 11. Resource Lifetime und Aliasing

Der Resource Planner bestimmt die erforderliche Lebensdauer temporärer Ressourcen.

Speicherbereiche DÜRFEN mehrfach verwendet werden, wenn:

- Die Ressourcenlebenszeiten nicht überlappen.
- Keine ausstehenden GPU-Zugriffe bestehen.
- Format- und Speicheranforderungen kompatibel sind.
- Erforderliche Synchronisation gewährleistet ist.

Imported Resources DÜRFEN nicht ohne entsprechende Berechtigung freigegeben oder überschrieben werden.

## 12. Scheduling und Synchronisation

Der Graph Scheduler erstellt einen gültigen Ausführungsplan.

Dabei berücksichtigt er:

- Pass-Abhängigkeiten
- Verfügbare GPU Queues
- Hardwarefähigkeiten
- Geschätzte Rendering-Kosten
- Ressourcenverfügbarkeit
- Synchronisationsanforderungen

Der Synchronization Planner bestimmt erforderliche Barriers, Fences und Semaphores.

Die konkrete Umsetzung erfolgt durch das jeweilige GPU-Backend.

## 13. Integration mit Damage Tracking

Nova Damage Tracking bestimmt räumlich betroffene Bildbereiche.

Der Render Graph MUSS diese Informationen berücksichtigen können.

Dadurch können:

- Nicht betroffene Render Passes entfallen.
- Pass-Ausgaben räumlich begrenzt werden.
- Gültige Zwischenergebnisse erhalten bleiben.
- Materialabhängigkeiten gezielt aktualisiert werden.

Bei Effekten wie Blur, Refraction und Liquid Glass MÜSSEN auch Abhängigkeiten außerhalb der unmittelbaren Damage Region berücksichtigt werden.

## 14. Integration mit der Render Engine

Die Verantwortlichkeiten sind eindeutig getrennt:

- **Scene Graph:** Beschreibt den persistenten grafischen Zustand.
- **Damage Tracking:** Bestimmt betroffene Bildbereiche.
- **Render Graph:** Organisiert erforderliche Rendering-Operationen.
- **Render Engine:** Führt den Ausführungsplan aus.
- **GPU Resource Manager:** Verwaltet physische Grafikressourcen.
- **GPU Synchronization:** Stellt Synchronisationsmechanismen bereit.
- **Compositor:** Kombiniert fertige Oberflächen.

Der Render Graph DARF keine Fensterverwaltung oder direkte Bildschirmausgabe übernehmen.

## 15. Fehlerbehandlung

NovaOS MUSS ungültige Graphstrukturen vor ihrer Ausführung erkennen.

Dazu gehören:

- Zyklische Abhängigkeiten
- Fehlende Ressourcen
- Ungültige Ressourcenzugriffe
- Nicht unterstützte Pass-Typen
- Unvereinbare Ressourcenformate
- Nicht erfüllbare Synchronisationsbedingungen

Bei fehlgeschlagener Optimierung MUSS ein gültiger konservativer Ausführungsplan möglich sein.

Ungültige oder unvollständige Rendering-Ergebnisse DÜRFEN nicht veröffentlicht werden.

## 16. Verbindliche Anforderungen

1. NovaOS MUSS einen Render Graph zur Organisation grafischer Rendering-Operationen bereitstellen.
2. Render Passes MÜSSEN ihre Ressourcen und Zugriffsarten deklarieren.
3. Ausführungsabhängigkeiten MÜSSEN einen azyklischen Graphen bilden.
4. Ressourcenabhängigkeiten MÜSSEN vor der Ausführung validiert werden.
5. Unnötige Render Passes SOLLEN entfernt werden.
6. Unabhängige Render Passes DÜRFEN parallel ausgeführt werden.
7. Temporäre Ressourcen MÜSSEN hinsichtlich ihrer Lebensdauer verwaltet werden.
8. Resource Aliasing DARF nur bei nachweislich sicherer Speichernutzung erfolgen.
9. Erforderliche Synchronisationsoperationen MÜSSEN geplant werden.
10. Der Render Graph MUSS inkrementelle Aktualisierungen unterstützen können.
11. Gültige Rendering-Ergebnisse SOLLEN wiederverwendet werden.
12. Damage Regions MÜSSEN bei der Optimierung berücksichtigt werden können.
13. Material- und Backdrop-Abhängigkeiten MÜSSEN korrekt verarbeitet werden.
14. Die Graphstruktur MUSS unabhängig vom konkreten GPU-Backend bleiben.
15. Ein deterministischer Ausführungsplan MUSS bei definierten Eingaben erzeugbar sein.
16. Bei ungültigen Optimierungen MUSS ein konservativer Fallback verfügbar sein.

## 17. Ergebnis

Die Nova Render Graph Architecture bildet die zentrale Planungs- und Optimierungsebene der Nova Rendering Pipeline.

Durch deklarierte Render Passes, automatische Abhängigkeitsanalyse, Ressourcenplanung und inkrementelle Graphverarbeitung werden ausschließlich erforderliche Rendering-Operationen vorbereitet.

Die Trennung zwischen logischer Graphstruktur und tatsächlicher GPU-Ausführung ermöglicht eine skalierbare, hardwareunabhängige und ressourcenschonende Grafikarchitektur.
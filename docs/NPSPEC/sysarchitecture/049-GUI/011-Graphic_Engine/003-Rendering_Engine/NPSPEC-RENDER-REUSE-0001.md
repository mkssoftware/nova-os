# NPSPEC-RENDER-REUSE-0001 – Nova Render Command Reuse

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Render Engine

## 1. Zweck

Nova Render Command Reuse definiert die Wiederverwendung bereits erzeugter Rendering-Befehle und Befehlsfolgen.

Ziel ist die Reduzierung redundanter CPU-Arbeit bei der Vorbereitung grafischer Operationen.

Die Wiederverwendung von Rendering-Befehlen ist unabhängig vom Render Result Caching und bedeutet nicht zwangsläufig, dass eine erneute GPU-Ausführung entfällt.

## 2. Architekturprinzipien

- **Command-Oriented:** Wiederverwendet werden ausführbare Rendering-Befehle.
- **State-Aware:** Die Gültigkeit hängt von den verwendeten Rendering-Zuständen ab.
- **Dependency-Aware:** Ressourcen- und Zustandsabhängigkeiten bestimmen die Wiederverwendbarkeit.
- **Incremental:** Nur veränderte Befehlsfolgen werden neu erzeugt.
- **Backend-Aware:** Hardwareabhängige Einschränkungen werden berücksichtigt.
- **Cache-Aware:** Wiederverwendbare Befehlsfolgen können zwischengespeichert werden.
- **Asynchronous:** Befehlsvorbereitung und GPU-Ausführung können entkoppelt erfolgen.
- **Fallback-Safe:** Ungültige Befehle werden regulär neu erzeugt.

## 3. Command Reuse Model

Eine wiederverwendbare Befehlsfolge besitzt mindestens:

- **Command ID:** Eindeutige Identifikation.
- **Command Type:** Art der Rendering-Operationen.
- **Pipeline State:** Verwendete Rendering-Konfiguration.
- **Resource Bindings:** Referenzierte Grafikressourcen.
- **Dependency Signature:** Relevante Zustands- und Ressourcenabhängigkeiten.
- **Target Compatibility:** Anforderungen an das Rendering-Ziel.
- **Validity State:** Aktuelle Wiederverwendbarkeit.
- **Backend Context:** Zugehöriges Rendering-Backend.

Die konkrete Speicherung DARF backendabhängig erfolgen.

## 4. Wiederverwendbare Befehle

NovaOS SOLL insbesondere folgende Befehlsfolgen wiederverwenden können:

- Draw Commands
- Compute Dispatches
- Geometriebezogene Rendering-Befehle
- Material- und Shader-Bindings
- Statische Render-Pass-Befehle
- Wiederkehrende UI-Rendering-Operationen

Synchronisationsbefehle, Ressourcenübergänge und dynamische Zustände DÜRFEN nur wiederverwendet werden, wenn ihre aktuellen Ausführungsbedingungen weiterhin erfüllt sind.

## 5. Gültigkeitsprüfung

Eine Befehlsfolge DARF nur wiederverwendet werden, wenn ihre relevanten Voraussetzungen gültig sind.

Dazu gehören:

- Pipeline-Kompatibilität
- Shader-Versionen
- Ressourcenidentitäten und -generationen
- Buffer-Offsets und Bindings
- Render-Target-Formate
- Dynamische Rendering-Parameter
- Backend- und Gerätekompatibilität

Eine unveränderte Scene-Node-ID allein reicht nicht zur Validierung aus.

Änderungen, die ausschließlich dynamische Parameter betreffen, DÜRFEN ohne vollständige Neuerzeugung der Befehlsfolge verarbeitet werden, sofern das Backend dies unterstützt.

## 6. Inkrementelle Befehlserzeugung

Nova Incremental Rendering bestimmt, welche Rendering-Befehle aktualisiert werden müssen.

Dabei SOLL NovaOS:

1. Relevante Zustandsänderungen ermitteln.
2. Betroffene Befehlsfolgen identifizieren.
3. Vorhandene Befehle validieren.
4. Gültige Befehle übernehmen.
5. Ungültige Befehle neu erzeugen.
6. Erforderliche Synchronisation ergänzen.
7. Die Ausführung vorbereiten.

Unveränderte Befehlsfolgen SOLLEN nicht unnötig neu aufgezeichnet werden.

## 7. Command Buffer Reuse

NovaOS SOLL die Wiederverwendung geeigneter Command Buffers unterstützen.

Dabei MUSS zwischen folgenden Zuständen unterschieden werden:

- **Recording:** Befehle werden aufgezeichnet.
- **Executable:** Befehle sind zur Ausführung vorbereitet.
- **Pending:** Befehle befinden sich in laufender Ausführung.
- **Invalid:** Befehle müssen neu erzeugt werden.

Eine erneute Übermittlung MUSS die jeweiligen Backend-Regeln für Command-Buffer-Lebensdauer, gleichzeitige Nutzung und erneute Ausführung einhalten.

Nicht jedes Rendering-Backend MUSS dieselben Wiederverwendungsmechanismen bereitstellen.

## 8. Dynamische Parameter

NovaOS SOLL statische Befehlsstrukturen von veränderlichen Rendering-Parametern trennen.

Dynamische Werte können beispielsweise über folgende Mechanismen bereitgestellt werden:

- Uniform Buffers
- Push Constants
- Dynamische Buffer-Offsets
- Descriptor Bindings
- Dynamische Pipeline States

Die Aktualisierung eines Parameters SOLL nicht automatisch die vollständige Neuerzeugung einer Befehlsfolge erfordern.

Die Gültigkeit der Parameterdaten MUSS bis zum Abschluss ihrer GPU-Verwendung gewährleistet sein.

## 9. Render Graph Integration

Der Nova Render Graph berücksichtigt wiederverwendbare Befehlsfolgen bei der Vorbereitung von Render Passes.

Dabei gilt:

- Der Render Graph bestimmt erforderliche Operationen und Abhängigkeiten.
- Render Command Reuse reduziert deren Vorbereitungskosten.
- GPU Synchronization gewährleistet die korrekte Ausführung.
- Render Result Caching kann die Ausführung vollständig vermeiden.

Ein wiederverwendbarer Command Buffer DARF erforderliche Ressourcenbarrieren oder Synchronisationsoperationen nicht umgehen.

## 10. Cache Management

Wiederverwendbare Befehlsfolgen DÜRFEN in einem Command Cache gespeichert werden.

NovaOS MUSS:

- Cache-Einträge validieren.
- Ungültige Befehlsfolgen erkennen.
- Ressourcenlebenszeiten berücksichtigen.
- Nicht mehr benötigte Einträge freigeben können.
- Speicherbudgets berücksichtigen.

Bei Speicherknappheit DÜRFEN gespeicherte Befehlsfolgen verworfen werden.

Die Verdrängung DARF laufende GPU-Aufträge nicht beeinträchtigen.

## 11. Parallel Command Recording

Unabhängige Befehlsfolgen DÜRFEN parallel vorbereitet werden.

NovaOS SOLL:

- Unabhängige Rendering-Aufträge erkennen.
- Parallele Befehlsaufzeichnung ermöglichen.
- Gemeinsame Ressourcen kontrolliert verwenden.
- Synchronisationsaufwand minimieren.

Die parallele Vorbereitung MUSS die Thread-Sicherheitsregeln des jeweiligen Rendering-Backends einhalten.

## 12. Adaptive Wiederverwendung

Die Wiederverwendung von Rendering-Befehlen ist nicht in jedem Fall effizienter als ihre Neuerzeugung.

NovaOS DARF die Strategie anhand folgender Kriterien auswählen:

- Kosten der Befehlsaufzeichnung
- Aufwand der Gültigkeitsprüfung
- Größe gespeicherter Command Buffers
- Wiederverwendungshäufigkeit
- Verfügbare CPU-Ressourcen
- Backend-Einschränkungen

Die Strategieauswahl DARF die Korrektheit der Rendering-Ausführung nicht beeinträchtigen.

## 13. Fehlerbehandlung

Ungültige oder inkompatible Befehlsfolgen MÜSSEN verworfen beziehungsweise neu erzeugt werden können.

NovaOS MUSS verhindern, dass:

- Freigegebene Ressourcen referenziert werden.
- Ungültige Pipeline States verwendet werden.
- Nicht abgeschlossene Command Buffers unzulässig verändert werden.
- Veraltete Befehle inkorrekte Darstellungsergebnisse erzeugen.

Bei fehlgeschlagener Wiederverwendung MUSS eine reguläre Befehlserzeugung möglich sein.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Render Command Reuse unterstützen.
2. Wiederverwendbare Befehlsfolgen MÜSSEN identifizierbar und validierbar sein.
3. Ressourcen- und Zustandsabhängigkeiten MÜSSEN berücksichtigt werden.
4. Unveränderte Befehlsfolgen SOLLEN nicht unnötig neu erzeugt werden.
5. Dynamische Parameter SOLLEN unabhängig von statischen Befehlsstrukturen aktualisierbar sein.
6. Command Buffer Reuse MUSS die Regeln des jeweiligen Backends einhalten.
7. Ressourcenlebenszeiten MÜSSEN gewährleistet bleiben.
8. Erforderliche Synchronisationsoperationen DÜRFEN nicht übersprungen werden.
9. Render Command Reuse MUSS mit dem Nova Render Graph integrierbar sein.
10. Unabhängige Befehlsfolgen DÜRFEN parallel vorbereitet werden.
11. Gespeicherte Befehlsfolgen MÜSSEN kontrolliert freigegeben werden können.
12. Eine reguläre Neuerzeugung MUSS jederzeit möglich sein.
13. Die Wiederverwendung DARF keine ungültigen Rendering-Ergebnisse erzeugen.
14. Die Architektur MUSS unterschiedliche Rendering-Backends unterstützen.

## 15. Ergebnis

Nova Render Command Reuse reduziert den CPU-Aufwand bei der Vorbereitung wiederkehrender Rendering-Operationen.

Durch die Validierung und Wiederverwendung vorhandener Befehlsfolgen vermeidet NovaOS unnötige Command-Buffer-Aufzeichnungen und redundante Rendering-Konfigurationen.

Während Render Result Caching bereits berechnete Grafikresultate wiederverwendet, optimiert Render Command Reuse die Vorbereitung weiterhin erforderlicher Rendering-Ausführungen.

Damit ergänzt das System Incremental Rendering und den Nova Render Graph um eine weitere ressourcenschonende Optimierungsebene.
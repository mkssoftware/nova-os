# NPSPEC-GPURESOURCE-RESIDENCY-0001 – Nova GPU Resource Residency

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Resource Residency definiert die Verwaltung der Verfügbarkeit von GPU-Ressourcen im für ihre Ausführung erforderlichen Speicher.

Ziel ist die effiziente Nutzung begrenzter GPU-Speicherkapazitäten, ohne ungültige Speicherzugriffe, unnötige Datentransfers oder übermäßige GPU-Blockierungen zu verursachen.

Die Architektur MUSS GPUs mit und ohne hardwareseitige Residency-Verwaltung unterstützen.

## 2. Architekturprinzipien

- **Demand-Driven:** Ressourcen werden entsprechend ihrem tatsächlichen Bedarf bereitgestellt.
- **Budget-Aware:** Residency-Entscheidungen berücksichtigen verfügbare Speicherbudgets.
- **Lifetime-Aware:** Aktive Ressourcen bleiben während erforderlicher Zugriffe verfügbar.
- **Dependency-Aware:** Ressourcenabhängigkeiten bestimmen Bereitstellung und Freigabe.
- **Predictive:** Absehbar benötigte Ressourcen können vorbereitet werden.
- **Eviction-Aware:** Entbehrliche Ressourcen können kontrolliert verdrängt werden.
- **Asynchronous:** Speichertransfers erfolgen möglichst ohne CPU-Blockierung.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Architekturen.

## 3. Residency Model

NovaOS unterscheidet folgende logische Residency-Zustände:

| Zustand | Beschreibung |
|---|---|
| Unallocated | Keine Speicherzuweisung vorhanden |
| Resident | Ressource ist für die vorgesehene GPU-Verwendung verfügbar |
| Non-Resident | Ressource ist nicht unmittelbar verfügbar |
| Pending Residency | Bereitstellung wurde angefordert |
| Pending Eviction | Verdrängung wurde angefordert |
| Invalid | Ressource ist nicht mehr verwendbar |

Residency beschreibt die tatsächliche Verfügbarkeit einer Ressource und ist vom logischen Ressourcenlebenszyklus zu unterscheiden.

## 4. Residency Management

Der Residency Manager MUSS mindestens folgende Aufgaben übernehmen:

1. Ermittlung benötigter GPU-Ressourcen.
2. Prüfung ihrer aktuellen Residency.
3. Planung erforderlicher Bereitstellungen.
4. Koordination notwendiger Speichertransfers.
5. Überwachung aktiver GPU-Zugriffe.
6. Auswahl geeigneter Eviction-Kandidaten.
7. Aktualisierung der Residency-Zustände.

Residency-Änderungen MÜSSEN mit GPU Memory Management und GPU Synchronization abgestimmt werden.

## 5. Resource Activation

Vor der GPU-Verwendung MUSS sichergestellt sein, dass alle erforderlichen Ressourcen verfügbar sind.

Dabei gilt:

- Fehlende Ressourcen werden rechtzeitig bereitgestellt.
- Erforderliche Transfers MÜSSEN abgeschlossen oder korrekt synchronisiert sein.
- Abhängige GPU-Aufträge dürfen nicht vorzeitig auf Ressourcen zugreifen.
- Ressourcen mit unbekanntem Zustand MÜSSEN konservativ behandelt werden.

Eine angeforderte Residency bedeutet noch keine abgeschlossene Bereitstellung.

## 6. Residency Priorities

NovaOS SOLL Ressourcen anhand ihrer Bedeutung priorisieren.

| Priorität | Verwendung |
|---|---|
| Critical | Systemkritische Grafikressourcen |
| Interactive | Ressourcen für direkte Benutzerinteraktionen |
| Visible | Aktuell sichtbare Grafikressourcen |
| Cached | Wiederverwendbare, derzeit nicht benötigte Ressourcen |
| Background | Ressourcen für Hintergrundoperationen |

Prioritäten DÜRFEN die erforderliche Sicherheit aktiver GPU-Zugriffe nicht beeinträchtigen.

## 7. Resource Eviction

NovaOS MUSS kontrollierte Resource Eviction unterstützen, soweit die Speicherarchitektur dies ermöglicht.

Dabei gilt:

- Ressourcen mit aktiven GPU-Zugriffen DÜRFEN nicht unsicher verdrängt werden.
- Wiederherstellbare Ressourcen SOLLEN bevorzugt verdrängt werden.
- Nicht wiederherstellbare Ressourcen MÜSSEN geschützt bleiben.
- Erforderliche Datenübertragungen werden synchronisiert.
- Residency-Zustände werden erst nach bestätigtem Übergang aktualisiert.

Die detaillierte Verdrängungsstrategie wird durch Nova GPU Resource Eviction definiert.

## 8. Predictive Residency

NovaOS DARF Ressourcen anhand absehbarer Verwendung vorbereiten.

Mögliche Informationsquellen:

- Render Graph
- Scene Graph
- Frame Scheduler
- Sichtbarkeitsinformationen
- Bekannte Ressourcenabhängigkeiten

Dabei gilt:

- Vorbereitende Residency-Operationen DÜRFEN notwendige Ressourcen nicht verdrängen.
- Speicherbudgets MÜSSEN eingehalten werden.
- Unnötige Transfers SOLLEN vermieden werden.
- Vorhersagen DÜRFEN keine Voraussetzung für die korrekte Ausführung sein.

## 9. Synchronization Integration

Nova GPU Resource Residency MUSS mit Nova GPU Synchronization zusammenarbeiten.

Dabei gilt:

- GPU Fences und Timeline-Werte schützen aktive Ressourcen.
- Queueübergreifende Zugriffe werden berücksichtigt.
- Speichertransfers besitzen definierte Abschlussbedingungen.
- Ressourcen dürfen erst nach erfüllten Residency-Abhängigkeiten verwendet werden.
- Fehlgeschlagene Transfers DÜRFEN keine erfolgreiche Residency signalisieren.

## 10. Hardware Fallback

Nicht jede GPU unterstützt dynamische Residency oder das Verdrängen einzelner Speicherzuweisungen.

NovaOS MUSS deshalb einen konservativen Fallback bereitstellen.

Dabei gilt:

- Ressourcen können während ihrer gesamten Allokationsdauer resident bleiben.
- Speicherbudgets begrenzen neue Allokationen.
- Entbehrliche Ressourcen können vollständig freigegeben und später neu erstellt werden.
- Nicht unterstützte Residency-Operationen werden nicht simuliert, wenn dadurch die Hardwaresemantik verletzt würde.

## 11. Performance und Memory Pressure

NovaOS SOLL Residency-Entscheidungen anhand folgender Faktoren optimieren:

- Verfügbarer GPU-Speicher
- Ressourcenverwendung
- Wiederherstellungskosten
- Transferbandbreite
- GPU-Auslastung
- Frame-Deadlines
- Speicherbelastung

Häufige Wechsel zwischen Resident und Non-Resident SOLLEN vermieden werden.

Bei Memory Pressure MUSS die Bereitstellung erforderlicher Ressourcen Vorrang vor spekulativer Zwischenspeicherung erhalten.

## 12. Sicherheit und Fehlerbehandlung

Residency-Operationen MÜSSEN capability-basiert autorisiert werden.

NovaOS MUSS insbesondere folgende Fehler behandeln:

- Fehlgeschlagene Residency-Anforderungen.
- Überschrittene Speicherbudgets.
- Fehlgeschlagene Speichertransfers.
- Ungültige Ressourcenreferenzen.
- GPU-Timeouts.
- Geräteverlust.

Fehlerhafte Ressourcen DÜRFEN nicht als resident und verwendbar gemeldet werden.

Bei Geräteverlust MÜSSEN betroffene Residency-Zustände invalidiert und gegebenenfalls kontrolliert wiederhergestellt werden.

## 13. Verbindliche Anforderungen

1. NovaOS MUSS GPU Resource Residency verwalten können.
2. Residency und Ressourcenlebenszyklus MÜSSEN getrennt betrachtet werden.
3. Benötigte Ressourcen MÜSSEN vor ihrer GPU-Verwendung verfügbar sein.
4. Residency-Übergänge MÜSSEN synchronisiert werden.
5. Aktive GPU-Ressourcen DÜRFEN nicht unsicher verdrängt werden.
6. GPU Memory Budgeting MUSS integriert sein.
7. Resource Eviction MUSS berücksichtigt werden.
8. Residency-Prioritäten SOLLEN unterstützt werden.
9. Predictive Residency DARF zur Optimierung verwendet werden.
10. Unnötige Speichertransfers SOLLEN vermieden werden.
11. Ein Fallback ohne native Residency-Unterstützung MUSS verfügbar sein.
12. Residency-Operationen MÜSSEN capability-basiert geschützt werden.
13. Fehlerhafte Residency-Zustände MÜSSEN kontrolliert behandelt werden.
14. Im deterministischen Modus MÜSSEN logische Residency-Entscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 14. Ergebnis

Nova GPU Resource Residency gewährleistet die kontrollierte Verfügbarkeit benötigter GPU-Ressourcen.

Durch bedarfsgerechte Bereitstellung, priorisierte Speicherverwendung, sichere Eviction und die Integration mit GPU Synchronization werden begrenzte GPU-Speicherkapazitäten effizient genutzt.

Damit erhält NovaOS eine skalierbare Residency-Verwaltung für dedizierte und integrierte GPUs, ohne auf spezielle Hardwarefunktionen angewiesen zu sein.
# NPSPEC-GPURESOURCE-ALLOC-0001 – Nova GPU Memory Allocation

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Memory Allocation definiert die Zuweisung, Verwaltung und Freigabe von GPU-Speicher innerhalb von NovaOS.

Ziel ist eine schnelle, speichereffiziente und fragmentierungsarme Allokation für Rendering, Compute und Display-Verarbeitung.

Die Speicherverwaltung MUSS unterschiedliche GPU-Speicherarchitekturen unterstützen und unabhängig von einer konkreten Grafik-API funktionieren.

## 2. Architekturprinzipien

- **Suballocation:** Kleine Ressourcen teilen sich größere Speicherblöcke.
- **Pooling:** Häufig benötigte Speicherbereiche werden wiederverwendet.
- **Alignment-Aware:** Hardwareabhängige Ausrichtungsanforderungen werden eingehalten.
- **Lifetime-Aware:** Speicher wird erst nach Abschluss aller relevanten GPU-Zugriffe freigegeben.
- **Budget-Aware:** Allokationen berücksichtigen verfügbare Speicherbudgets.
- **Fragmentation-Aware:** Speicherfragmentierung wird möglichst gering gehalten.
- **Adaptive:** Die Allokationsstrategie richtet sich nach Ressourcenart und Nutzung.
- **Backend-Independent:** Einheitliches Modell für unterschiedliche GPU-Treiber.

## 3. Allocation Model

Jede Speicherzuweisung besitzt mindestens folgende Eigenschaften:

| Eigenschaft | Beschreibung |
|---|---|
| Allocation ID | Eindeutige Identifikation |
| Memory Block | Zugehöriger Speicherblock |
| Offset | Position innerhalb des Blocks |
| Size | Zugewiesene Speichergröße |
| Alignment | Erforderliche Speicherausrichtung |
| Memory Class | Verwendete Speicherklasse |
| Owner | Zugehöriger Prozess oder Systemdienst |
| Resource Reference | Zugeordnete GPU-Ressource |
| Allocation State | Aktueller Verwaltungszustand |

Logische Ressourcen und physische Speicherzuweisungen MÜSSEN getrennt verwaltet werden.

## 4. Allocation Strategies

NovaOS SOLL mehrere Allokationsstrategien unterstützen.

| Strategie | Verwendung |
|---|---|
| Dedicated Allocation | Große oder spezialisierte Ressourcen |
| Suballocation | Kleine und mittlere Ressourcen |
| Pool Allocation | Häufig wiederverwendete Speicherbereiche |
| Linear Allocation | Kurzlebige, sequenziell verwendete Ressourcen |
| Ring Allocation | Fortlaufende temporäre Datenübertragungen |
| Buddy Allocation | Variable Speichergrößen mit kontrollierter Fragmentierung |

Die Auswahl MUSS anhand von Speicherklasse, Größe, Lebensdauer und Hardwareanforderungen erfolgen.

Eine bestimmte Strategie DARF nicht für sämtliche GPU-Ressourcen erzwungen werden.

## 5. Suballocation

NovaOS SOLL kleinere GPU-Ressourcen innerhalb größerer Speicherblöcke verwalten.

Dabei gilt:

1. Speicherblöcke werden beim GPU-Backend angefordert.
2. Einzelne Ressourcen erhalten geeignete Teilbereiche.
3. Alignment und Ressourcenanforderungen werden berücksichtigt.
4. Freigegebene Teilbereiche werden wiederverwendet.
5. Benachbarte freie Bereiche SOLLEN zusammengeführt werden.

Suballocation DARF keine unzulässigen Speicherüberlappungen erzeugen.

## 6. Dedicated Allocation

Eigene Speicherzuweisungen SOLLEN verwendet werden, wenn dies technisch erforderlich oder wirtschaftlicher ist.

Typische Anwendungsfälle:

- Sehr große Texturen.
- Große Render Targets.
- Spezielle Hardware-Ressourcen.
- Extern freigegebene GPU-Buffer.
- Ressourcen mit besonderen Speicheranforderungen.

Die Entscheidung MUSS die tatsächlichen Backend-Anforderungen berücksichtigen.

## 7. Alignment und Memory Requirements

Jede Allokation MUSS die Hardwareanforderungen erfüllen.

Dazu gehören:

- Speichergröße.
- Alignment.
- Kompatible Speicherklassen.
- Speichergranularität.
- Ressourcenbindung.
- Externe Speicheranforderungen.

Speicherbereiche DÜRFEN nur dann gemeinsam verwendet werden, wenn ihre Anforderungen kompatibel sind.

## 8. Allocation Lifecycle

NovaOS definiert folgende Zustände:

`Requested → Allocated → Bound → Active → Retired → Released`

Dabei gilt:

- Allokationen werden vor ihrer Verwendung validiert.
- Ressourcenbindungen MÜSSEN gültig sein.
- Aktive GPU-Zugriffe verhindern eine vorzeitige Freigabe.
- Retired Allocations können bis zum GPU-Abschluss zurückgehalten werden.
- Freigegebene Speicherbereiche können erneut zugewiesen werden.

Eine logische Ressourcenfreigabe bedeutet nicht automatisch eine sofortige physische Speicherfreigabe.

## 9. Deferred Deallocation

NovaOS MUSS verzögerte Speicherfreigaben unterstützen.

Dabei gilt:

- GPU Fences oder Timeline-Werte bestimmen den sicheren Freigabezeitpunkt.
- Noch verwendete Speicherbereiche bleiben reserviert.
- Abgeschlossene Allokationen können automatisch freigegeben werden.
- Mehrere relevante GPU-Queues MÜSSEN berücksichtigt werden.

Eine Allokation DARF nicht wiederverwendet werden, solange noch unvollständige GPU-Zugriffe darauf bestehen.

## 10. Fragmentation Management

NovaOS SOLL interne und externe Speicherfragmentierung minimieren.

Mögliche Maßnahmen:

- Größenklassen.
- Speicherpools.
- Zusammenführung freier Bereiche.
- Geeignete Blockgrößen.
- Kontrollierte Defragmentierung.

Eine Defragmentierung DARF aktive Ressourcen nur verschieben, wenn deren Bindungen, Synchronisation und Hardwarebeschränkungen dies zulassen.

## 11. Memory Budget Integration

Nova GPU Memory Allocation MUSS mit Nova GPU Memory Budgeting zusammenarbeiten.

Dabei gilt:

- Speicheranforderungen werden vor der Allokation geprüft.
- Prozessbezogene Budgets werden berücksichtigt.
- Systemkritische Reserven SOLLEN geschützt werden.
- Speicheranforderungen können bei unzureichendem Budget abgelehnt werden.
- Unnötige Speicherreservierungen SOLLEN vermieden werden.

## 12. Resource Pool Integration

NovaOS SOLL geeignete Speicherbereiche wiederverwenden.

Dabei gilt:

- Pool-Ressourcen müssen kompatible Eigenschaften besitzen.
- Noch aktive GPU-Zugriffe MÜSSEN abgeschlossen sein.
- Pool-Größen unterliegen Speicherbudgets.
- Ungenutzte Speicherblöcke DÜRFEN bei Memory Pressure freigegeben werden.

Pooling DARF keine unkontrollierte Speicherbelegung verursachen.

## 13. Resource Aliasing

Nova GPU Memory Allocation MUSS Resource Aliasing unterstützen können.

Dabei gilt:

- Unterschiedliche Ressourcen können denselben Speicherbereich verwenden.
- Ihre aktiven Verwendungsphasen dürfen sich nicht unzulässig überschneiden.
- Erforderliche Aliasing Barriers MÜSSEN berücksichtigt werden.
- Ressourcenbindungen und Speicherlebensdauern bleiben nachvollziehbar.

Aliasing wird insbesondere für temporäre Render-Graph-Ressourcen verwendet.

## 14. Performance und Optimierung

NovaOS SOLL Allokationsoperationen mit möglichst geringem Verwaltungsaufwand durchführen.

Dazu gehören:

- Reduzierung häufiger Backend-Allokationen.
- Wiederverwendung vorhandener Speicherblöcke.
- Effiziente Freispeicherverwaltung.
- Begrenzung unnötiger Speicherreservierungen.
- Nutzung geeigneter Allokationsstrategien.

Die Allokationsverwaltung DARF keine unnötige globale Serialisierung unabhängiger GPU-Arbeit verursachen.

## 15. Sicherheit und Isolation

GPU-Speicherzuweisungen MÜSSEN capability-basiert geschützt werden.

Dabei gilt:

- Jede Allokation besitzt einen definierten Eigentümer.
- Ressourcenbindungen MÜSSEN autorisiert sein.
- Prozessübergreifende Speicherfreigaben erfordern entsprechende Berechtigungen.
- Freigegebene Speicherbereiche DÜRFEN keine fremden Daten offenlegen.
- Ungültige Allocation Handles MÜSSEN zurückgewiesen werden.

## 16. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Fehlgeschlagene Speicherzuweisungen.
- Ungültige Alignment-Anforderungen.
- Überschrittene Speicherbudgets.
- Speicherfragmentierung.
- Ungültige Ressourcenbindungen.
- Synchronisationsfehler.
- Geräteverlust.

Dabei gilt:

- Fehlgeschlagene Allokationen DÜRFEN keine teilweise gültigen Ressourcen hinterlassen.
- Bereits reservierte Speicherbereiche MÜSSEN kontrolliert zurückgegeben werden.
- Aktive GPU-Zugriffe MÜSSEN bei der Fehlerbehandlung berücksichtigt werden.
- Ein sicherer Fallback auf alternative Allokationsstrategien SOLL verfügbar sein.

## 17. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale GPU-Speicherallokation bereitstellen.
2. Dedicated Allocation und Suballocation MÜSSEN unterstützt werden.
3. Memory Pooling SOLL verfügbar sein.
4. Hardwareabhängige Alignment-Anforderungen MÜSSEN eingehalten werden.
5. Ressourcenbindungen MÜSSEN validiert werden.
6. Speicherlebenszyklen MÜSSEN kontrolliert verwaltet werden.
7. Deferred Deallocation MUSS unterstützt werden.
8. Speicher DÜRFEN nicht während aktiver GPU-Zugriffe wiederverwendet werden.
9. Speicherfragmentierung SOLL minimiert werden.
10. Memory Budgeting MUSS integriert sein.
11. Resource Aliasing SOLL unterstützt werden.
12. Unnötige Backend-Allokationen SOLLEN vermieden werden.
13. Unterschiedliche GPU-Speicherklassen MÜSSEN berücksichtigt werden.
14. Speicherzuweisungen MÜSSEN capability-basiert geschützt werden.
15. Fehlerhafte Allokationen MÜSSEN kontrolliert behandelt werden.
16. Im deterministischen Modus MÜSSEN logische Allokationsentscheidungen bei identischen Eingaben und Speicherbedingungen reproduzierbar sein.

## 18. Ergebnis

Nova GPU Memory Allocation ermöglicht die effiziente Zuweisung und Wiederverwendung von GPU-Speicher.

Durch Suballocation, Pooling, kontrollierte Speicherfreigaben und fragmentierungsarme Allokationsstrategien werden Speicherverbrauch und Verwaltungsaufwand reduziert.

Damit erhält NovaOS eine leistungsfähige und hardwareunabhängige Speicherallokation für Rendering, Compute und Display-Verarbeitung.
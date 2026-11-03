# NPSPEC-GPURESOURCE-BUDGET-0001 – Nova GPU Memory Budgeting

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Resource Management

## 1. Zweck

Nova GPU Memory Budgeting definiert die Überwachung, Zuteilung und Begrenzung des GPU-Speicherverbrauchs innerhalb von NovaOS.

Ziel ist eine faire, effiziente und stabile Speichernutzung zwischen Systemdiensten, Anwendungen und GPU-Operationen.

Speicherbudgets MÜSSEN sowohl dedizierten Videospeicher als auch gemeinsam genutzten Systemspeicher berücksichtigen.

## 2. Architekturprinzipien

- **Budget-Aware:** Speicheranforderungen berücksichtigen verfügbare Kapazitäten.
- **Dynamic:** Budgets passen sich veränderten Speicherbedingungen an.
- **Priority-Based:** Systemkritische Ressourcen erhalten bevorzugte Behandlung.
- **Process-Aware:** Speicherverbrauch wird Prozessen und Diensten zugeordnet.
- **Pressure-Aware:** Speicherknappheit wird frühzeitig erkannt.
- **Fairness:** Einzelne Anwendungen dürfen andere nicht unkontrolliert verdrängen.
- **Adaptive:** Speicherstrategien reagieren auf tatsächliche Nutzung.
- **Backend-Independent:** Einheitliches Budgetmodell für unterschiedliche GPU-Architekturen.

## 3. Budget Model

NovaOS verwaltet GPU-Speicherbudgets anhand folgender Werte:

| Eigenschaft | Beschreibung |
|---|---|
| Total Capacity | Gesamte adressierbare Speicherkapazität |
| Available Budget | Aktuell nutzbares Speicherbudget |
| Committed Memory | Bereits zugesagter Speicher |
| Resident Memory | Aktuell residenter Speicher |
| Reserved Memory | Reservierte Speicherbereiche |
| Cached Memory | Für Wiederverwendung vorgehaltener Speicher |
| Process Usage | Zugeordneter Speicherverbrauch eines Prozesses |
| System Reserve | Geschützter Speicher für Systemfunktionen |

Die Werte MÜSSEN nach Speicherklasse beziehungsweise Hardware-Speichersegment unterscheidbar sein.

Physische Speicherkapazität und tatsächlich verfügbares Budget DÜRFEN nicht gleichgesetzt werden.

## 4. Budget Hierarchy

NovaOS SOLL GPU-Speicherbudgets hierarchisch verwalten.

Die Hierarchie umfasst:

1. Globales GPU-Speicherbudget.
2. Speicherbudget je GPU und Speicherklasse.
3. Reserven für systemkritische Komponenten.
4. Budgets für Prozesse und Systemdienste.
5. Optionale Budgets für einzelne Ressourcenklassen.

Dabei gilt:

- Untergeordnete Budgets MÜSSEN übergeordneten Grenzen entsprechen.
- Gemeinsam genutzter Speicher MUSS korrekt zugeordnet werden.
- Mehrfachzählungen derselben physischen Speicherbelegung MÜSSEN vermieden werden.
- Systemreserven DÜRFEN nicht durch gewöhnliche Anwendungen aufgebraucht werden.

## 5. Dynamic Budget Management

NovaOS MUSS verfügbare Speicherbudgets dynamisch aktualisieren können.

Berücksichtigt werden:

- Änderungen der GPU-Speicherbelegung.
- Speicheranforderungen anderer Prozesse.
- Gemeinsame Nutzung des Systemspeichers.
- Veränderungen der verfügbaren Hardware-Ressourcen.
- Freigabe und Verdrängung von GPU-Ressourcen.

Budgetänderungen DÜRFEN aktive GPU-Ressourcen nicht unmittelbar ungültig machen.

Bei sinkendem Budget MUSS eine kontrollierte Anpassung eingeleitet werden.

## 6. Memory Accounting

NovaOS MUSS den GPU-Speicherverbrauch nachvollziehbar erfassen.

Dabei gilt:

- Allokationen werden ihrem Eigentümer zugeordnet.
- Gemeinsam genutzte Ressourcen werden eindeutig erfasst.
- Physischer Speicherverbrauch und logische Ressourcennutzung bleiben unterscheidbar.
- Pool- und Cache-Speicher wird berücksichtigt.
- Verzögert freizugebende Allokationen bleiben bis zur tatsächlichen Freigabe erfasst.

Speicherverbrauch DARF nicht allein anhand der logischen Ressourcengröße berechnet werden, wenn die tatsächliche Speicherbelegung davon abweicht.

## 7. Allocation Budget Validation

Nova GPU Memory Allocation MUSS Speicherbudgets vor neuen Allokationen berücksichtigen.

Dabei gilt:

1. Die Speicheranforderung wird ermittelt.
2. Das verfügbare Budget wird geprüft.
3. Erforderliche Systemreserven werden berücksichtigt.
4. Die Allokation wird zugelassen, angepasst oder abgelehnt.
5. Die tatsächliche Speicherbelegung wird anschließend aktualisiert.

Budgetprüfungen und Speicherreservierungen MÜSSEN gegen konkurrierende Anforderungen abgesichert sein.

Eine erfolgreiche Budgetprüfung garantiert keine erfolgreiche Hardware-Allokation.

## 8. Budget Priorities

NovaOS SOLL Speicheranforderungen priorisieren.

| Priorität | Ressourcen |
|---|---|
| Critical | Display Server, notwendige Systemdarstellung |
| High | Aktive Benutzeroberfläche und interaktive Anwendungen |
| Normal | Gewöhnliche Anwendungsressourcen |
| Low | Hintergrundverarbeitung |
| Evictable | Entbehrliche Cache- und temporäre Ressourcen |

Prioritäten beeinflussen die Speicherzuteilung, dürfen jedoch keine unautorisierten Zugriffe ermöglichen.

Die Benutzeroberfläche SOLL auch bei hoher GPU-Speicherbelastung bedienbar bleiben.

## 9. Budget Enforcement

NovaOS MUSS definierte Speichergrenzen durchsetzen können.

Mögliche Maßnahmen:

- Begrenzung neuer Allokationen.
- Freigabe ungenutzter Speicherpools.
- Verdrängung wiederherstellbarer Ressourcen.
- Reduzierung temporärer Render-Ressourcen.
- Anpassung der Rendering-Qualität.
- Kontrollierte Ablehnung weiterer Anforderungen.

Bereits laufende GPU-Operationen MÜSSEN dabei korrekt abgeschlossen oder kontrolliert abgebrochen werden.

## 10. Memory Pressure Integration

Nova GPU Memory Budgeting MUSS mit Nova GPU Memory Pressure Management zusammenarbeiten.

Dabei gilt:

- Sinkende Speicherreserven werden frühzeitig erkannt.
- Pressure-Zustände werden an betroffene Komponenten gemeldet.
- Resource Cache und Resource Pooling können ihre Speicherbelegung reduzieren.
- Resource Eviction kann geeignete Ressourcen verdrängen.
- Systemkritische Ressourcen erhalten bevorzugten Schutz.

Budgeting definiert die verfügbaren Grenzen; Memory Pressure Management koordiniert die Reaktion auf Speicherknappheit.

## 11. Multi-GPU und Shared Memory

NovaOS MUSS unterschiedliche GPU-Speicherarchitekturen berücksichtigen.

Dabei gilt:

- Jede GPU besitzt eine eigene Budgetverwaltung.
- Unterschiedliche Speichersegmente werden getrennt erfasst.
- Gemeinsam genutzter Systemspeicher wird mit der allgemeinen Speicherverwaltung abgestimmt.
- Ressourcen auf mehreren GPUs werden entsprechend ihrer tatsächlichen Speicherbelegung berücksichtigt.
- Speicherbudgets DÜRFEN nicht pauschal zwischen unabhängigen GPUs addiert werden.

## 12. Performance und Optimierung

NovaOS SOLL Budgetentscheidungen mit geringem Verwaltungsaufwand durchführen.

Dazu gehören:

- Ereignisgesteuerte Aktualisierung.
- Zwischengespeicherte Budgetinformationen.
- Inkrementelle Speicherverbrauchserfassung.
- Vermeidung unnötiger Speicherreservierungen.
- Hysterese zur Vermeidung häufiger Budgetanpassungen.

Budgetoptimierungen DÜRFEN keine unkontrollierte Speicherüberbelegung verursachen.

## 13. Sicherheit und Isolation

GPU-Speicherbudgets MÜSSEN in die NovaOS-Sicherheitsarchitektur integriert werden.

Dabei gilt:

- Prozesse DÜRFEN ihre Speicherbudgets nicht eigenmächtig erhöhen.
- Budgetänderungen benötigen entsprechende Berechtigungen.
- Gemeinsam genutzte Ressourcen MÜSSEN korrekt abgerechnet werden.
- Fehlerhafte Anwendungen DÜRFEN keine unbegrenzten GPU-Speicherressourcen beanspruchen.
- Systemkritische Speicherreserven MÜSSEN geschützt bleiben.

## 14. Fehlerbehandlung

NovaOS MUSS folgende Situationen kontrolliert behandeln:

- Überschrittene Speicherbudgets.
- Fehlgeschlagene Speicherreservierungen.
- Unvollständige Hardware-Budgetinformationen.
- Unerwartete Änderungen der verfügbaren Kapazität.
- Inkonsistente Speicherverbrauchsdaten.
- Geräteverlust.

Dabei gilt:

- Unbekannte Budgets MÜSSEN konservativ behandelt werden.
- Fehlgeschlagene Allokationen DÜRFEN keine dauerhaft falschen Speicherbuchungen hinterlassen.
- Speicherknappheit DARF keine unkontrollierte Beschädigung bestehender Ressourcen verursachen.

## 15. Verbindliche Anforderungen

1. NovaOS MUSS eine zentrale GPU-Speicherbudgetverwaltung bereitstellen.
2. Dedizierter und gemeinsam genutzter GPU-Speicher MÜSSEN berücksichtigt werden.
3. Budgets MÜSSEN nach GPU und Speicherklasse unterscheidbar sein.
4. Speicherverbrauch MUSS nachvollziehbar erfasst werden.
5. Prozessbezogene Speicherbudgets MÜSSEN unterstützt werden.
6. Systemkritische Speicherreserven MÜSSEN geschützt werden.
7. Budgetänderungen MÜSSEN dynamisch verarbeitet werden können.
8. Neue Allokationen MÜSSEN verfügbare Budgets berücksichtigen.
9. Gemeinsam genutzte Ressourcen DÜRFEN nicht mehrfach als physischer Verbrauch gezählt werden.
10. Resource Residency MUSS integriert werden.
11. Memory Pressure Management MUSS integriert werden.
12. Resource Eviction SOLL zur Budgetentlastung verwendet werden.
13. Speicherprioritäten SOLLEN unterstützt werden.
14. Überschrittene Budgets MÜSSEN kontrolliert behandelt werden.
15. Budgetänderungen MÜSSEN autorisiert sein.
16. Im deterministischen Modus MÜSSEN Budgetentscheidungen bei identischen Eingaben und Ressourcenbedingungen reproduzierbar sein.

## 16. Ergebnis

Nova GPU Memory Budgeting gewährleistet eine kontrollierte und faire Verteilung verfügbarer GPU-Speicherressourcen.

Durch dynamische Budgets, präzise Speicherverbrauchserfassung, Priorisierung und die Integration mit Resource Residency und Memory Pressure Management verhindert NovaOS unkontrollierte Speicherbelegung.

Damit entsteht eine stabile Grundlage für ressourceneffizientes Rendering, Compute und die zuverlässige Darstellung der Benutzeroberfläche.
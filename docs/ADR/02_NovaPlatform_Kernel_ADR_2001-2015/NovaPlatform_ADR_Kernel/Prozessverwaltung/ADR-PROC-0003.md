# ADR-PROC-0003 – Scheduler Classes

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0003 |
| Titel | Scheduler Classes |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Threads / Scheduling |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002 |
| Nachfolgende Spezifikation | NPSPEC-SCHEDULER-0001 |

---

## 1. Kontext

NovaOS benötigt einen Scheduler, der sehr unterschiedliche Arbeitslasten gleichzeitig verarbeiten kann.

Dazu gehören unter anderem:

- interaktive Desktop-Anwendungen,
- Hintergrunddienste,
- zeitkritische Audio- und Videoverarbeitung,
- Treiber und Kernel-Worker,
- lokale KI- und Compute-Aufgaben,
- energieeffiziente Hintergrundprozesse,
- administrative Systemdienste,
- harte oder weiche Echtzeitanforderungen,
- virtuelle Maschinen und Kompatibilitätsumgebungen.

Ein einzelnes Scheduling-Verfahren kann diese Anforderungen nicht gleichermaßen erfüllen.

Ein Desktop-Thread benötigt beispielsweise eine geringe Eingabelatenz, während eine rechenintensive Hintergrundaufgabe möglichst viel ungenutzte CPU-Zeit verwenden soll, ohne die Benutzeroberfläche zu beeinträchtigen.

NovaOS führt deshalb mehrere Scheduler-Klassen ein. Jede Klasse beschreibt eine Scheduling-Politik für eine bestimmte Kategorie von Threads.

Die kleinste planbare Einheit bleibt der in ADR-PROC-0002 definierte Thread. Prozesse werden nicht direkt eingeplant, sondern stellen den Ressourcen-, Sicherheits- und Adressraumkontext ihrer Threads bereit.

---

## 2. Problemstellung

Der Scheduler muss folgende teilweise widersprüchliche Anforderungen erfüllen:

- geringe Latenz für interaktive Aufgaben,
- fairen CPU-Zugriff für normale Anwendungen,
- deterministisches Verhalten für Echtzeitaufgaben,
- garantierte Ausführung kritischer Kernel-Arbeiten,
- kontrollierte Nutzung durch Hintergrundaufgaben,
- Energieeffizienz auf mobilen Systemen,
- Skalierbarkeit auf Mehrkern- und NUMA-Systemen,
- Vermeidung von Starvation,
- Schutz vor Prioritätsmissbrauch,
- Unterstützung von Prioritätsvererbung,
- nachvollziehbare und diagnostizierbare Entscheidungen,
- dynamische Anpassung an Systemlast und Benutzeraktivität.

Eine globale numerische Priorität reicht hierfür nicht aus.

Eine Echtzeitpriorität, eine interaktive Priorität und eine Hintergrundpriorität besitzen unterschiedliche Bedeutungen. Würden alle Threads ausschließlich in einer gemeinsamen Prioritätsliste geführt, könnten nicht vertrauenswürdige Anwendungen durch falsch gewählte Prioritäten das gesamte System blockieren.

---

## 3. Entscheidungsziele

Die Scheduler-Architektur soll:

1. verschiedene Arbeitslasten explizit unterscheiden,
2. interaktive Reaktionsfähigkeit priorisieren,
3. Echtzeitanforderungen kontrolliert unterstützen,
4. Systemstillstände durch unbeschränkte Echtzeitthreads verhindern,
5. Hintergrundarbeit zuverlässig, aber unaufdringlich ausführen,
6. Klassenwechsel kontrollieren und autorisieren,
7. Mehrkernsysteme effizient auslasten,
8. Energie- und Temperaturgrenzen berücksichtigen,
9. Prioritätsinversion begrenzen,
10. Scheduling-Entscheidungen lokal diagnostizierbar machen.

---

## 4. Betrachtete Alternativen

### 4.1 Eine einzige globale Prioritätswarteschlange

Alle Threads werden anhand einer globalen numerischen Priorität eingeordnet.

#### Vorteile

- einfache Implementierung,
- geringe Anzahl interner Datenstrukturen,
- leicht verständliches Grundmodell.

#### Nachteile

- unterschiedliche Arbeitslasten werden nicht angemessen abgebildet,
- Echtzeit- und normale Prioritäten lassen sich nur schwer sicher kombinieren,
- erhöhtes Starvation-Risiko,
- schlechte Kontrolle über Hintergrundlast,
- geringe Erweiterbarkeit.

Diese Alternative wird verworfen.

---

### 4.2 Vollständig fairer Scheduler für alle Threads

Alle Threads erhalten langfristig einen proportional fairen Anteil der CPU-Zeit.

#### Vorteile

- gute Fairness,
- geeignet für normale Desktop- und Serverlasten,
- verhindert weitgehend dauerhafte Benachteiligung.

#### Nachteile

- keine ausreichenden Garantien für Echtzeitaufgaben,
- Kernel-Arbeiten können verzögert werden,
- interaktive Aufgaben konkurrieren direkt mit rechenintensiven Aufgaben,
- keine klare Trennung sicherheitskritischer Klassen.

Diese Alternative wird als Grundlage für normale Threads verwendet, aber nicht als alleinige Scheduler-Strategie übernommen.

---

### 4.3 Fest definierte Scheduler-Klassen

Threads werden einer Scheduler-Klasse zugeordnet. Jede Klasse besitzt eigene Regeln, Prioritäten, Zeitbudgets und Zulassungsbedingungen.

#### Vorteile

- klare Trennung verschiedener Arbeitslasten,
- kontrollierbare Echtzeitunterstützung,
- bessere Desktop-Reaktionsfähigkeit,
- gezielte Energieoptimierung,
- erweiterbares Architekturmodell,
- bessere Sicherheits- und Ressourcensteuerung.

#### Nachteile

- höhere Implementierungskomplexität,
- Klasseninteraktionen müssen eindeutig definiert werden,
- fehlerhafte Klassenzuordnung kann die Systemleistung beeinträchtigen,
- aufwendigere Diagnose und Tests.

Diese Alternative wird gewählt.

---

### 4.4 Vollständig lernender oder KI-gesteuerter Scheduler

Ein lernendes System entscheidet dynamisch über Prioritäten, CPU-Zuordnung und Zeitbudgets.

#### Vorteile

- potenziell gute Anpassung an wiederkehrende Arbeitslasten,
- Möglichkeit zur individuellen Optimierung,
- langfristige Effizienzsteigerung.

#### Nachteile

- Scheduling-Entscheidungen wären schwer reproduzierbar,
- keine belastbaren Echtzeitgarantien,
- schwer überprüfbare Fehlerfälle,
- erhöhtes Sicherheitsrisiko,
- ungeeignet als grundlegender Kernelmechanismus.

Diese Alternative wird für den Kern-Scheduler verworfen.

Lernende Komponenten dürfen später ausschließlich begrenzte Hinweise liefern, beispielsweise zur Energieoptimierung. Die endgültige Entscheidung bleibt bei deterministischen Kernelregeln.

---

## 5. Entscheidung

NovaOS verwendet ein hierarchisches, präemptives Scheduling-Modell mit mehreren fest definierten Scheduler-Klassen.

Die Scheduler-Klasse ist eine Eigenschaft eines Threads.

Folgende Klassen werden eingeführt:

| Klasse | Bezeichnung | Zweck |
|---|---|---|
| `NOVA_SCHED_CRITICAL` | Critical | zwingend erforderliche, kurze Kernel-Arbeiten |
| `NOVA_SCHED_REALTIME` | Realtime | kontrollierte Echtzeitaufgaben |
| `NOVA_SCHED_INTERACTIVE` | Interactive | Benutzeroberfläche und latenzkritische Interaktionen |
| `NOVA_SCHED_NORMAL` | Normal | allgemeine Anwendungen und Dienste |
| `NOVA_SCHED_BATCH` | Batch | rechenintensive, nicht interaktive Aufgaben |
| `NOVA_SCHED_BACKGROUND` | Background | niedrig priorisierte Hintergrundarbeit |
| `NOVA_SCHED_IDLE` | Idle | Arbeit ausschließlich bei ansonsten ungenutzter CPU |

Die Klassen bilden eine globale Rangfolge. Innerhalb jeder Klasse gelten eigene Scheduling-Regeln.

```text
CRITICAL
   ↓
REALTIME
   ↓
INTERACTIVE
   ↓
NORMAL
   ↓
BATCH
   ↓
BACKGROUND
   ↓
IDLE
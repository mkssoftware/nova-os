# ADR-PROC-0004 – Prioritäten

| Feld | Wert |
|---|---|
| Dokument-ID | ADR-PROC-0004 |
| Titel | Prioritäten |
| Status | Angenommen |
| Version | 1.0 |
| Datum | 2026-07-30 |
| Bereich | Kernel / Prozesse / Threads / Scheduling |
| Verantwortlich | NovaOS Architecture Team |
| Abhängigkeiten | ADR-PROC-0001, ADR-PROC-0002, ADR-PROC-0003 |
| Nachfolgende Spezifikation | NPSPEC-SCHEDULER-PRIORITY-0001 |

---

## 1. Kontext

NovaOS verwendet gemäß ADR-PROC-0003 mehrere Scheduler-Klassen für unterschiedliche Arten von Arbeitslasten.

Innerhalb dieser Scheduler-Klassen muss festgelegt werden, welcher Thread bevorzugt ausgeführt wird. Hierfür benötigt jeder Thread ein klar definiertes Prioritätsmodell.

Eine einzelne globale Prioritätszahl ist dafür nicht ausreichend, weil Prioritäten je nach Scheduler-Klasse unterschiedliche Bedeutungen besitzen.

Beispiele:

- Eine Echtzeitpriorität beschreibt zeitliche Dringlichkeit.
- Eine interaktive Priorität beschreibt die gewünschte Reaktionsgeschwindigkeit.
- Eine normale Priorität beeinflusst den fairen CPU-Anteil.
- Eine Hintergrundpriorität beeinflusst, wie stark eine Aufgabe gegenüber aktiver Arbeit zurückgestellt wird.
- Eine Critical-Priorität ist ausschließlich für kurze interne Kernel-Arbeiten bestimmt.

Zusätzlich muss zwischen der konfigurierten Priorität eines Threads und seiner aktuell wirksamen Priorität unterschieden werden.

Die wirksame Priorität kann sich zeitweise verändern durch:

- Prioritätsvererbung,
- Aging,
- Wake-up-Boni,
- Budgetüberschreitungen,
- Ressourcenbegrenzungen,
- thermische Drosselung,
- temporäre Kernel-Boosts,
- Deadline-Nähe.

Das Prioritätsmodell muss daher verständlich, sicher, deterministisch und diagnostizierbar sein.

---

## 2. Problemstellung

Das System benötigt ein Prioritätsmodell, das folgende Anforderungen erfüllt:

- klare Trennung zwischen Scheduler-Klasse und Priorität,
- vorhersehbare Reihenfolge konkurrierender Threads,
- Unterstützung interaktiver Anwendungen,
- Unterstützung kontrollierter Echtzeitaufgaben,
- Schutz vor Prioritätsinversion,
- Schutz vor Starvation,
- Verhinderung unberechtigter Hochstufungen,
- dynamische, aber begrenzte Prioritätsanpassungen,
- Vererbung sinnvoller Standardwerte,
- Kompatibilität mit Mehrkern- und NUMA-Systemen,
- Integration in das Capability- und Ressourcenmodell,
- stabile öffentliche ABI,
- vollständige lokale Diagnosefähigkeit.

Ohne verbindliche Regeln könnten unterschiedliche Kernelkomponenten Prioritäten unterschiedlich interpretieren.

Das könnte zu folgenden Problemen führen:

- nicht reproduzierbaren Scheduler-Entscheidungen,
- dauerhafter Benachteiligung einzelner Threads,
- blockierten interaktiven Aufgaben,
- missbräuchlicher Echtzeitnutzung,
- Prioritätsinversionen,
- widersprüchlichen APIs,
- schwer nachvollziehbaren Leistungseinbrüchen.

---

## 3. Entscheidungsziele

Das Prioritätsmodell soll:

1. Scheduler-Klasse und Priorität strikt voneinander trennen,
2. eine einheitliche Prioritätsrichtung verwenden,
3. Basispriorität und effektive Priorität unterscheiden,
4. temporäre Prioritätsänderungen sicher unterstützen,
5. unberechtigte Hochstufungen verhindern,
6. Prioritätsinversion begrenzen,
7. Starvation niedriger Prioritäten verhindern,
8. Prioritätsänderungen nachvollziehbar machen,
9. eine stabile und erweiterbare ABI ermöglichen,
10. auf Einzelkern-, Mehrkern-, NUMA- und heterogenen Systemen funktionieren.

---

## 4. Begriffe

### 4.1 Scheduler-Klasse

Die Scheduler-Klasse definiert die grundlegende Scheduling-Politik eines Threads.

Beispiele:

```text
CRITICAL
REALTIME
INTERACTIVE
NORMAL
BATCH
BACKGROUND
IDLE
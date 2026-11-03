# NPSPEC-GRAPHICS-TRANSACTION-0001 – Nova Graphics State Transactions

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics

## 1. Zweck

Nova Graphics State Transactions definiert die atomare und konsistente Verarbeitung zusammengehöriger grafischer Zustandsänderungen.

Änderungen werden zunächst gesammelt, validiert und anschließend gemeinsam übernommen.

NovaOS verhindert dadurch die Darstellung unvollständiger Zwischenzustände und reduziert unnötige Layout- und Rendering-Operationen.

## 2. Architekturprinzipien

- **Atomic:** Zusammengehörige Änderungen werden vollständig oder gar nicht übernommen.
- **Consistent:** Jede Transaktion erhält die Gültigkeit des grafischen Zustands.
- **Isolated:** Nicht abgeschlossene Änderungen bleiben für die Darstellung unsichtbar.
- **Ordered:** Transaktionen werden in definierter Reihenfolge verarbeitet.
- **Deterministic:** Identische Ausgangszustände und Transaktionsfolgen erzeugen identische logische Ergebnisse.
- **Incremental:** Nur tatsächlich veränderte Zustände werden weiterverarbeitet.
- **Batched:** Mehrere Änderungen können gemeinsam verarbeitet werden.

## 3. Transaktionsmodell

Eine Graphics State Transaction umfasst:

- Eindeutige Transaktionskennung
- Betroffenen grafischen Zustandsbereich
- Ausgangsversion des Zustands
- Gesammelte Zustandsänderungen
- Validierungsergebnis
- Transaktionsstatus

Unterstützte Transaktionszustände:

- **Open:** Änderungen werden gesammelt.
- **Validating:** Änderungen werden geprüft.
- **Committed:** Änderungen wurden erfolgreich übernommen.
- **Aborted:** Änderungen wurden verworfen.

Eine abgeschlossene Transaktion DARF nicht erneut verändert oder übernommen werden.

## 4. Transaktionsverarbeitung

Die Verarbeitung erfolgt grundsätzlich in folgender Reihenfolge:

1. Transaktion beginnen.
2. Zustandsänderungen sammeln.
3. Änderungen und Abhängigkeiten validieren.
4. Zustandskonflikte erkennen und behandeln.
5. Änderungen atomar übernehmen.
6. Neue Zustandsversion veröffentlichen.
7. Betroffene Abhängigkeiten und Damage Regions bestimmen.
8. Erforderliche Rendering-Aktualisierungen auslösen.

Die Veröffentlichung des neuen Zustands MUSS atomar erfolgen.

Rendering und Komposition DÜRFEN ausschließlich konsistente, veröffentlichte Zustandsversionen verwenden.

## 5. Zustandskonsistenz

Eine Transaktion MUSS alle betroffenen Zustandsinvarianten erhalten.

Dazu gehören insbesondere:

- Gültige Elementidentitäten
- Konsistente Eltern-Kind-Beziehungen
- Gültige Eigenschaftswerte
- Zulässige Transformationen
- Konsistente Ressourcenreferenzen
- Gültige Abhängigkeiten

Fehlgeschlagene Transaktionen MÜSSEN vollständig verworfen werden, ohne den veröffentlichten Zustand teilweise zu verändern.

## 6. Parallelität und Konflikte

Mehrere Transaktionen DÜRFEN gleichzeitig vorbereitet werden.

Die Übernahme MUSS eine eindeutige, serialisierbare Reihenfolge gewährleisten.

Konkurrierende Änderungen am selben Zustand MÜSSEN erkannt und nach definierten Konfliktregeln behandelt werden.

Transaktionen DÜRFEN bei Konflikten abgewiesen oder auf Grundlage einer aktuellen Zustandsversion erneut ausgeführt werden.

Eine automatische Zusammenführung ist nur zulässig, wenn die Konsistenz gewährleistet bleibt.

## 7. Änderungszusammenfassung

Innerhalb einer Transaktion DÜRFEN mehrfach geänderte Eigenschaften zusammengefasst werden.

Für die weitere Verarbeitung ist grundsätzlich die Differenz zwischen veröffentlichtem Ausgangszustand und endgültigem Zielzustand maßgeblich.

Zwischenänderungen ohne Auswirkung auf den endgültigen Zustand SOLLEN keine unnötigen Rendering-Operationen verursachen.

Abhängigkeiten, Ressourcenlebensdauer und notwendige Zustandsereignisse MÜSSEN dennoch korrekt berücksichtigt werden.

## 8. Integration in die Grafikarchitektur

Graphics State Transactions bilden die Grundlage konsistenter Zustandsübergänge zwischen:

- Nova Declarative Graphics Model
- Nova State-Driven Graphics
- Nova Scene Graph
- Nova UI Engine
- Nova Render Graph
- Nova Render Engine
- Nova Material Engine
- Nova Compositor

Der Scene Graph verwaltet die veröffentlichten Zustandsversionen.

Change Tracking und Dependency Tracking ermitteln die Auswirkungen einer übernommenen Transaktion.

Der Render Graph organisiert anschließend die erforderlichen Rendering-Operationen.

Die Transaktionsverarbeitung DARF nicht von der tatsächlichen GPU-Ausführung abhängig sein.

## 9. Fehlerbehandlung

Fehlerhafte Transaktionen MÜSSEN kontrolliert abgebrochen werden.

Mögliche Fehler umfassen:

- Ungültige Eigenschaften
- Nicht vorhandene Elemente
- Ungültige Abhängigkeiten
- Zustandskonflikte
- Fehlende Berechtigungen
- Ressourcenbeschränkungen

Ein Transaktionsfehler DARF den zuletzt gültigen veröffentlichten Zustand nicht beschädigen.

Fehler MÜSSEN über definierte Rückgabewerte oder Ereignisse nachvollziehbar sein.

## 10. Verbindliche Anforderungen

1. Zusammengehörige Zustandsänderungen MÜSSEN atomar übernehmbar sein.
2. Nicht abgeschlossene Änderungen DÜRFEN nicht als veröffentlichter Zustand sichtbar werden.
3. Jede erfolgreiche Transaktion MUSS eine eindeutig zuordenbare Zustandsversion besitzen.
4. Transaktionen MÜSSEN vor ihrer Übernahme validiert werden.
5. Konkurrierende Änderungen MÜSSEN konsistent verarbeitet werden.
6. Fehlgeschlagene Transaktionen DÜRFEN keine partiellen Zustandsänderungen hinterlassen.
7. Änderungen SOLLEN vor der Rendering-Verarbeitung zusammengefasst werden.
8. Rendering MUSS auf konsistenten Zustandsversionen basieren.
9. Transaktionen MÜSSEN unabhängig von konkreten Rendering-Backends funktionieren.
10. Die Transaktionsverarbeitung MUSS deterministische Zustandsübergänge ermöglichen.

## 11. Ergebnis

Nova Graphics State Transactions gewährleistet atomare, konsistente und deterministische Änderungen grafischer Zustände.

Durch die gemeinsame Übernahme zusammengehöriger Änderungen verhindert NovaOS unvollständige Darstellungen und vermeidet unnötige Zwischenberechnungen.

Die Transaktionsarchitektur bildet damit eine zentrale Grundlage für State-Driven Graphics, den persistenten Scene Graph und inkrementelles Rendering.
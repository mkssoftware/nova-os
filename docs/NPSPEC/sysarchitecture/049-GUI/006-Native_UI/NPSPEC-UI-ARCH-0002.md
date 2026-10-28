# NPSPEC-UI-ARCH-0002 – Nova UI Runtime

**Status:** Angenommen  
**Bereich:** UI / Architecture / Runtime  
**System:** NovaOS

## 1. Zweck

Nova UI Runtime definiert die Laufzeitumgebung für native Benutzeroberflächen innerhalb von NovaOS.

Sie verwaltet den Lebenszyklus von UI-Instanzen, deren Zustände, Ereignisverarbeitung, Aktualisierungen und die Zusammenarbeit mit Layout Engine und Renderer.

Ziel ist eine performante, deterministische und ressourcenschonende Ausführung deklarativer Benutzeroberflächen.

## 2. Architektur

Die Nova UI Runtime umfasst folgende Komponenten:

| Komponente | Aufgabe |
|---|---|
| UI Runtime Manager | Verwaltung der UI-Instanzen |
| UI Instance Context | Laufzeitkontext einer Oberfläche |
| UI Object Manager | Verwaltung von UI-Objekten und Hierarchien |
| UI Update Scheduler | Planung erforderlicher Aktualisierungen |
| UI Event Loop | Verarbeitung von UI-Ereignissen |
| UI State Manager | Verwaltung des Oberflächenzustands |
| UI Lifecycle Controller | Steuerung von Initialisierung und Beendigung |

Jede UI-Instanz besitzt einen eigenen Laufzeitkontext.

Die Runtime MUSS unabhängig vom verwendeten Renderbackend funktionieren.

## 3. UI-Lebenszyklus

Eine UI-Instanz durchläuft folgende Zustände:

1. **Created:** Laufzeitkontext wurde erstellt.
2. **Initializing:** UI-Definition und Ressourcen werden initialisiert.
3. **Ready:** Oberfläche ist bereit zur Darstellung.
4. **Active:** Oberfläche verarbeitet Ereignisse und Aktualisierungen.
5. **Suspended:** Verarbeitung ist vorübergehend eingeschränkt.
6. **Closing:** Oberfläche wird kontrolliert beendet.
7. **Disposed:** Ressourcen wurden freigegeben.

Ungültige Zustandsübergänge MÜSSEN verhindert werden.

Die Runtime MUSS eine kontrollierte Beendigung auch bei fehlerhaften UI-Komponenten ermöglichen.

## 4. UI-Instanzen

Eine UI-Instanz enthält mindestens:

- Eindeutige Instanzkennung.
- Zugehörigen Prozess und Sicherheitskontext.
- UI-Objektstruktur.
- Aktuellen UI-Zustand.
- Ereigniswarteschlange.
- Aktualisierungsstatus.
- Referenzen auf benötigte Ressourcen.

Mehrere UI-Instanzen DÜRFEN innerhalb eines Prozesses existieren.

UI-Instanzen MÜSSEN logisch voneinander getrennt verwaltet werden.

## 5. UI Event Loop

Die UI Event Loop verarbeitet Ereignisse innerhalb des jeweiligen Laufzeitkontexts.

Dazu gehören:

- Benutzereingaben.
- Fokusänderungen.
- Eigenschaftsänderungen.
- Datenbindungsaktualisierungen.
- Timer.
- Animationen.
- Lebenszyklusereignisse.

Die Ereignisverarbeitung erfolgt in definierter Reihenfolge.

Lang laufende Operationen DÜRFEN die UI Event Loop nicht dauerhaft blockieren.

Asynchrone Operationen MÜSSEN unterstützt werden.

## 6. UI Update Scheduler

Der UI Update Scheduler koordiniert erforderliche Aktualisierungen.

Dabei unterscheidet Nova UI:

- Property Update.
- Binding Update.
- Layout Update.
- Visual Update.
- Render Update.

Änderungen MÜSSEN nur die tatsächlich betroffenen Verarbeitungsschritte auslösen.

Mehrere kompatible Aktualisierungen DÜRFEN zusammengefasst werden.

Unveränderte UI-Bereiche DÜRFEN keine unnötigen Layout- oder Renderoperationen verursachen.

Die Verarbeitung MUSS unabhängig von einer permanent laufenden Render-Schleife funktionieren.

## 7. Zustandsverwaltung

Der UI State Manager verwaltet den aktuellen Zustand einer Oberfläche.

Dazu gehören:

- Eigenschaften.
- Datenbindungen.
- Auswahlzustände.
- Scrollpositionen.
- Fokusinformationen.
- Sichtbarkeitszustände.
- Benutzerdefinierte UI-Zustände.

Zustandsänderungen MÜSSEN konsistent erfolgen.

Zusammengehörige Änderungen SOLLEN transaktional angewendet werden können.

Persistierbare Zustände MÜSSEN über definierte Schnittstellen gespeichert und wiederhergestellt werden können.

## 8. Threading und Nebenläufigkeit

Jede UI-Instanz besitzt einen eindeutig definierten Ausführungskontext für Änderungen an ihrer UI-Objektstruktur.

Dabei gilt:

- UI-Zustandsänderungen werden serialisiert.
- Hintergrundaufgaben DÜRFEN keine unkoordinierten Änderungen an UI-Objekten durchführen.
- Ergebnisse asynchroner Operationen werden kontrolliert in den UI-Kontext übernommen.
- Die Runtime MUSS Structured Concurrency unterstützen können.
- Beendete UI-Instanzen MÜSSEN zugehörige laufende Aufgaben kontrolliert abbrechen können.

Rendering und andere rechenintensive Operationen DÜRFEN auf getrennten Ausführungskontexten erfolgen.

## 9. Ressourcenverwaltung

Die Runtime verwaltet die Lebensdauer UI-bezogener Ressourcen.

Dazu gehören:

- UI-Objekte.
- Styles und Templates.
- Bilder und Schriftressourcen.
- Datenbindungen.
- Ereignisregistrierungen.
- Animationen.
- Renderressourcen.

Nicht mehr benötigte Ressourcen MÜSSEN freigegeben werden.

Gemeinsam nutzbare unveränderliche Ressourcen SOLLEN wiederverwendet werden.

Die Runtime MUSS Speicher- und Ressourcenlimits berücksichtigen.

## 10. Fehlerbehandlung

Fehler innerhalb einer UI-Instanz MÜSSEN kontrolliert behandelt werden.

Dabei gilt:

- Fehlerhafte Ereignisbehandlungen DÜRFEN andere UI-Instanzen nicht beeinträchtigen.
- Ungültige Datenbindungen werden kontrolliert deaktiviert oder als fehlerhaft markiert.
- Fehlerhafte Layout- oder Renderanforderungen DÜRFEN keine unkontrollierten Endlosschleifen verursachen.
- Ressourcen MÜSSEN auch bei fehlerhafter Beendigung freigegeben werden.
- Diagnosedaten SOLLEN über die NovaOS-Diagnoseinfrastruktur bereitgestellt werden.

Eine automatische Wiederherstellung DARF erfolgen, sofern ein konsistenter Zustand verfügbar ist.

## 11. Sicherheit

Die Nova UI Runtime unterliegt Nova.Trust.

- UI-Instanzen arbeiten innerhalb ihres autorisierten Sicherheitskontexts.
- Ereignisbehandlungen DÜRFEN keine Capability-Grenzen umgehen.
- Datenbindungen MÜSSEN bestehende Zugriffsrechte beachten.
- Ressourcenfreigaben zwischen Prozessen MÜSSEN autorisiert werden.
- Dynamische UI-Definitionen MÜSSEN vor der Ausführung validiert werden.
- Eine UI-Definition DARF keine zusätzlichen Berechtigungen verleihen.

## 12. Normative Anforderungen

- **UI-001:** NovaOS MUSS eine zentrale Laufzeitarchitektur für native UI-Instanzen bereitstellen.
- **UI-002:** Jede UI-Instanz MUSS einen eigenen Laufzeitkontext besitzen.
- **UI-003:** UI-Instanzen MÜSSEN einen definierten Lebenszyklus besitzen.
- **UI-004:** Die Runtime MUSS eine ereignisgesteuerte Verarbeitung unterstützen.
- **UI-005:** UI-Zustandsänderungen MÜSSEN konsistent und serialisiert erfolgen.
- **UI-006:** Asynchrone Operationen MÜSSEN unterstützt werden.
- **UI-007:** Aktualisierungen MÜSSEN bedarfsorientiert geplant werden.
- **UI-008:** Unveränderte UI-Bereiche DÜRFEN keine unnötigen Neuberechnungen verursachen.
- **UI-009:** Zusammengehörige Zustandsänderungen SOLLEN transaktional verarbeitet werden können.
- **UI-010:** UI-Zustände MÜSSEN über definierte Schnittstellen persistierbar sein.
- **UI-011:** Ressourcen MÜSSEN bei Beendigung einer UI-Instanz kontrolliert freigegeben werden.
- **UI-012:** Fehler einzelner UI-Instanzen DÜRFEN andere Instanzen nicht beeinträchtigen.
- **UI-013:** Die Runtime MUSS unabhängig vom Renderbackend funktionieren.
- **UI-014:** UI-Instanzen MÜSSEN die Sicherheitsrichtlinien von Nova.Trust einhalten.
- **UI-015:** Identische Ausgangszustände und Ereignisfolgen MÜSSEN reproduzierbare UI-Zustände erzeugen.
- **UI-016:** Nova UI Runtime MUSS vollständig offline und ohne KI-Abhängigkeit funktionieren.

## 13. Integration

- **Nova UI Architecture:** Übergeordnete UI-Architektur.
- **Nova UI Object Model:** Verwaltung der UI-Elemente.
- **Nova UI Property System:** Eigenschaften und Änderungsbenachrichtigungen.
- **Nova UI Layout Engine:** Layoutberechnung.
- **Nova UI Data Binding:** Datenbindungen.
- **Nova UI Event System:** Ereignisverarbeitung.
- **Nova UI Renderer Interface:** Darstellung.
- **Nova Input Architecture:** Eingabeereignisse.
- **Nova Window Manager:** Fensterlebenszyklus.
- **Nova Scheduler:** Ausführungsplanung.
- **Nova Memory Management:** Ressourcenverwaltung.
- **Nova.Trust:** Sicherheit und Isolation.

## 14. Ergebnis

NovaOS erhält eine eigenständige Laufzeitumgebung für native Benutzeroberflächen.

Die Runtime verwaltet UI-Instanzen, Zustände und Ereignisse und führt nur tatsächlich erforderliche Aktualisierungen aus.

Dadurch bleiben Benutzeroberflächen reaktionsschnell, ressourcenschonend und unabhängig vom verwendeten Renderer.

**Grundsatz:** Nova UI Runtime verarbeitet Änderungen, wenn sie entstehen – nicht permanent, nur weil eine Oberfläche sichtbar ist.
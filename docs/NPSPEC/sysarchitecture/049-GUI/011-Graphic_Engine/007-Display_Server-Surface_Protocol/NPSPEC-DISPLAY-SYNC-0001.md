# NPSPEC-DISPLAY-SYNC-0001 – Nova Surface Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Display Server

## 1. Zweck

Nova Surface Synchronization definiert die Synchronisation grafischer Surfaces und ihrer Buffer zwischen Anwendungen, Render Engine, Compositor und Display Server.

Ziel ist die sichere, möglichst blockierungsarme Zusammenarbeit aller Komponenten ohne Datenrennen, unvollständige Bildinhalte oder vorzeitige Ressourcenfreigaben.

## 2. Architekturprinzipien

- **Explicit Synchronization:** Zugriffsabhängigkeiten werden über Synchronisationsobjekte beschrieben.
- **Asynchronous:** CPU und GPU arbeiten möglichst unabhängig.
- **Producer-Consumer:** Erzeugung und Verwendung grafischer Inhalte sind entkoppelt.
- **Fence-Based:** Fertigstellung und Freigabe werden explizit signalisiert.
- **Versioned:** Synchronisation ist eindeutig einem Buffer- und Inhaltszustand zugeordnet.
- **Zero-Copy:** Gemeinsame Buffer werden ohne unnötige Kopien verwendet.
- **Lifetime-Safe:** Ressourcen bleiben während aktiver Zugriffe gültig.

## 3. Synchronization Model

Eine Surface besitzt Synchronisationsinformationen für ihre veröffentlichten Buffer-Zustände.

Mindestens erforderlich sind:

| Eigenschaft | Beschreibung |
|---|---|
| Surface ID | Zugehörige Surface |
| Buffer ID | Betroffener Buffer |
| Content Version | Version des Inhalts |
| Acquire Fence | Freigabe zur Verwendung |
| Release Fence | Abschluss der Verwendung |
| Access State | Aktuelle Zugriffsart |
| Synchronization Status | Zustand der Synchronisation |

Synchronisationsobjekte MÜSSEN unabhängig von der jeweiligen Surface-Identität verwaltet werden können.

## 4. Producer-Consumer Synchronization

Der grundlegende Ablauf lautet:

1. Der Producer erhält einen beschreibbaren Buffer.
2. Der Producer erzeugt oder aktualisiert dessen Inhalt.
3. Der Producer veröffentlicht den Buffer mit einem Acquire Fence.
4. Der Consumer wartet die erforderliche Fertigstellung ab.
5. Der Consumer verarbeitet oder präsentiert den Inhalt.
6. Der Consumer signalisiert den Abschluss seiner Verwendung.
7. Der Buffer kann entsprechend seiner verbleibenden Verwendungen erneut genutzt werden.

Mehrere Consumer DÜRFEN denselben unveränderlichen Buffer parallel lesen.

Schreibzugriffe MÜSSEN gegen konkurrierende Zugriffe abgesichert werden.

## 5. Acquire Synchronization

Ein Acquire Fence beschreibt die Fertigstellung vorausgehender Schreiboperationen.

Dabei gilt:

- Der Fence MUSS dem zugehörigen Buffer-Zustand zugeordnet sein.
- Ein Consumer DARF den Inhalt erst nach erfüllter Synchronisation verwenden.
- GPU-seitiges Warten SOLL gegenüber blockierendem CPU-Warten bevorzugt werden.
- Fehlgeschlagene Synchronisation MUSS erkannt werden.

Die Annahme eines Surface-Commits DARF nicht mit der Signalisierung seines Acquire Fence gleichgesetzt werden.

## 6. Release Synchronization

Ein Release Fence beschreibt den Abschluss einer definierten Buffer-Verwendung.

Dabei gilt:

- Die Freigabe MUSS sämtliche zugehörigen Lese- oder Schreibzugriffe berücksichtigen.
- Ein Buffer DARF nicht vorzeitig überschrieben werden.
- Mehrere Consumer MÜSSEN unabhängig berücksichtigt werden.
- Laufende Scanout-Zugriffe MÜSSEN eingeschlossen sein.

Die Freigabe zur Wiederverwendung ist von der endgültigen Speicherfreigabe zu unterscheiden.

## 7. CPU-GPU-Display Synchronization

NovaOS MUSS Synchronisationsabhängigkeiten zwischen folgenden Komponenten unterstützen:

- CPU → GPU
- GPU → CPU
- GPU → GPU
- GPU → Display
- Display → Producer

Dabei MÜSSEN auch Speicher-Sichtbarkeit und erforderliche Cache-Kohärenz berücksichtigt werden.

Ein signalisierter Fence allein ersetzt keine notwendigen Speicherbarrieren oder Cache-Operationen.

## 8. Surface Commit Integration

Nova Surface Synchronization MUSS mit Nova Atomic Surface Commit zusammenarbeiten.

Ein Commit kann einen neuen Buffer und dessen Acquire Fence veröffentlichen.

Dabei gilt:

- Der Commit veröffentlicht einen konsistenten Surface-Zustand.
- Die Buffer-Verfügbarkeit wird separat synchronisiert.
- Der Compositor darf den Zustand übernehmen, ohne sofort auf den Buffer zuzugreifen.
- Nicht mehr benötigte Buffer werden erst nach Abschluss ihrer Verwendung freigegeben.

Ein fehlgeschlagener Fence DARF nicht zur Darstellung unvollständiger Inhalte führen.

## 9. Asynchronous Execution

NovaOS SOLL blockierende Warteoperationen minimieren.

Dazu gehören:

- Asynchrone Fence-Signalisierung
- GPU-seitige Abhängigkeiten
- Ereignisgesteuerte Benachrichtigungen
- Parallele Verarbeitung unabhängiger Surfaces
- Vermeidung unnötiger CPU-GPU-Roundtrips

Unabhängige Surfaces SOLLEN nicht durch fremde Synchronisationsvorgänge blockiert werden.

## 10. Deadlock und Fehlerbehandlung

NovaOS MUSS ungültige Synchronisationszustände erkennen und kontrolliert behandeln können.

Dazu gehören:

- Ungültige Fence-Handles
- Nicht erfüllbare Abhängigkeiten
- Geräteverlust
- Fehlgeschlagene GPU-Operationen
- Abgebrochene Producer
- Fehlerhafte Buffer-Zustände

Zyklische Synchronisationsabhängigkeiten MÜSSEN verhindert oder sicher behandelt werden.

Ein Timeout DARF nicht automatisch als erfolgreicher Abschluss eines Hardwarezugriffs interpretiert werden.

Bei nicht verfügbarer Synchronisation MUSS ein sicherer Fallback möglich sein.

## 11. Sicherheit und Ressourcenverwaltung

Synchronisationsobjekte MÜSSEN über Nova Capabilities geschützt werden.

Dabei gilt:

- Unberechtigte Clients dürfen fremde Fences nicht manipulieren.
- Handles und Zustandsübergänge müssen validiert werden.
- Ressourcenverbrauch muss begrenzbar sein.
- Aktive Buffer und Synchronisationsobjekte dürfen nicht vorzeitig freigegeben werden.
- Fehlerhafte Clients dürfen andere Surface-Verbindungen nicht kompromittieren.

## 12. Verbindliche Anforderungen

1. NovaOS MUSS explizite Surface-Synchronisation unterstützen.
2. Acquire- und Release-Synchronisation MÜSSEN verfügbar sein.
3. Synchronisation MUSS Buffer- und Inhaltsversionen eindeutig zuordenbar sein.
4. Unvollständige Schreiboperationen DÜRFEN nicht als gültige Inhalte verwendet werden.
5. Mehrere lesende Consumer MÜSSEN unterstützt werden.
6. Konkurrierende Schreibzugriffe MÜSSEN kontrolliert werden.
7. CPU-, GPU- und Display-Zugriffe MÜSSEN korrekt synchronisiert werden.
8. Speicher-Sichtbarkeit und Cache-Kohärenz MÜSSEN berücksichtigt werden.
9. Blockierende CPU-Wartezeiten SOLLEN minimiert werden.
10. Surface-Commits und Buffer-Verfügbarkeit MÜSSEN getrennt behandelt werden.
11. Buffer DÜRFEN während aktiver Verwendung nicht freigegeben werden.
12. Ungültige oder zyklische Synchronisationsabhängigkeiten MÜSSEN sicher behandelt werden.
13. Synchronisationsobjekte MÜSSEN capability-basiert geschützt sein.
14. Fehlerhafte Synchronisation DARF keine unvollständigen Bildinhalte sichtbar machen.
15. Im deterministischen Modus MÜSSEN Synchronisationszustände und Abhängigkeitsreihenfolgen reproduzierbar sein.

## 13. Ergebnis

Nova Surface Synchronization gewährleistet die sichere Zusammenarbeit zwischen grafischen Produzenten und Konsumenten.

Durch explizite Acquire- und Release-Fences, asynchrone Verarbeitung und kontrollierte Buffer-Lebenszeiten werden unnötige Wartezeiten reduziert und Datenrennen verhindert.

Damit bildet das Subsystem die Grundlage für zuverlässiges Zero-Copy Sharing, Incremental Composition und synchronisierte Frame-Präsentation innerhalb von NovaOS.
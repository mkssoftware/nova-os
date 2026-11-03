# NPSPEC-GPUSYNC-SURFACE-0001 – Nova Surface Access Synchronization

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova GPU Synchronization

## 1. Zweck

Nova Surface Access Synchronization definiert die Synchronisation von Lese- und Schreibzugriffen auf gemeinsam verwendete Grafikoberflächen (Surfaces).

Ziel ist die sichere Zusammenarbeit zwischen Anwendungen, Render Engine, Compositor, GPU und Display Server ohne unnötige Datenkopien oder CPU-Blockierungen.

Die Synchronisation MUSS unabhängig vom verwendeten Grafikbackend funktionieren.

## 2. Architekturprinzipien

- **Explicit Synchronization:** Zugriffsabhängigkeiten werden eindeutig beschrieben.
- **Producer-Consumer:** Erzeuger und Verbraucher werden kontrolliert synchronisiert.
- **Zero-Copy:** Gemeinsame Buffer werden möglichst ohne Kopieren verwendet.
- **Multi-Consumer:** Mehrere gleichzeitige Leser werden unterstützt.
- **Resource-Safe:** Buffer werden nicht während aktiver Zugriffe überschrieben.
- **Non-Blocking:** CPU-seitige Warteoperationen werden minimiert.
- **Backend-Independent:** Einheitliches Synchronisationsmodell für unterschiedliche GPU-Treiber.

## 3. Surface Access Model

NovaOS unterscheidet folgende Zugriffsarten:

| Zugriff | Beschreibung |
|---|---|
| Read | Lesen vorhandener Surface-Inhalte |
| Write | Verändern von Surface-Inhalten |
| Read-Write | Kombinierter Lese- und Schreibzugriff |
| Scanout | Verwendung durch die Display-Hardware |
| Transfer | Kopieren oder Übertragen von Surface-Daten |

Jeder Zugriff besitzt mindestens:

- Surface ID
- Buffer ID
- Access Type
- Access Owner
- Acquire Dependency
- Release Completion
- Resource State

Die Synchronisation MUSS auf den tatsächlich verwendeten Buffer bezogen sein, nicht ausschließlich auf die logische Surface.

## 4. Producer-Consumer Synchronization

NovaOS MUSS ein kontrolliertes Producer-Consumer-Modell unterstützen.

Dabei gilt:

1. Der Producer erzeugt oder verändert einen Surface-Buffer.
2. Nach Abschluss seiner Schreiboperationen stellt er eine Acquire-Bedingung bereit.
3. Der Consumer wartet vor dem Zugriff auf diese Bedingung.
4. Nach Abschluss seiner Verwendung stellt der Consumer eine Release-Bedingung bereit.
5. Der Producer darf den Buffer erst wiederverwenden, wenn alle relevanten Consumer-Zugriffe abgeschlossen sind.

Ein Surface Commit bedeutet nicht automatisch, dass sämtliche GPU-Schreiboperationen abgeschlossen sind.

## 5. Acquire Synchronization

Acquire Synchronization stellt sicher, dass ein Consumer gültige Buffer-Inhalte erhält.

Unterstützte Mechanismen:

- GPU Semaphores
- Timeline Synchronization
- GPU Fences
- Backend-spezifische Synchronisationsobjekte

Dabei gilt:

- Acquire-Bedingungen MÜSSEN vor abhängigen Zugriffen erfüllt sein.
- Erforderliche Speicher- und Cache-Sichtbarkeit MUSS gewährleistet werden.
- Unnötiges CPU-seitiges Warten SOLL vermieden werden.
- Fehlgeschlagene Acquire-Bedingungen DÜRFEN keinen ungeschützten Zugriff ermöglichen.

## 6. Release Synchronization

Release Synchronization signalisiert das Ende einer definierten Buffer-Verwendung.

Dabei gilt:

- Release erfolgt erst nach Abschluss aller zugehörigen Zugriffe.
- Mehrere Consumer MÜSSEN unabhängig berücksichtigt werden.
- Ein Buffer DARF nicht vorzeitig überschrieben oder freigegeben werden.
- Fehlgeschlagene GPU-Operationen MÜSSEN kontrolliert behandelt werden.
- Release-Bedingungen MÜSSEN eindeutig dem jeweiligen Buffer zugeordnet sein.

Eine abgeschlossene Frame-Präsentation bedeutet nicht automatisch, dass die Display-Hardware den Buffer bereits freigegeben hat.

## 7. Multi-Consumer Synchronization

NovaOS MUSS mehrere gleichzeitige Consumer unterstützen.

Beispiele:

- Compositor
- Display Server
- Hardware Overlay
- Screenshot-Dienst
- Weitere autorisierte Grafikprozesse

Dabei gilt:

- Gleichzeitige Lesezugriffe können zulässig sein.
- Schreibzugriffe erfordern geeignete Exklusivität oder nachweislich konfliktfreie Bereiche.
- Jeder Consumer besitzt eine nachvollziehbare Abschlussbedingung.
- Die Wiederverwendung erfolgt erst nach Freigabe durch alle relevanten Consumer.

## 8. Surface Commit Integration

Nova Surface Access Synchronization MUSS mit Nova Atomic Surface Commit zusammenarbeiten.

Dabei gilt:

- Surface Commits veröffentlichen einen konsistenten Surface-Zustand.
- Buffer-Referenzen und Acquire-Bedingungen werden atomar übernommen.
- Noch ausstehende GPU-Arbeit DARF durch einen Commit nicht als abgeschlossen gelten.
- Ersetzte Buffer bleiben bis zum Ende ihrer tatsächlichen Verwendung geschützt.
- Neue Commits DÜRFEN laufende Consumer-Zugriffe nicht unzulässig beeinträchtigen.

## 9. Compositor und Display Integration

Nova Surface Access Synchronization MUSS mit Nova Compositor und Nova Display Server zusammenarbeiten.

Dabei gilt:

- Der Compositor wartet auf erforderliche Producer-Abschlüsse.
- Composition-Operationen erzeugen eigene Abschlussbedingungen.
- Direct Scanout und Hardware Overlays berücksichtigen die Buffer-Lebensdauer.
- Presentation Synchronization wird mit Surface Access Synchronization abgestimmt.
- Die Display-Hardware DARF keine noch unvollständig geschriebenen Buffer verwenden.

## 10. Zero-Copy Integration

NovaOS SOLL Surface-Buffer ohne unnötige Kopien zwischen Prozessen und GPU-Komponenten austauschen.

Voraussetzungen:

- Kompatible Buffer-Formate und Speicherlayouts.
- Gültige Zugriffsberechtigungen.
- Korrekte Acquire- und Release-Synchronisation.
- Geeignete Cache-Kohärenz.
- Kontrollierte Ressourcenlebensdauer.

Zero-Copy DARF nicht zulasten der Speicher- oder Zugriffssicherheit erfolgen.

## 11. Performance und Optimierung

NovaOS SOLL den Synchronisationsaufwand minimieren.

Dazu gehören:

- GPU-seitige Synchronisation.
- Timeline-basierte Abschlussverwaltung.
- Vermeidung unnötiger Buffer-Kopien.
- Parallele unabhängige Lesezugriffe.
- Wiederverwendung freigegebener Buffer.
- Vermeidung globaler GPU-Stalls.

Synchronisationsoptimierungen DÜRFEN keine Datenkonflikte verursachen.

## 12. Sicherheit und Isolation

Surface-Zugriffe MÜSSEN capability-basiert geschützt werden.

Dabei gilt:

- Producer und Consumer benötigen entsprechende Zugriffsrechte.
- Schreibrechte MÜSSEN von Leserechten unterscheidbar sein.
- Prozessübergreifende Buffer-Freigaben erfordern ausdrückliche Berechtigungen.
- Ungültige Surface- und Buffer-Handles MÜSSEN zurückgewiesen werden.
- Synchronisationsobjekte DÜRFEN nicht unautorisiert verändert werden.

## 13. Fehlerbehandlung

NovaOS MUSS folgende Fehler kontrolliert behandeln:

- Ungültige Acquire-Bedingungen.
- Nicht erfüllte Release-Bedingungen.
- Gleichzeitige inkompatible Buffer-Zugriffe.
- Fehlgeschlagene GPU-Operationen.
- GPU-Timeouts.
- Geräteverlust.
- Vorzeitige Buffer-Freigabeversuche.

Dabei gilt:

- Fehler DÜRFEN keine erfolgreiche Freigabe vortäuschen.
- Betroffene Buffer MÜSSEN bis zur sicheren Freigabe geschützt bleiben.
- Bei Geräteverlust MUSS eine kontrollierte Invalidierung oder Wiederherstellung erfolgen.

## 14. Verbindliche Anforderungen

1. NovaOS MUSS Surface Access Synchronization bereitstellen.
2. Synchronisation MUSS bufferbezogen erfolgen.
3. Producer-Consumer-Abhängigkeiten MÜSSEN unterstützt werden.
4. Acquire- und Release-Synchronisation MÜSSEN verfügbar sein.
5. Mehrere Consumer MÜSSEN berücksichtigt werden.
6. Inkompatible Lese- und Schreibzugriffe MÜSSEN verhindert werden.
7. GPU Fences und Semaphores MÜSSEN integrierbar sein.
8. Timeline Synchronization SOLL unterstützt werden.
9. Surface Commits MÜSSEN Synchronisationsbedingungen erhalten.
10. Buffer DÜRFEN nicht vorzeitig wiederverwendet werden.
11. Compositor und Display Server MÜSSEN integriert werden können.
12. Direct Scanout und Hardware Overlays MÜSSEN berücksichtigt werden.
13. Zero-Copy SOLL bevorzugt werden, sofern sicher möglich.
14. CPU-seitiges Busy Waiting SOLL vermieden werden.
15. Surface-Zugriffe MÜSSEN capability-basiert geschützt werden.
16. Fehlerhafte Synchronisationszustände MÜSSEN kontrolliert behandelt werden.
17. Im deterministischen Modus MÜSSEN logische Zugriffsabhängigkeiten und Freigabeentscheidungen bei identischen Eingaben reproduzierbar sein.

## 15. Ergebnis

Nova Surface Access Synchronization gewährleistet den sicheren Zugriff auf gemeinsam verwendete Grafikoberflächen.

Durch Acquire- und Release-Synchronisation, Multi-Consumer-Unterstützung und kontrollierte Buffer-Lebenszyklen können Anwendungen, Compositor und Display Server effizient zusammenarbeiten.

Damit schafft NovaOS die Grundlage für sichere Zero-Copy-Grafikverarbeitung, parallele Surface-Nutzung und zuverlässige Bildschirmpräsentation.
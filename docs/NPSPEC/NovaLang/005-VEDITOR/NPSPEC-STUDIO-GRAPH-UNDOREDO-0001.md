
# NPSPEC-STUDIO-GRAPH-UNDOREDO-0001 – NovaLang Studio Graph Undo/Redo

## Status

Angenommen

## Kategorie

NovaOS / NovaLang Studio / Logic Graph Editor / Undo & Redo

## Zweck

Definiert das Rückgängigmachen und Wiederherstellen von Änderungen innerhalb des Logic Graph Editors.

Ziel ist eine zuverlässige, konsistente und ressourcenschonende Bearbeitung von Logic Graphs ohne unbeabsichtigten Verlust von Änderungen.

## Architektur

Das Undo/Redo-System besteht aus:

- **Command Manager:** Verwaltung rückgängig machbarer Editoraktionen.
- **Undo Stack:** Speicherung vorheriger Änderungen.
- **Redo Stack:** Speicherung rückgängig gemachter Aktionen.
- **Transaction Controller:** Zusammenfassung zusammengehöriger Änderungen.
- **State Restorer:** Wiederherstellung konsistenter Graphzustände.

Das System arbeitet auf dem Graphmodell und verwendet dessen Validierungs- und Serialisierungsregeln.

## Unterstützte Aktionen

- Knoten erstellen und löschen
- Knoten verschieben und kopieren
- Verbindungen erstellen, ändern und entfernen
- Eigenschaften bearbeiten
- Gruppen erstellen und auflösen
- Subgraphs erstellen und bearbeiten
- Layoutänderungen durchführen
- Mehrfachänderungen als gemeinsame Aktion ausführen

## Funktionsweise

Jede abgeschlossene Änderung wird als umkehrbare Editoraktion registriert.

- **Undo:** Macht die letzte abgeschlossene Aktion rückgängig.
- **Redo:** Stellt eine rückgängig gemachte Aktion wieder her.
- **Grouped Action:** Behandelt mehrere zusammengehörige Änderungen als eine Aktion.

Eine neue Änderung nach einem Undo verwirft den nicht mehr gültigen Redo-Verlauf.

## Konsistenz

Undo und Redo müssen Knotenidentitäten, Portreferenzen, Verbindungen und Graphversionen konsistent behandeln.

Zusammengehörige Änderungen werden atomar übernommen oder vollständig verworfen.

Nach strukturellen Änderungen wird die betroffene Graphvalidierung aktualisiert.

## Verlauf und Ressourcen

Der Änderungsverlauf ist pro geöffnetem Graphdokument isoliert.

Die Speichertiefe ist konfigurierbar und durch Ressourcenlimits begrenzt.

Ältere Einträge dürfen kontrolliert entfernt werden, ohne den aktuellen Graphzustand zu verändern.

## Sicherheit

Undo und Redo betreffen ausschließlich Editoränderungen.

Bereits ausgeführte Capability-Operationen, externe Seiteneffekte und erteilte Berechtigungen dürfen dadurch nicht rückgängig gemacht oder verändert werden.

Sicherheitsrelevante Graphänderungen unterliegen weiterhin der Solution-Integritäts- und Berechtigungsprüfung.

## Normative Anforderungen

1. Der Graph Editor MUSS Undo und Redo unterstützen.
2. Jede abgeschlossene Graphänderung MUSS rückgängig machbar sein, soweit sie eine Editoraktion darstellt.
3. Zusammengehörige Änderungen MÜSSEN als atomare Aktion behandelbar sein.
4. Undo und Redo MÜSSEN Graphidentitäten und Referenzen konsistent erhalten.
5. Neue Änderungen nach Undo MÜSSEN den ungültigen Redo-Verlauf verwerfen.
6. Änderungen MÜSSEN die erforderliche Graphvalidierung auslösen.
7. Der Verlauf MUSS pro Graphdokument isoliert sein.
8. Die Verlaufsspeicherung MUSS ressourcenbegrenzt sein.
9. Undo und Redo DÜRFEN keine externen Capability-Seiteneffekte rückgängig machen.
10. Undo und Redo DÜRFEN keine Sicherheits- oder Berechtigungsprüfungen umgehen.
11. Das System MUSS unabhängig von KI funktionieren.

## Ergebnis

NovaLang Studio erhält ein konsistentes, transaktionales und ressourcenschonendes Undo/Redo-System für die sichere Bearbeitung komplexer Logic Graphs.

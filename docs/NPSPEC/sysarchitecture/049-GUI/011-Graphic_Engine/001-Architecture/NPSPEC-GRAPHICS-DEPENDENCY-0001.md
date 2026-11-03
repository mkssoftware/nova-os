# NPSPEC-GRAPHICS-DEPENDENCY-0001 – Nova Graphics Dependency Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics

## 1. Zweck

Nova Graphics Dependency Tracking definiert die Erfassung und Verwaltung von Abhängigkeiten zwischen grafischen Zuständen, Elementen, Oberflächen und Effekten.

NovaOS erkennt, welche grafischen Ergebnisse durch eine Zustandsänderung beeinflusst werden, und aktualisiert ausschließlich die erforderlichen Abhängigkeiten.

Ziel ist eine korrekte, deterministische und ressourcenschonende Grafikverarbeitung.

## 2. Architekturprinzipien

- **Dependency-Aware:** Grafische Abhängigkeiten werden explizit verwaltet.
- **Change-Driven:** Änderungen lösen die Auswertung betroffener Abhängigkeiten aus.
- **Incremental:** Unveränderte Abhängigkeiten werden nicht unnötig verarbeitet.
- **Deterministic:** Identische Zustände und Änderungen erzeugen identische logische Ergebnisse.
- **Transactional:** Abhängigkeiten werden konsistent mit Zustandsänderungen aktualisiert.
- **Hierarchical:** Abhängigkeiten können innerhalb grafischer Hierarchien bestehen.
- **Cross-Surface:** Abhängigkeiten zwischen unterschiedlichen Oberflächen werden berücksichtigt.

## 3. Abhängigkeitsmodell

Grafische Abhängigkeiten werden als gerichtete Beziehungen zwischen Zuständen und Verarbeitungsergebnissen verwaltet.

Eine Abhängigkeit umfasst:

- Eindeutige Identität der beteiligten Objekte
- Quelle der Abhängigkeit
- Abhängiges Ziel
- Abhängigkeitstyp
- Betroffene Eigenschaften oder Ressourcen
- Gültigkeits- und Versionsinformationen

Abhängigkeiten können dynamisch entstehen, verändert und entfernt werden.

## 4. Abhängigkeitstypen

NovaOS unterscheidet insbesondere:

- **Property Dependencies:** Abhängigkeiten zwischen Eigenschaften.
- **Layout Dependencies:** Auswirkungen auf Position, Größe und Anordnung.
- **Transform Dependencies:** Abhängigkeiten durch Transformationen.
- **Visibility Dependencies:** Auswirkungen durch Sichtbarkeit und Überlagerung.
- **Material Dependencies:** Abhängigkeiten von Materialparametern.
- **Effect Dependencies:** Auswirkungen auf Blur, Schatten, Reflexionen und Lichtbrechung.
- **Surface Dependencies:** Abhängigkeiten zwischen grafischen Oberflächen.
- **Resource Dependencies:** Abhängigkeiten von Texturen, Buffern und anderen Grafikressourcen.

Weitere Abhängigkeitstypen DÜRFEN über definierte Schnittstellen ergänzt werden.

## 5. Dependency Graph

Abhängigkeiten werden in einem gerichteten Dependency Graph verwaltet.

Der Graph MUSS:

- Abhängigkeiten eindeutig zuordnen.
- Änderungen an Beziehungen unterstützen.
- Betroffene Ziele effizient ermitteln.
- Transitive Abhängigkeiten berücksichtigen.
- Ungültige Referenzen erkennen.
- Zyklen erkennen und kontrolliert behandeln.

Der Dependency Graph ergänzt den Nova Scene Graph und DARF dessen hierarchische Struktur nicht voraussetzen.

## 6. Änderungspropagation

Nach einer veröffentlichten Zustandsänderung ermittelt NovaOS die betroffenen Abhängigkeiten.

Die Verarbeitung umfasst:

1. Erfassen der geänderten Zustände.
2. Ermitteln unmittelbar abhängiger Elemente.
3. Verfolgen transitiver Abhängigkeiten.
4. Bestimmen erforderlicher Neuberechnungen.
5. Invalidieren betroffener Cache-Ergebnisse.
6. Weitergeben relevanter Änderungen an Layout, Rendering und Komposition.

Änderungen DÜRFEN zusammengefasst werden, sofern sämtliche Auswirkungen erhalten bleiben.

Unveränderte effektive Ergebnisse SOLLEN keine weitere Änderungspropagation auslösen.

## 7. Räumliche Abhängigkeiten

Grafische Abhängigkeiten können räumlich begrenzt sein.

NovaOS MUSS insbesondere berücksichtigen:

- Überlagernde Fenster und Oberflächen
- Transparente und halbtransparente Bereiche
- Hintergrundabhängige Materialeffekte
- Schatten und Effektgrenzen
- Clipping und Transformationen
- Änderungen der Stapelreihenfolge

Räumliche Abhängigkeiten SOLLEN auf die tatsächlich betroffenen Bereiche begrenzt werden.

Bei unvollständigen Abhängigkeitsinformationen MUSS eine konservative Aktualisierung erfolgen.

## 8. Integration mit Damage Tracking

Graphics Dependency Tracking ermittelt die logischen Auswirkungen einer Änderung.

Nova Damage Tracking bestimmt daraus die erforderlichen grafischen Aktualisierungsbereiche.

Beide Systeme MÜSSEN zusammenarbeiten, ohne ihre Verantwortlichkeiten zu vermischen.

Abhängige Effekte MÜSSEN auch außerhalb der ursprünglichen Änderungsregion berücksichtigt werden.

## 9. Cache-Invalidierung

Zwischengespeicherte Rendering-Ergebnisse besitzen Abhängigkeiten zu ihren Eingabedaten.

Ändert sich eine relevante Eingabe, MUSS das betroffene Cache-Ergebnis invalidiert oder aktualisiert werden.

Gültige Cache-Ergebnisse SOLLEN wiederverwendet werden.

Die Wiederverwendung DARF ausschließlich erfolgen, wenn sämtliche relevanten Abhängigkeiten weiterhin gültig sind.

## 10. Transaktionsintegration

Änderungen an grafischen Zuständen und ihren Abhängigkeiten MÜSSEN konsistent übernommen werden.

Graphics State Transactions gewährleisten, dass:

- Neue Abhängigkeiten atomar veröffentlicht werden.
- Entfernte Abhängigkeiten nicht weiterverwendet werden.
- Die Änderungspropagation auf konsistenten Zustandsversionen basiert.
- Fehlgeschlagene Transaktionen keine veröffentlichten Abhängigkeitsstrukturen verändern.

## 11. Verbindliche Anforderungen

1. Grafische Abhängigkeiten MÜSSEN eindeutig verwaltbar sein.
2. Direkte und transitive Abhängigkeiten MÜSSEN berücksichtigt werden.
3. Änderungen MÜSSEN an tatsächlich betroffene Ziele propagiert werden.
4. Zyklische und ungültige Abhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.
5. Abhängigkeiten MÜSSEN konsistent mit grafischen Zustandsänderungen aktualisiert werden.
6. Räumliche und materialabhängige Auswirkungen MÜSSEN berücksichtigt werden.
7. Betroffene Cache-Ergebnisse MÜSSEN korrekt invalidiert werden.
8. Unveränderte Ergebnisse SOLLEN keine unnötigen Folgeaktualisierungen auslösen.
9. Bei unsicheren Abhängigkeiten MUSS eine korrekte konservative Aktualisierung erfolgen.
10. Die Abhängigkeitsverwaltung MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 12. Ergebnis

Nova Graphics Dependency Tracking stellt eine zentrale Verwaltung grafischer Abhängigkeiten bereit.

NovaOS erkennt die Auswirkungen von Zustandsänderungen auf Elemente, Oberflächen, Materialien und Rendering-Ergebnisse.

Dadurch werden gezielte Aktualisierungen, sichere Cache-Wiederverwendung und effizientes inkrementelles Rendering ermöglicht.
# NPSPEC-GRAPHICS-CHANGE-0001 – Nova Graphics State Change Tracking

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Graphics

## 1. Zweck

Nova Graphics State Change Tracking definiert die Erkennung, Erfassung und Verwaltung grafischer Zustandsänderungen innerhalb von NovaOS.

Das System erkennt Änderungen an grafischen Elementen, Eigenschaften und Ressourcen und stellt diese für die weitere Verarbeitung bereit.

Ziel ist die präzise Ermittlung tatsächlicher Änderungen, um unnötige Layout-, Rendering- und Kompositionsoperationen zu vermeiden.

## 2. Architekturprinzipien

- **Change-Driven:** Nur relevante Zustandsänderungen lösen Aktualisierungen aus.
- **State-Based:** Änderungen werden anhand veröffentlichter Zustandsversionen ermittelt.
- **Incremental:** Unveränderte Zustände bleiben erhalten.
- **Transactional:** Zusammengehörige Änderungen werden gemeinsam erfasst.
- **Deterministic:** Identische Zustandsübergänge erzeugen identische logische Änderungsinformationen.
- **Efficient:** Mehrfache und redundante Änderungen werden zusammengefasst.
- **Dependency-Aware:** Erkannte Änderungen werden an das Dependency Tracking weitergegeben.

## 3. Änderungsmodell

Jede relevante Zustandsänderung wird als Change Record erfasst.

Ein Change Record enthält mindestens:

- Identität des betroffenen Elements oder der Ressource
- Art der Änderung
- Betroffene Eigenschaft oder Zustandskomponente
- Ausgangs- und Zielversion
- Änderungsrelevante Informationen

Änderungen werden eindeutig einer veröffentlichten Zustandsversion zugeordnet.

## 4. Änderungstypen

NovaOS unterscheidet insbesondere:

- **Property Change:** Änderung grafischer Eigenschaften.
- **Content Change:** Änderung dargestellter Inhalte.
- **Geometry Change:** Änderung von Position oder Größe.
- **Layout Change:** Änderung der Elementanordnung.
- **Transform Change:** Änderung grafischer Transformationen.
- **Visibility Change:** Änderung der Sichtbarkeit.
- **Material Change:** Änderung von Materialien oder Effekten.
- **Hierarchy Change:** Änderung grafischer Beziehungen.
- **Resource Change:** Änderung verwendeter Grafikressourcen.
- **Surface Change:** Änderung grafischer Oberflächen.

Weitere Änderungstypen DÜRFEN ergänzt werden.

## 5. Änderungserkennung

Grafische Änderungen MÜSSEN über kontrollierte Zustandszugriffe oder definierte Änderungsbenachrichtigungen erfasst werden.

Das System MUSS:

- Tatsächliche Zustandsänderungen erkennen.
- Unveränderte effektive Werte unterscheiden.
- Änderungen eindeutig zuordnen.
- Mehrfachänderungen zusammenfassen.
- Änderungen an Elementen und Ressourcen berücksichtigen.
- Die Reihenfolge relevanter Zustandsübergänge erhalten.

Nicht verwaltete externe Änderungen MÜSSEN über definierte Schnittstellen signalisiert werden.

## 6. Change Sets

Zusammengehörige Änderungen werden in Change Sets zusammengefasst.

Ein Change Set enthält die Änderungen einer erfolgreich übernommenen Graphics State Transaction.

Change Sets MÜSSEN:

- Einer eindeutigen Zustandsversion zugeordnet sein.
- Konsistente Änderungsinformationen enthalten.
- Redundante Änderungen vermeiden.
- Für nachgelagerte Verarbeitung verfügbar sein.

Mehrere Change Sets DÜRFEN zusammengeführt werden, sofern die korrekte Zustandsentwicklung erhalten bleibt.

## 7. Änderungsverarbeitung

Nach Veröffentlichung einer Zustandsänderung erfolgt:

1. Ermittlung der tatsächlichen Zustandsdifferenz.
2. Erstellung des Change Sets.
3. Klassifizierung der Änderungen.
4. Zusammenfassung redundanter Änderungen.
5. Übergabe an Graphics Dependency Tracking.
6. Auslösung erforderlicher Folgeaktualisierungen.

Das Change Tracking selbst führt keine Rendering-Operationen aus.

## 8. Integration mit Dependency und Damage Tracking

Nova Graphics State Change Tracking ermittelt, **was sich verändert hat**.

Nova Graphics Dependency Tracking bestimmt, **welche weiteren Zustände und Ergebnisse betroffen sind**.

Nova Damage Tracking bestimmt, **welche Bildbereiche aktualisiert werden müssen**.

Diese Verantwortlichkeiten MÜSSEN getrennt bleiben.

## 9. Änderungsoptimierung

NovaOS SOLL:

- Identische Ausgangs- und Zielwerte erkennen.
- Mehrfache Änderungen derselben Eigenschaft zusammenfassen.
- Unveränderte Teilstrukturen überspringen.
- Versionsinformationen zur Änderungserkennung verwenden.
- Änderungsinformationen nur so lange wie erforderlich vorhalten.

Optimierungen DÜRFEN keine relevanten Zustandsänderungen verlieren.

## 10. Fehlerbehandlung

Ungültige oder fehlgeschlagene Zustandsänderungen DÜRFEN keine veröffentlichten Change Sets erzeugen.

Bei unvollständigen Änderungsinformationen MUSS NovaOS eine konservative Aktualisierung des betroffenen Zustandsbereichs ermöglichen.

Veraltete Änderungsinformationen DÜRFEN nicht auf inkompatible Zustandsversionen angewendet werden.

## 11. Verbindliche Anforderungen

1. Relevante grafische Zustandsänderungen MÜSSEN zuverlässig erfasst werden.
2. Änderungen MÜSSEN eindeutig Elementen, Eigenschaften oder Ressourcen zugeordnet werden.
3. Change Sets MÜSSEN konsistent mit veröffentlichten Zustandsversionen sein.
4. Fehlgeschlagene Transaktionen DÜRFEN keine veröffentlichten Änderungen erzeugen.
5. Redundante Änderungen SOLLEN zusammengefasst werden.
6. Unveränderte effektive Zustände SOLLEN keine unnötigen Folgeaktualisierungen auslösen.
7. Änderungsinformationen MÜSSEN für Dependency Tracking verfügbar sein.
8. Externe Renderer MÜSSEN Änderungen über definierte Surface-Schnittstellen melden können.
9. Bei unvollständiger Änderungserkennung MUSS eine korrekte konservative Aktualisierung möglich sein.
10. Change Tracking MUSS unabhängig von konkreten Rendering-Backends funktionieren.

## 12. Ergebnis

Nova Graphics State Change Tracking stellt eine einheitliche und effiziente Erfassung grafischer Zustandsänderungen bereit.

Durch versionsbasierte Change Sets, Änderungszusammenfassung und die Integration mit Dependency Tracking erkennt NovaOS präzise, welche Zustände verändert wurden.

Damit bildet das System die Grundlage für gezielte Aktualisierungen, inkrementelles Rendering und eine ressourcenschonende Grafikverarbeitung.
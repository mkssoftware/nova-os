# NPSPEC-MATERIAL-CACHE-0001 – Nova Material Caching

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Caching definiert die Zwischenspeicherung und Wiederverwendung bereits berechneter Materialdaten innerhalb von NovaOS.

Ziel ist die Vermeidung redundanter Berechnungen, unnötiger Speichertransfers und wiederholter Rendering-Operationen.

Unveränderte Materialergebnisse sollen wiederverwendet werden, solange ihre Eingabedaten und Abhängigkeiten gültig bleiben.

## 2. Architektur

Material Caching ist Bestandteil der Nova Material Engine und verwendet die bestehende Ressourcenverwaltung der Nova Graphics Engine.

Die Zuständigkeiten sind:

- **Material System:** Verwaltung gemeinsam genutzter Materialdefinitionen.
- **Material Renderer:** Erzeugung und Verwendung zwischengespeicherter Ergebnisse.
- **Material Cache:** Verwaltung von Cache-Einträgen, Gültigkeit und Speicherbudgets.
- **Material Damage Tracking:** Gezielte Invalidierung betroffener Cache-Bereiche.
- **Graphics Engine:** Verwaltung der zugrunde liegenden CPU- und GPU-Ressourcen.
- **Compositor:** Bereitstellung gültiger Hintergrund- und Kompositionszustände.

Material Caching implementiert keinen eigenständigen Renderer oder Speichermanager.

## 3. Cache-Modell

Ein Cache-Eintrag beschreibt ein wiederverwendbares Materialergebnis.

Jeder Eintrag besitzt mindestens:

- **CacheKey:** Eindeutige Identifikation des Berechnungszustands.
- **Resource:** Referenz auf das zwischengespeicherte Ergebnis.
- **Dependencies:** Abhängigkeiten von Eingabedaten und Ressourcen.
- **ValidRegion:** Gültiger Bereich des Ergebnisses.
- **ResourceSize:** Benötigter Speicher.
- **LastUsed:** Zeitpunkt beziehungsweise Nutzungssequenz der letzten Verwendung.
- **State:** Gültigkeits- und Verwendungszustand.

Cache-Einträge dürfen keine unkontrollierten Kopien gemeinsam genutzter Ressourcen erzeugen.

## 4. Cache-Typen

Nova Material Caching unterstützt mindestens:

- **Definition Cache:** Aufgelöste Materialdefinitionen und Eigenschaften.
- **Effect Cache:** Berechnete Materialeffekte.
- **Backdrop Cache:** Hintergrundabhängige Zwischenergebnisse.
- **Mask Cache:** Materialmasken und geometrische Abdeckungen.
- **Layer Cache:** Ergebnisse einzelner Materialebenen.
- **Composition Cache:** Zusammengesetzte Materialergebnisse.
- **Pipeline Cache:** Wiederverwendbare Rendering-Konfigurationen.

Die konkrete Speicherung erfolgt entsprechend dem jeweiligen Ressourcentyp.

## 5. Cache-Schlüssel

Cache-Schlüssel müssen alle für das Ergebnis relevanten Eingaben berücksichtigen.

Dazu gehören bei Bedarf:

- Materialtyp und Materialdefinition
- Effektparameter
- Materialgeometrie und Transformation
- Eingaberessourcen und deren Versionen
- Hintergrundzustand und Sampling-Bereich
- Rendering-Auflösung und Skalierung
- Farbraum und Alpha-Verarbeitung
- Qualitätsstufe
- Rendering-Implementierung

Identische Schlüssel dürfen nur dann dasselbe Ergebnis referenzieren, wenn dessen Wiederverwendung tatsächlich zulässig ist.

## 6. Cache-Gültigkeit

Ein Cache-Eintrag bleibt gültig, solange sämtliche relevanten Abhängigkeiten unverändert sind.

Verbindliche Anforderungen:

- Abhängigkeiten müssen eindeutig nachvollziehbar sein.
- Änderungen relevanter Eingaben müssen betroffene Einträge invalidieren.
- Nicht relevante Änderungen dürfen keine Invalidierung auslösen.
- Teilweise gültige Ergebnisse müssen unterstützt werden.
- Ungültige Ergebnisse dürfen nicht für die Darstellung verwendet werden.
- Die Gültigkeitsprüfung muss vor der Wiederverwendung erfolgen.

## 7. Partielle Invalidierung

Material Caching muss das Nova Material Damage Tracking unterstützen.

Anforderungen:

- Änderungen einzelner Materialbereiche invalidieren ausschließlich abhängige Cache-Bereiche.
- Effektabhängige Abtastbereiche müssen berücksichtigt werden.
- Unveränderte Teilbereiche bleiben wiederverwendbar.
- Bei komplexen Abhängigkeiten darf eine vollständige Invalidierung des betroffenen Cache-Eintrags erfolgen.
- Eine Cache-Invalidierung darf nicht automatisch eine vollständige Bildschirmaktualisierung auslösen.

## 8. Hintergrundabhängiges Caching

Hintergrundabhängige Materialeffekte verwenden Nova Backdrop Sampling.

Verbindliche Anforderungen:

- Cache-Einträge müssen dem gültigen Hintergrundzustand zugeordnet sein.
- Änderungen relevanter Hintergrundbereiche invalidieren abhängige Ergebnisse.
- Änderungen außerhalb des Abtastbereichs dürfen keine Invalidierung verursachen.
- Unterschiedliche Compositor-Ebenenreihenfolgen müssen berücksichtigt werden.
- Geschützte Hintergrunddaten dürfen nicht unberechtigt wiederverwendet werden.
- Zyklische Cache- und Hintergrundabhängigkeiten sind unzulässig.

## 9. Gemeinsame Ressourcennutzung

Identische Materialressourcen sollen gemeinsam verwendet werden.

Anforderungen:

- Unveränderliche Materialdefinitionen dürfen zwischen Materialinstanzen geteilt werden.
- Identische Effektressourcen dürfen gemeinsam verwendet werden.
- Individuelle Materialzustände müssen getrennt bleiben.
- Gemeinsam genutzte Ressourcen dürfen durch einzelne Materialinstanzen nicht unbeabsichtigt verändert werden.
- Ressourcenreferenzen müssen während ihrer Verwendung gültig bleiben.

## 10. Speicherverwaltung

Material Caching muss definierte Speicherbudgets einhalten.

Verbindliche Anforderungen:

- CPU- und GPU-Cache-Speicher müssen begrenzbar sein.
- Cache-Einträge müssen nach ihrer Bedeutung und Nutzung verdrängt werden können.
- Nicht mehr benötigte Ressourcen müssen freigegeben werden.
- Ressourcen dürfen während aktiver GPU-Operationen nicht vorzeitig freigegeben werden.
- Speicherknappheit muss kontrolliert behandelt werden.
- Cache-Speicher darf nicht unbegrenzt anwachsen.

Die Verdrängungsstrategie kann Nutzungshäufigkeit, Speichergröße und Wiederherstellungskosten berücksichtigen.

## 11. GPU- und CPU-Caching

Material Caching unterstützt CPU- und GPU-basierte Ergebnisse.

Anforderungen:

- GPU-Ressourcen sollen möglichst ohne CPU-Kopien wiederverwendet werden.
- Unnötige Speichertransfers sind zu vermeiden.
- CPU-basierte Ergebnisse müssen bei Bedarf zwischengespeichert werden können.
- Die Ressourcenverwaltung erfolgt über die Nova Graphics Engine.
- Bei Verlust des GPU-Kontexts müssen betroffene Cache-Einträge ungültig werden.
- Ein Wechsel zwischen GPU- und CPU-Rendering darf keine ungültigen Ergebnisse übernehmen.

## 12. Cache-Aktualisierung

Cache-Einträge werden bei Bedarf erzeugt oder aktualisiert.

Verbindliche Anforderungen:

- Cache-Berechnungen erfolgen grundsätzlich bedarfsgesteuert.
- Mehrfache identische Berechnungsanforderungen sollen zusammengeführt werden.
- Unveränderte Ergebnisse dürfen nicht unnötig neu berechnet werden.
- Veraltete Berechnungsaufträge dürfen verworfen werden.
- Aktualisierungen müssen mit dem bestehenden Rendering-Scheduler koordiniert werden.
- Cache-Aktualisierungen dürfen keine permanenten Rendering-Schleifen erzeugen.

## 13. Adaptive Cache-Verwaltung

Die Cache-Verwaltung muss auf unterschiedliche Hardwarebedingungen reagieren können.

Berücksichtigt werden können:

- Verfügbarer Arbeitsspeicher
- Verfügbarer GPU-Speicher
- Materialkomplexität
- Wiederverwendungshäufigkeit
- Berechnungskosten
- Aktuelle Qualitätsstufe
- Energiezustand

Bei Ressourcenknappheit dürfen weniger wichtige Cache-Einträge verdrängt und aufwendige Zwischenspeicherungen reduziert werden.

Die funktionale Materialdarstellung muss erhalten bleiben.

## 14. Performance-Monitoring

Material Caching stellt Diagnoseinformationen für die bestehenden NovaOS-Diagnosesysteme bereit.

Mindestens erfassbar sind:

- Anzahl der Cache-Einträge
- CPU- und GPU-Speicherverbrauch
- Cache-Treffer
- Cache-Fehlzugriffe
- Invalidierungen
- Verdrängungen
- Wiederverwendete Rendering-Ergebnisse

Die Diagnoseerfassung muss ressourcenschonend erfolgen und darf keine unnötigen Rendering-Aktualisierungen auslösen.

## 15. Determinismus und Sicherheit

Die Cache-Verwaltung muss reproduzierbare Materialergebnisse ermöglichen.

Verbindliche Anforderungen:

- Cache-Schlüssel und Gültigkeitsregeln müssen deterministisch sein.
- Cache-Treffer dürfen das visuelle Ergebnis nicht unzulässig verändern.
- Sicherheitsgrenzen zwischen Prozessen und Oberflächen müssen eingehalten werden.
- Geschützte Ressourcen dürfen nicht unberechtigt gemeinsam verwendet werden.
- Speicherzugriffe müssen auf gültige Ressourcen begrenzt sein.
- Fehlerhafte Cache-Einträge müssen verworfen werden können.
- Bei Cache-Fehlern muss eine reguläre Neuberechnung oder ein definierter Material-Fallback erfolgen.
- Ein deterministischer Betriebsmodus muss reproduzierbare Cache-Entscheidungen ermöglichen, soweit diese das beobachtbare Verhalten beeinflussen.

## 16. Ergebnis

Nova Material Caching stellt eine einheitliche Grundlage zur Wiederverwendung berechneter Materialdaten innerhalb der Nova Material Engine bereit.

Durch eindeutige Cache-Schlüssel, gezielte Invalidierung, gemeinsame Ressourcennutzung und kontrollierte Speicherverwaltung werden redundante Berechnungen vermieden.

Das Ergebnis ist eine ressourcenschonende und reaktionsschnelle Materialdarstellung, die vollständig mit Damage Tracking, GPU-Beschleunigung und adaptiver Materialqualität zusammenarbeitet.
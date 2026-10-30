# NPSPEC-MATERIAL-PERF-0001 – Nova Material Performance Architecture

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Material Performance Architecture definiert die grundlegenden Verfahren zur effizienten Verarbeitung visueller Materialien innerhalb von NovaOS.

Ziel ist eine hochwertige Materialdarstellung bei minimalem Ressourcenverbrauch, geringer Rendering-Latenz und hoher Energieeffizienz.

Die Material Engine soll ausschließlich tatsächlich notwendige Berechnungen ausführen und vorhandene Rendering-Ergebnisse möglichst wiederverwenden.

## 2. Architektur

Die Performance-Architektur ist Bestandteil der Nova Material Engine und verwendet die bestehenden Grafiksysteme.

Die Zuständigkeiten sind:

- **Material System:** Verwaltung und Wiederverwendung von Materialdefinitionen.
- **Material Renderer:** Optimierung der Materialverarbeitung.
- **Graphics Engine:** Hardwarebeschleunigung und effiziente Rendering-Operationen.
- **Compositor:** Sichtbarkeitsprüfung, Ebenenkomposition und Damage Tracking.
- **Material Cache:** Wiederverwendung berechneter Materialergebnisse.
- **Quality Management:** Anpassung der Materialqualität an verfügbare Ressourcen.

Die Performance-Architektur implementiert keinen eigenständigen Renderer oder Scheduler.

## 3. Grundprinzipien

Die Materialverarbeitung folgt diesen Grundsätzen:

- **On-Demand Rendering:** Berechnung ausschließlich bei tatsächlichen Änderungen.
- **Partial Rendering:** Aktualisierung ausschließlich betroffener Bereiche.
- **Resource Sharing:** Gemeinsame Nutzung identischer Ressourcen.
- **Zero-Copy:** Vermeidung unnötiger Datenkopien.
- **Caching:** Wiederverwendung gültiger Zwischenergebnisse.
- **GPU Acceleration:** Bevorzugte hardwarebeschleunigte Verarbeitung.
- **Adaptive Quality:** Dynamische Anpassung der Effektqualität.
- **Lazy Evaluation:** Berechnung von Effekten erst bei tatsächlichem Bedarf.

Eine permanente Neuberechnung unveränderter Materialien ist unzulässig.

## 4. Rendering-Optimierung

Die Material Rendering Pipeline muss unnötige Rendering-Operationen vermeiden.

Verbindliche Anforderungen:

- Kompatible Materialeffekte sollen in gemeinsamen Rendering-Durchläufen verarbeitet werden.
- Unveränderte Materialparameter dürfen keine erneute Effektberechnung auslösen.
- Nicht benötigte Effekte werden übersprungen.
- Vollständig transparente oder verdeckte Materialbereiche werden nicht unnötig verarbeitet.
- Statische Effektergebnisse dürfen wiederverwendet werden.
- Temporäre Rendering-Buffer werden nach Möglichkeit wiederverwendet.
- CPU-GPU-Synchronisationen werden auf notwendige Fälle begrenzt.

## 5. Damage Tracking

Die Material Engine muss das bestehende Damage Tracking des Nova Compositors verwenden.

Verbindliche Anforderungen:

- Materialänderungen erzeugen gezielte Damage-Regionen.
- Effektabhängige Abtastbereiche werden berücksichtigt.
- Änderungen des Hintergrunds invalidieren ausschließlich tatsächlich abhängige Materialbereiche.
- Bewegte oder verformte Materialien berücksichtigen bisherige und neue Wirkungsbereiche.
- Unveränderte Bereiche werden nicht erneut gerendert.
- Abgeschlossene Animationen dürfen keine weiteren Bildaktualisierungen auslösen.

Damage Tracking muss auch bei verschachtelten und mehrschichtigen Materialien korrekt funktionieren.

## 6. Sichtbarkeitsoptimierung

Die Material Engine muss die Sichtbarkeitsinformationen des Compositors berücksichtigen.

Anforderungen:

- Vollständig verdeckte Materialbereiche werden nicht unnötig gerendert.
- Nicht sichtbare Materialinstanzen dürfen ihre aktiven Rendering-Operationen aussetzen.
- Sichtbarkeitsänderungen müssen betroffene Materialbereiche aktualisieren.
- Hintergrundabhängige Materialien müssen ihre tatsächlichen Abhängigkeiten berücksichtigen.
- Dekorative Animationen dürfen bei fehlender Sichtbarkeit reduziert oder ausgesetzt werden.

Die logische Zustandsverwaltung bleibt unabhängig von der Sichtbarkeit erhalten.

## 7. Material Caching

Berechnete Materialergebnisse dürfen zwischengespeichert werden.

Unterstützt werden:

- Statische Unschärfeergebnisse
- Hintergrundabhängige Effektergebnisse
- Reflexions- und Beleuchtungsdaten
- Materialmasken
- Farbverläufe
- Oberflächennormalen
- Verformungsdaten
- Zusammengesetzte Materialebenen

Verbindliche Anforderungen:

- Cache-Einträge müssen eindeutig ihrem Berechnungszustand zugeordnet sein.
- Ungültige Ergebnisse dürfen nicht wiederverwendet werden.
- Hintergrundabhängige Cache-Einträge müssen Änderungen ihrer Eingabebereiche berücksichtigen.
- Cache-Speicher muss begrenzbar sein.
- Nicht mehr benötigte Einträge müssen freigegeben werden können.

## 8. Ressourcenverwaltung

Die Material Engine muss Speicher und Rendering-Ressourcen effizient verwalten.

Anforderungen:

- Gemeinsame Materialdefinitionen werden nicht unnötig dupliziert.
- Materialinstanzen speichern ausschließlich notwendige individuelle Zustände.
- Identische Shader und Effektressourcen können gemeinsam verwendet werden.
- Temporäre GPU-Buffer sollen aus wiederverwendbaren Ressourcenpools stammen.
- Ressourcen müssen nach ihrer letzten Verwendung freigegeben werden können.
- Speicherverbrauch und Ressourcenanzahl müssen begrenzbar sein.
- Speicherknappheit muss kontrollierte Qualitätsreduzierungen ermöglichen.

## 9. GPU-Beschleunigung

Die Material Engine bevorzugt die Verarbeitung auf der GPU.

Verbindliche Anforderungen:

- Materialeffekte müssen über die bestehende Graphics Engine ausgeführt werden.
- Kompatible Effektoperationen sollen zusammengefasst werden.
- Wiederholte Shader-Wechsel sollen minimiert werden.
- Unnötige GPU-Speichertransfers müssen vermieden werden.
- Asynchrone GPU-Verarbeitung soll genutzt werden, sofern verfügbar.
- Die Material Engine darf keine direkte Abhängigkeit von einer bestimmten GPU-API besitzen.

Die konkrete Hardwareansteuerung verbleibt bei der Graphics Engine.

## 10. CPU-Verarbeitung

Grundlegende Materialdarstellungen müssen auch ohne GPU-Beschleunigung möglich sein.

Anforderungen:

- Einfache Materialeffekte müssen CPU-basiert verarbeitet werden können.
- CPU-Fallbacks müssen dieselben grundlegenden Materialeigenschaften berücksichtigen.
- Aufwendige Effekte dürfen vereinfacht oder deaktiviert werden.
- Die CPU-Verarbeitung muss mit Damage Tracking kompatibel sein.
- Rendering-Aufgaben dürfen die funktionale Eingabeverarbeitung nicht unnötig blockieren.

## 11. Adaptive Qualität

Die Material Engine unterstützt eine automatische Anpassung der Darstellungsqualität.

Qualitätsstufen:

1. **Full:** Vollständige Materialeffekte mit maximaler verfügbarer Qualität.
2. **Balanced:** Optimierte Effekte bei reduziertem Ressourcenverbrauch.
3. **Reduced:** Vereinfachte Effekte und reduzierte Berechnungsauflösung.
4. **Fallback:** Grundlegende Materialdarstellung ohne aufwendige Effekte.

Die Auswahl kann berücksichtigen:

- GPU-Leistung
- CPU-Auslastung
- Verfügbaren Speicher
- Energiezustand
- Rendering-Latenz
- Benutzerpräferenzen
- Barrierefreiheitseinstellungen

Qualitätswechsel dürfen keine Neuerstellung von UI-Elementen erfordern.

## 12. Frame-Budget

Die Materialverarbeitung muss das verfügbare Rendering-Zeitbudget berücksichtigen.

Verbindliche Anforderungen:

- Die Material Engine verwendet die Zeitvorgaben des bestehenden Rendering- und Compositor-Systems.
- Dekorative Effekte dürfen funktionale UI-Aktualisierungen nicht dauerhaft verzögern.
- Bei Zeitüberschreitungen können Effektqualität und Berechnungskomplexität reduziert werden.
- Veraltete Animationszwischenstände dürfen übersprungen werden.
- Die logische Animationszeit bleibt unabhängig von der tatsächlichen Bildwiederholrate.
- Qualitätsanpassungen müssen über geeignete Schwellwerte und Hysterese stabilisiert werden.

## 13. Energieeffizienz

Die Material Engine muss energieeffizientes Rendering unterstützen.

Anforderungen:

- Unveränderte Materialien verursachen keine periodischen Rendering-Operationen.
- Dekorative Animationen dürfen bei Bedarf reduziert werden.
- Hintergrundeffekte werden nur bei relevanten Änderungen aktualisiert.
- GPU- und CPU-Ressourcen sollen bei Inaktivität freigegeben oder in energiesparende Zustände versetzt werden können.
- Systemweite Energiesparvorgaben müssen berücksichtigt werden.
- Benutzer können die Materialqualität unabhängig von automatischen Optimierungen begrenzen.

## 14. Performance-Monitoring

Die Material Engine muss grundlegende Leistungsinformationen für die bestehenden NovaOS-Diagnosesysteme bereitstellen.

Mindestens erfassbar sind:

- Material-Rendering-Zeit
- Anzahl aktiver Materialinstanzen
- Anzahl ausgeführter Effektoperationen
- Cache-Treffer und Cache-Fehlzugriffe
- Speicherverbrauch der Materialressourcen
- Aktuelle Qualitätsstufe
- Anzahl und Umfang aktualisierter Materialbereiche

Die Erfassung muss abschaltbar oder auf ein ressourcenschonendes Minimum reduzierbar sein.

Performance-Monitoring darf keine unnötigen Bildschirmaktualisierungen auslösen.

## 15. Barrierefreiheit

Performance-Optimierungen müssen mit systemweiten Barrierefreiheitseinstellungen kompatibel sein.

Anforderungen:

- Kontrast und Lesbarkeit besitzen Vorrang vor dekorativer Qualität.
- Reduzierte Animationen und Transparenz müssen berücksichtigt werden.
- Qualitätsreduzierungen dürfen funktionale UI-Zustände nicht unkenntlich machen.
- Fokus- und Interaktionsrückmeldungen müssen erhalten bleiben.
- Einfache, vollständig bedienbare Materialdarstellungen müssen jederzeit verfügbar sein.

## 16. Determinismus und Sicherheit

Die Performance-Architektur muss reproduzierbare Materialergebnisse innerhalb definierter numerischer Toleranzen ermöglichen.

Verbindliche Anforderungen:

- Optimierungen dürfen die funktionale Bedeutung von UI-Zuständen nicht verändern.
- Cache-Einträge müssen gegen veraltete oder unberechtigte Datenverwendung geschützt sein.
- Ressourcen dürfen nicht über Sicherheitsgrenzen hinweg unkontrolliert gemeinsam verwendet werden.
- Speicher- und Berechnungsbudgets müssen begrenzbar sein.
- Fehlerhafte Optimierungen dürfen die Rendering-Pipeline nicht destabilisieren.
- Bei Ressourcenmangel muss ein definierter Fallback aktiviert werden.
- Ein deterministischer Betriebsmodus muss automatische, laufzeitabhängige Qualitätsentscheidungen kontrollieren oder deaktivieren können.

## 17. Ergebnis

Nova Material Performance Architecture definiert die gemeinsame Optimierungsgrundlage der Nova Material Engine.

Durch On-Demand Rendering, Damage Tracking, Caching, Ressourcenwiederverwendung, GPU-Beschleunigung und adaptive Qualitätsstufen werden aufwendige Materialeffekte effizient verarbeitet.

Das Ergebnis ist eine leistungsfähige, energieeffiziente und skalierbare Materialdarstellung, die auch auf schwächerer Hardware eine reaktionsschnelle NovaOS-Oberfläche ermöglicht.
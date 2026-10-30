# NPSPEC-MATERIAL-QUALITY-0001 – Nova Adaptive Material Quality

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Adaptive Material Quality definiert die automatische Anpassung der Materialdarstellungsqualität an die verfügbaren Systemressourcen.

Ziel ist eine gleichbleibend reaktionsschnelle, energieeffiziente und visuell hochwertige NovaOS-Oberfläche.

Die Materialqualität soll dynamisch angepasst werden können, ohne die Funktionalität oder Bedienbarkeit der Benutzeroberfläche einzuschränken.

## 2. Architektur

Adaptive Material Quality ist Bestandteil der Nova Material Engine und verwendet die bestehenden Leistungs- und Ressourceninformationen von NovaOS.

Die Zuständigkeiten sind:

- **Material System:** Bereitstellung der Materialeigenschaften und Qualitätsanforderungen.
- **Quality Management:** Auswahl und Anpassung der Materialqualität.
- **Material Renderer:** Umsetzung der gewählten Qualitätsstufe.
- **Graphics Engine:** Bereitstellung hardwareabhängiger Rendering-Fähigkeiten.
- **Compositor:** Bereitstellung von Frame-Budget und Rendering-Informationen.
- **Performance Monitoring:** Erfassung relevanter Leistungsdaten.

Es wird kein eigenständiger Rendering-Scheduler implementiert.

## 3. Qualitätsstufen

Nova Adaptive Material Quality unterstützt vier Qualitätsstufen:

### Full

Vollständige Materialdarstellung mit sämtlichen unterstützten Effekten.

- Maximale verfügbare Effektqualität
- Vollständige Hintergrundabtastung
- Hochwertige Reflexionen und Lichtbrechungen
- Volle Materialauflösung
- Vollständige Materialanimationen

### Balanced

Optimierte Materialdarstellung mit reduziertem Ressourcenverbrauch.

- Optimierte Effektberechnungen
- Angepasste Sampling-Auflösung
- Vereinfachte Reflexionen
- Wiederverwendung von Zwischenergebnissen
- Erhalt der wesentlichen visuellen Eigenschaften

### Reduced

Deutlich vereinfachte Materialdarstellung.

- Reduzierte Effektauflösung
- Vereinfachte Unschärfe
- Begrenzte Reflexionen und Lichtbrechungen
- Reduzierte dekorative Animationen
- Bevorzugung kostengünstiger Materialeffekte

### Fallback

Grundlegende Materialdarstellung ohne aufwendige Effekte.

- Solid-Materialien oder einfache Farbüberlagerungen
- Keine aufwendige Hintergrundabtastung
- Keine komplexen Reflexionen oder Verzerrungen
- Minimale Rendering-Anforderungen
- Vollständig erhaltene UI-Funktionalität

## 4. Qualitätsparameter

Die Qualitätsanpassung kann folgende Parameter beeinflussen:

- Sampling-Auflösung
- Blur-Radius und Filterqualität
- Reflexionsqualität
- Lichtbrechungsqualität
- Beleuchtungsberechnung
- Glanzlichtqualität
- Verzerrungsauflösung
- Anzahl dekorativer Materialebenen
- Effektaktualisierungsrate
- Animationsqualität
- Cache-Nutzung

Die zulässigen Anpassungen werden durch den jeweiligen Materialeffekt definiert.

## 5. Automatische Qualitätsauswahl

Die Qualitätsstufe kann anhand folgender Informationen bestimmt werden:

- GPU-Leistung
- CPU-Auslastung
- Verfügbarer Arbeitsspeicher
- Verfügbarer GPU-Speicher
- Rendering-Latenz
- Frame-Budget
- Energiezustand
- Anzahl sichtbarer Materialinstanzen
- Komplexität aktiver Materialeffekte

Verbindliche Anforderungen:

- Die Qualitätsauswahl muss ressourcenschonend erfolgen.
- Kurzzeitige Lastspitzen dürfen keine unnötigen Qualitätswechsel auslösen.
- Die Anpassung muss auf tatsächlichen Leistungsinformationen basieren.
- Die Qualität darf bei ausreichenden Ressourcen wieder erhöht werden.
- Die Benutzeroberfläche muss während der Anpassung bedienbar bleiben.

## 6. Qualitätsprioritäten

Nicht alle Materialinstanzen besitzen dieselbe Priorität.

Die Qualitätsverwaltung kann berücksichtigen:

- Sichtbarkeit
- Aktiver UI-Zustand
- Vordergrund- oder Hintergrunddarstellung
- Materialkomplexität
- Funktionale Bedeutung
- Aktuelle Benutzerinteraktion

Verbindliche Anforderungen:

- Funktionale UI-Elemente besitzen Vorrang vor dekorativen Effekten.
- Nicht sichtbare Materialien dürfen keine unnötigen Rendering-Ressourcen beanspruchen.
- Hintergrundmaterialien können stärker vereinfacht werden.
- Qualitätsreduzierungen dürfen keine funktionalen Informationen entfernen.
- Prioritäten müssen deterministisch auflösbar sein.

## 7. Dynamische Qualitätsanpassung

Die Materialqualität muss zur Laufzeit verändert werden können.

Anforderungen:

- Qualitätswechsel dürfen keine Neuerstellung von UI-Elementen erfordern.
- Materialdefinitionen bleiben unverändert.
- Die Anpassung erfolgt über effektive Rendering-Parameter der Materialinstanzen.
- Nicht betroffene Materialeigenschaften bleiben erhalten.
- Änderungen müssen mit dem Material Damage Tracking kompatibel sein.
- Ungültige Cache-Ergebnisse müssen gezielt invalidiert werden.
- Qualitätswechsel dürfen keine dauerhaften Rendering-Schleifen verursachen.

## 8. Stabilisierung

Automatische Qualitätswechsel müssen gegen häufiges Umschalten geschützt werden.

Verbindliche Anforderungen:

- Qualitätsentscheidungen verwenden definierte Schwellwerte.
- Hysterese muss unterstützt werden.
- Qualitätsverbesserungen dürfen verzögert erfolgen.
- Qualitätsreduzierungen können bei kritischer Ressourcenknappheit unmittelbar erfolgen.
- Wiederholte Wechsel zwischen zwei Qualitätsstufen müssen vermieden werden.
- Die Stabilisierung darf die Reaktion auf kritische Systemzustände nicht verhindern.

## 9. Frame-Budget

Adaptive Material Quality muss das Frame-Budget des Nova Compositors berücksichtigen.

Anforderungen:

- Die Material Engine darf das verfügbare Rendering-Zeitbudget nicht dauerhaft überschreiten.
- Dekorative Effekte müssen bei Ressourcenknappheit reduziert werden können.
- Veraltete Animationszwischenstände dürfen übersprungen werden.
- Die logische Animationszeit bleibt unabhängig von der tatsächlichen Rendering-Frequenz.
- Qualitätsanpassungen dürfen die funktionale Eingabeverarbeitung nicht blockieren.

## 10. Energieeffizienz

Die Qualitätsverwaltung muss die Energieeinstellungen von NovaOS berücksichtigen.

Verbindliche Anforderungen:

- Energiesparmodi können niedrigere Qualitätsstufen vorgeben.
- Aufwendige Materialeffekte dürfen bei geringem Energiebudget reduziert werden.
- Unveränderte Materialien dürfen keine periodischen Neuberechnungen verursachen.
- Bei Netzbetrieb kann eine höhere Qualität zugelassen werden.
- Benutzerdefinierte Energie- und Qualitätsvorgaben müssen berücksichtigt werden.

## 11. Benutzersteuerung

Der Benutzer behält die Kontrolle über die Materialqualität.

Unterstützt werden:

- Automatische Qualitätsanpassung
- Bevorzugte Qualitätsstufe
- Maximale Qualitätsstufe
- Reduzierte Materialeffekte
- Deaktivierung aufwendiger Materialeffekte

Verbindliche Anforderungen:

- Automatische Anpassungen müssen abschaltbar sein.
- Benutzerdefinierte Qualitätsgrenzen müssen eingehalten werden.
- Systemweite Einstellungen gelten als Standard für Materialinstanzen.
- Anwendungen dürfen systemweite Sicherheits- und Barrierefreiheitsvorgaben nicht überschreiben.
- Kritische Ressourcenknappheit darf einen sicheren Fallback erzwingen.

## 12. Material- und Effektintegration

Jeder Materialeffekt muss seine unterstützten Qualitätsvarianten bereitstellen.

Anforderungen:

- Effekte müssen ihre Qualitätsparameter deklarieren.
- Nicht unterstützte Qualitätsstufen müssen kontrolliert behandelt werden.
- Die Material Engine muss kompatible Effektvarianten auswählen können.
- Qualitätsreduzierungen sollen den visuellen Charakter eines Materials möglichst erhalten.
- Material Styles und Templates müssen mit sämtlichen Qualitätsstufen kompatibel bleiben.
- Die Qualitätsverwaltung darf keine eigenen Implementierungen bestehender Materialeffekte enthalten.

## 13. Damage Tracking und Caching

Adaptive Material Quality muss mit Nova Material Damage Tracking und Nova Material Caching zusammenarbeiten.

Verbindliche Anforderungen:

- Qualitätsänderungen invalidieren ausschließlich betroffene Materialbereiche.
- Unveränderte Cache-Ergebnisse dürfen wiederverwendet werden.
- Cache-Einträge müssen ihre zugrunde liegende Qualitätskonfiguration berücksichtigen.
- Unterschiedliche Qualitätsstufen dürfen keine inkompatiblen Ergebnisse gemeinsam verwenden.
- Änderungen der Sampling-Auflösung müssen korrekt berücksichtigt werden.
- Qualitätsanpassungen dürfen keine vollständige Bildschirmaktualisierung erzwingen, sofern diese nicht tatsächlich erforderlich ist.

## 14. Barrierefreiheit

Barrierefreiheit besitzt Vorrang vor dekorativer Materialqualität.

Anforderungen:

- Hohe Kontrasteinstellungen müssen unabhängig von der Qualitätsstufe erhalten bleiben.
- Reduzierte Bewegung muss berücksichtigt werden.
- Reduzierte Transparenz muss unterstützt werden.
- Fokus- und Interaktionszustände müssen eindeutig erkennbar bleiben.
- Texte und Bedienelemente müssen lesbar bleiben.
- Qualitätsanpassungen dürfen die Bedienbarkeit nicht beeinträchtigen.

## 15. Determinismus und Sicherheit

Die Qualitätsverwaltung muss kontrollierte und reproduzierbare Entscheidungen ermöglichen.

Verbindliche Anforderungen:

- Qualitätsparameter und Grenzwerte müssen validiert werden.
- Ungültige Qualitätskonfigurationen müssen auf definierte Standardwerte zurückfallen.
- Ressourcenbudgets dürfen nicht unbegrenzt überschritten werden.
- Qualitätswechsel dürfen keine ungültigen Rendering-Ressourcen verwenden.
- Fehlerhafte Effektvarianten dürfen die Rendering-Pipeline nicht destabilisieren.
- Ein deterministischer Betriebsmodus muss automatische lastabhängige Qualitätswechsel deaktivieren oder anhand reproduzierbarer Vorgaben steuern können.
- Bei nicht verfügbarer Effektimplementierung muss ein definierter Fallback verwendet werden.

## 16. Ergebnis

Nova Adaptive Material Quality ermöglicht eine dynamische Anpassung der Materialdarstellung an Hardwareleistung, Systemauslastung, Energiezustand und Benutzerpräferenzen.

Durch abgestufte Effektqualität, stabile Qualitätsentscheidungen und die Integration in Damage Tracking und Caching bleibt die NovaOS-Oberfläche auch bei begrenzten Ressourcen reaktionsschnell.

Die Material Engine erhält damit eine skalierbare Qualitätsverwaltung, die visuelle Darstellung und Ressourcenverbrauch ausbalanciert, ohne die Funktionalität der Benutzeroberfläche einzuschränken.
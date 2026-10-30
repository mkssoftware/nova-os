# NPSPEC-MATERIAL-CUSTOM-0001 – Nova Custom Materials

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Material Engine

## 1. Zweck

Nova Custom Materials definiert die Erstellung, Registrierung und Verwendung benutzerdefinierter Materialien innerhalb von NovaOS.

Das System ermöglicht die Kombination bestehender Materialtypen, Effekte und Eigenschaften zu individuellen Materialdefinitionen.

Ziel ist eine erweiterbare, deklarative und sichere Materialgestaltung ohne Änderungen an der bestehenden Material Rendering Pipeline.

## 2. Architektur

Custom Materials ist Bestandteil des Nova Material Systems und verwendet die bestehenden Komponenten der Nova Material Engine.

Die Zuständigkeiten sind:

- **Material System:** Registrierung und Verwaltung benutzerdefinierter Materialien.
- **Material Properties:** Definition und Validierung individueller Eigenschaften.
- **Material Layer Composition:** Zusammensetzung mehrschichtiger Materialien.
- **Material Renderer:** Verarbeitung der Materialdefinitionen.
- **Graphics Engine:** Ausführung der Rendering-Operationen.
- **Nova UI System:** Deklarative Verwendung und Designer-Integration.

Custom Materials implementiert keinen eigenständigen Renderer.

## 3. Materialmodell

Ein benutzerdefiniertes Material besitzt mindestens:

- **MaterialId:** Eindeutige Materialkennung.
- **Name:** Materialbezeichnung.
- **Version:** Version der Materialdefinition.
- **BaseMaterial:** Optionaler Basismaterialtyp.
- **Properties:** Registrierte Materialeigenschaften.
- **Layers:** Optionale Materialebenen.
- **Effects:** Verwendete Materialeffekte.
- **Fallback:** Definierte Ersatzdarstellung.

Materialdefinitionen müssen deklarativ beschreibbar und eindeutig referenzierbar sein.

## 4. Materialerstellung

Benutzerdefinierte Materialien können durch folgende Verfahren erstellt werden:

- Ableitung bestehender Materialtypen
- Kombination registrierter Materialeffekte
- Zusammensetzung mehrerer Materialebenen
- Definition individueller Materialeigenschaften
- Verwendung bestehender Material Presets
- Kombination von Material Styles und Templates

Verbindliche Anforderungen:

- Bestehende Materialfunktionen müssen wiederverwendbar sein.
- Materialdefinitionen dürfen keine direkten GPU-Zugriffe enthalten.
- Materialinstanzen müssen unabhängig voneinander konfigurierbar sein.
- Gemeinsame Materialdefinitionen bleiben unveränderlich.
- Die Materialerstellung darf keine Änderungen am Compositor erfordern.

## 5. Materialvererbung

Custom Materials können bestehende Materialien erweitern.

Anforderungen:

- Ein Material kann einen registrierten Basismaterialtyp referenzieren.
- Nicht überschriebene Eigenschaften werden übernommen.
- Zusätzliche Eigenschaften und Effekte können definiert werden.
- Bestehende Eigenschaften dürfen innerhalb ihrer zulässigen Grenzen überschrieben werden.
- Die Vererbungsauflösung muss deterministisch erfolgen.
- Zyklische Materialvererbungen sind unzulässig.

## 6. Benutzerdefinierte Eigenschaften

Custom Materials können eigene Materialeigenschaften bereitstellen.

Verbindliche Anforderungen:

- Eigenschaften müssen über Nova Material Properties registriert werden.
- Jede Eigenschaft besitzt einen definierten Datentyp und Standardwert.
- Zulässige Wertebereiche müssen festgelegt werden.
- Eigenschaften können bindbar und animierbar sein.
- Änderungen müssen ihre Auswirkungen auf Rendering und Damage Tracking deklarieren.
- Ungültige Eigenschaftswerte müssen kontrolliert behandelt werden.

## 7. Effektkomposition

Custom Materials verwenden registrierte Effekte der Nova Material Engine.

Unterstützt werden unter anderem:

- Backdrop Blur
- Light Refraction
- Surface Reflections
- Dynamic Lighting
- Specular Highlights
- Surface Distortion
- Adaptive Transparency
- Material Tinting
- Fluid Deformation

Anforderungen:

- Effekte müssen in einer definierten Reihenfolge verarbeitet werden.
- Effektabhängigkeiten müssen eindeutig aufgelöst werden.
- Gemeinsame Effektressourcen dürfen wiederverwendet werden.
- Hintergrundabhängige Effekte verwenden Nova Backdrop Sampling.
- Nicht unterstützte Effektkombinationen müssen kontrolliert behandelt werden.

## 8. Materialebenen

Custom Materials können mehrere Materialebenen enthalten.

Verbindliche Anforderungen:

- Ebenen werden über Nova Material Layer Composition definiert.
- Jede Ebene kann eigene Eigenschaften, Effekte und Masken besitzen.
- Die Ebenenreihenfolge muss eindeutig sein.
- Ebenen können dynamisch aktiviert oder deaktiviert werden.
- Unveränderte Ebenenergebnisse dürfen wiederverwendet werden.
- Materialebenen dürfen keine eigenständigen Compositor-Surfaces voraussetzen.

## 9. Registrierung

Benutzerdefinierte Materialien müssen vor ihrer Verwendung registriert werden.

Anforderungen:

- Jede Materialdefinition benötigt eine eindeutige Kennung.
- Materialeigenschaften und Effektabhängigkeiten müssen validiert werden.
- Benötigte Ressourcen müssen verfügbar oder ersetzbar sein.
- Unterschiedliche Materialversionen müssen unterscheidbar sein.
- Fehlerhafte Registrierungen müssen zurückgewiesen werden.
- Nicht mehr benötigte Materialdefinitionen müssen kontrolliert freigegeben werden können.

## 10. Ressourcen und Portabilität

Custom Materials müssen über das bestehende NovaOS Resource System verwaltet werden.

Verbindliche Anforderungen:

- Materialdefinitionen müssen speicherbar und wiederherstellbar sein.
- Ressourcenreferenzen müssen eindeutig auflösbar sein.
- Benutzerdefinierte Materialien müssen zwischen kompatiblen NovaOS-Installationen übertragbar sein.
- Fehlende Ressourcen müssen kontrolliert behandelt werden.
- Materialdefinitionen dürfen keine unzulässigen externen Abhängigkeiten voraussetzen.
- Gemeinsam genutzte Ressourcen dürfen nicht unbeabsichtigt verändert werden.

## 11. UI-Integration

Custom Materials müssen mit dem nativen Nova UI-System kompatibel sein.

Anforderungen:

- Benutzerdefinierte Materialien können deklarativ referenziert werden.
- Materialeigenschaften müssen Datenbindungen unterstützen.
- Material Styles und Templates können Custom Materials verwenden.
- Theme-Ressourcen müssen referenzierbar sein.
- Zustandsabhängige Materialeigenschaften müssen unterstützt werden.
- Änderungen dürfen keine Neuerstellung des zugehörigen UI-Elements erfordern.

## 12. Designer-Integration

Der Nova UI Designer muss die Erstellung und Bearbeitung benutzerdefinierter Materialien unterstützen.

Mindestens vorgesehen sind:

- Auswahl eines Basismaterials
- Bearbeitung von Materialeigenschaften
- Hinzufügen und Entfernen von Effekten
- Konfiguration von Materialebenen
- Anpassung von Effektparametern
- Materialvorschau
- Speicherung als wiederverwendbare Materialdefinition

Die visuelle Bearbeitung muss mit der deklarativen Materialdefinition synchronisiert werden.

## 13. Animation und Interaktion

Custom Materials können dynamische Eigenschaften verwenden.

Verbindliche Anforderungen:

- Animationen verwenden das bestehende NovaOS Animation System.
- Interaktive Zustände werden über Nova UI bereitgestellt.
- Materialeffekte dürfen auf definierte Zustandsänderungen reagieren.
- Animationen dürfen gemeinsame Materialdefinitionen nicht verändern.
- Abgeschlossene Animationen dürfen keine unnötigen Rendering-Aktualisierungen verursachen.

## 14. Damage Tracking und Caching

Custom Materials müssen vollständig mit Nova Material Damage Tracking und Nova Material Caching kompatibel sein.

Anforderungen:

- Eigenschaftsänderungen müssen gezielte Damage-Regionen erzeugen.
- Effektabhängige Abtastbereiche müssen berücksichtigt werden.
- Unveränderte Zwischenergebnisse dürfen wiederverwendet werden.
- Änderungen müssen betroffene Cache-Einträge invalidieren.
- Unsichtbare Materialien dürfen keine unnötigen Rendering-Operationen auslösen.
- Eine permanente vollständige Bildschirmaktualisierung ist unzulässig.

## 15. Performance und Qualitätsstufen

Custom Materials müssen Nova Adaptive Material Quality unterstützen.

Verbindliche Anforderungen:

- Materialeffekte müssen ihre unterstützten Qualitätsstufen deklarieren.
- Die Material Engine darf aufwendige Effekte bei Ressourcenknappheit reduzieren.
- Die Anzahl und Komplexität aktiver Effekte muss begrenzbar sein.
- GPU-Beschleunigung wird bevorzugt.
- Vereinfachte CPU-basierte Darstellungen müssen möglich sein.
- Benutzerdefinierte Materialien dürfen definierte Ressourcenbudgets nicht unbegrenzt überschreiten.

## 16. Fallback

Jedes Custom Material muss eine definierte Ersatzdarstellung besitzen.

Anforderungen:

- Nicht verfügbare Effekte müssen vereinfacht oder deaktiviert werden können.
- Fehlende Hintergrundabtastung darf die grundlegende Darstellung nicht verhindern.
- Nicht unterstützte Materialtypen müssen kontrolliert ersetzt werden.
- Eine Solid-Darstellung muss als letzter Fallback verfügbar sein.
- Die funktionale Bedeutung und Bedienbarkeit der UI-Elemente muss erhalten bleiben.
- Fehlerhafte Materialdefinitionen dürfen die Rendering-Pipeline nicht destabilisieren.

## 17. Sicherheit

Benutzerdefinierte Materialien unterliegen den Sicherheitsrichtlinien von NovaOS.

Verbindliche Anforderungen:

- Materialdefinitionen müssen vor ihrer Aktivierung validiert werden.
- Ausschließlich registrierte und autorisierte Materialeffekte dürfen verwendet werden.
- Direkte Zugriffe auf fremde Surface-Buffer sind unzulässig.
- Hintergrunddaten dürfen ausschließlich über Nova Backdrop Sampling bezogen werden.
- Ressourcenverbrauch und Berechnungskomplexität müssen begrenzbar sein.
- Zyklische Effekt- und Materialabhängigkeiten müssen erkannt werden.
- Benutzerdefinierte Materialien dürfen keine Sicherheitsgrenzen der Graphics Engine oder des Compositors umgehen.

## 18. Erweiterbarkeit

Die Material Engine muss die spätere Erweiterung um neue Materialeffekte und Materialtypen ermöglichen.

Anforderungen:

- Neue Effekte müssen über definierte Schnittstellen registrierbar sein.
- Erweiterungen müssen ihre Eigenschaften, Ressourcenanforderungen und Rendering-Abhängigkeiten deklarieren.
- Neue Effektimplementierungen müssen Damage Tracking und Fallback unterstützen.
- Erweiterungen dürfen keine Änderungen an bestehenden Materialdefinitionen erzwingen.
- Ausführbarer Erweiterungscode unterliegt dem Sicherheits- und Capability-Modell von NovaOS.
- Die deklarative Materialdefinition selbst darf keinen uneingeschränkten Programmcode ausführen.

## 19. Determinismus

Die Verarbeitung benutzerdefinierter Materialien muss reproduzierbar erfolgen.

Verbindliche Anforderungen:

- Materialdefinitionen müssen eindeutig aufgelöst werden.
- Effekt- und Ebenenreihenfolgen müssen deterministisch sein.
- Identische Eingaben müssen innerhalb definierter numerischer Toleranzen identische Rendering-Ergebnisse erzeugen.
- Zufallsbasierte Effekte benötigen kontrollierbare Ausgangswerte.
- Ein deterministischer Betriebsmodus muss reproduzierbare Materialzustände ermöglichen.

## 20. Ergebnis

Nova Custom Materials ermöglicht die Erstellung individueller Materialtypen durch die Kombination bestehender Materialeigenschaften, Effekte und Ebenen.

Die deklarative Architektur gewährleistet Wiederverwendbarkeit, Erweiterbarkeit und vollständige Integration in das Nova UI-System.

Durch gemeinsame Rendering-Ressourcen, Damage Tracking, adaptive Qualität und definierte Fallbacks können benutzerdefinierte Materialien sicher und ressourcenschonend dargestellt werden.
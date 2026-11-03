# NPSPEC-SCENE-PROPERTY-0001 – Nova Scene Property System

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Das Nova Scene Property System definiert die einheitliche Verwaltung grafischer Eigenschaften innerhalb des Nova Scene Graph.

Scene Properties beschreiben den Zustand und das Verhalten grafischer Elemente unabhängig von konkreten Rendering-Operationen.

Ziel ist ein typisiertes, deklaratives und transaktionsfähiges Eigenschaftssystem, das inkrementelle Aktualisierungen und effiziente Zustandsverwaltung ermöglicht.

## 2. Architekturprinzipien

- **Strongly Typed:** Eigenschaften besitzen eindeutig definierte Datentypen.
- **Declarative:** Eigenschaften beschreiben gewünschte grafische Zustände.
- **State-Driven:** Die Darstellung basiert auf effektiven Eigenschaftswerten.
- **Transactional:** Änderungen werden atomar veröffentlicht.
- **Versioned:** Eigenschaften sind Bestandteil versionierter Szenenzustände.
- **Dependency-Aware:** Eigenschaftsabhängigkeiten werden berücksichtigt.
- **Incremental:** Nur tatsächlich veränderte Eigenschaften lösen Folgeaktualisierungen aus.
- **Extensible:** Neue Eigenschaften können über definierte Schnittstellen ergänzt werden.
- **Renderer-Independent:** Das Eigenschaftsmodell ist unabhängig vom Rendering-Backend.

## 3. Property-Modell

Jede Scene Property besitzt mindestens:

- **Property ID:** Eindeutige Eigenschaftskennung.
- **Property Type:** Definierter Datentyp.
- **Default Value:** Standardwert.
- **Validation Rules:** Zulässige Werte und Einschränkungen.
- **Property Flags:** Definierte Verhaltensmerkmale.

Optionale Metadaten umfassen:

- Vererbungsverhalten
- Änderungsrelevanz
- Abhängigkeitsinformationen
- Bindungsfähigkeit
- Animierbarkeit
- Serialisierungsregeln

Property-Definitionen MÜSSEN unabhängig von einzelnen Scene-Node-Instanzen verwaltet werden können.

## 4. Property-Typen

Das System unterstützt insbesondere:

- Boolean
- Integer
- Float
- Color
- Vector
- Matrix
- Rectangle
- String
- Enum
- Resource Reference
- Material Reference
- Structured Property

Weitere Typen DÜRFEN ergänzt werden.

Datentypen MÜSSEN eindeutig definiert und validierbar sein.

## 5. Property-Werte

Scene Properties können unterschiedliche Wertquellen besitzen.

Dazu gehören:

- Standardwerte
- Lokal gesetzte Werte
- Geerbte Werte
- Style- und Template-Werte
- Datenbindungen
- Animationswerte

Die Auflösung erfolgt anhand einer definierten Prioritätsordnung.

Das System MUSS für jede Eigenschaft einen eindeutigen effektiven Wert bestimmen können.

Die konkrete Prioritätsordnung wird durch das zuständige Property-Resolution-Modell festgelegt.

## 6. Property-Vererbung

Eigenschaften DÜRFEN entlang der Szenenhierarchie vererbt werden.

Dabei gilt:

- Nur entsprechend gekennzeichnete Eigenschaften sind vererbbar.
- Lokale Werte können geerbte Werte gemäß definierter Priorität überschreiben.
- Änderungen übergeordneter Eigenschaften können untergeordnete Elemente beeinflussen.
- Unveränderte effektive Werte dürfen keine unnötigen Aktualisierungen auslösen.

Die Vererbung MUSS deterministisch erfolgen.

## 7. Property Bindings

Scene Properties DÜRFEN an andere Eigenschaften oder externe Zustandsquellen gebunden werden.

Bindings MÜSSEN:

- Definierte Quellen besitzen.
- Typkompatibilität gewährleisten.
- Änderungen kontrolliert weitergeben.
- Abhängigkeiten registrieren.
- Ungültige Referenzen erkennen.

Zyklische Bindungsabhängigkeiten MÜSSEN erkannt und kontrolliert behandelt werden.

Die Bindungsverwaltung erfolgt über definierte Schnittstellen zur Nova UI Engine und zum Graphics Dependency Tracking.

## 8. Property-Änderungen

Eigenschaftsänderungen erfolgen über Nova Graphics State Transactions.

Die Verarbeitung umfasst:

1. Entgegennahme des neuen Wertes.
2. Typ- und Wertevalidierung.
3. Ermittlung des effektiven Zielwertes.
4. Atomare Veröffentlichung des neuen Zustands.
5. Erfassung tatsächlicher Änderungen.
6. Weitergabe relevanter Abhängigkeiten.

Fehlgeschlagene Änderungen DÜRFEN den veröffentlichten Szenenzustand nicht verändern.

## 9. Change Tracking

Das Property System ist in Nova Graphics State Change Tracking integriert.

Änderungen werden anhand des effektiven Ausgangs- und Zielzustands bewertet.

Dabei MUSS unterschieden werden zwischen:

- Änderung eines gespeicherten Property-Wertes
- Änderung der aktiven Wertquelle
- Änderung des effektiven Property-Wertes

Nur Änderungen mit relevanten Auswirkungen SOLLEN nachgelagerte Grafikoperationen auslösen.

## 10. Property-Metadaten

Properties DÜRFEN Metadaten zur Änderungsverarbeitung bereitstellen.

Dazu gehören insbesondere:

- **AffectsLayout:** Beeinflusst die Layoutberechnung.
- **AffectsGeometry:** Beeinflusst geometrische Eigenschaften.
- **AffectsRender:** Beeinflusst das Rendering-Ergebnis.
- **AffectsComposition:** Beeinflusst die Komposition.
- **AffectsDependencies:** Verändert grafische Abhängigkeiten.
- **Inheritable:** Unterstützt Eigenschaftsvererbung.
- **Animatable:** Unterstützt Animationen.

Diese Merkmale dienen der Optimierung und ersetzen keine tatsächliche Abhängigkeitsanalyse.

## 11. Caching und Optimierung

NovaOS SOLL:

- Effektive Property-Werte zwischenspeichern.
- Unveränderte Werte wiederverwenden.
- Redundante Wertauflösungen vermeiden.
- Mehrfachänderungen zusammenfassen.
- Eigenschaftsabhängigkeiten gezielt invalidieren.
- Unnötige Layout- und Rendering-Operationen verhindern.

Cache-Einträge MÜSSEN an gültige Zustandsversionen gebunden sein.

## 12. Erweiterbarkeit

Neue Scene Properties DÜRFEN über definierte Registrierungsschnittstellen ergänzt werden.

Erweiterte Eigenschaften MÜSSEN:

- Eindeutig identifizierbar sein.
- Einen gültigen Datentyp besitzen.
- Standardwerte definieren.
- Validierungsregeln bereitstellen.
- Ihr Änderungsverhalten deklarieren.

Unbekannte oder nicht unterstützte Eigenschaften MÜSSEN kontrolliert behandelt werden.

## 13. Verbindliche Anforderungen

1. Scene Properties MÜSSEN typisiert sein.
2. Jede Property MUSS eine eindeutige Definition besitzen.
3. Property-Werte MÜSSEN validierbar sein.
4. Effektive Property-Werte MÜSSEN deterministisch aufgelöst werden.
5. Property-Änderungen MÜSSEN transaktionsbasiert veröffentlicht werden.
6. Veröffentlichte Property-Zustände MÜSSEN während aktiver Snapshots unveränderlich bleiben.
7. Property-Änderungen MÜSSEN für Graphics Change Tracking verfügbar sein.
8. Property-Abhängigkeiten MÜSSEN durch Graphics Dependency Tracking berücksichtigt werden können.
9. Unveränderte effektive Werte SOLLEN keine unnötigen Grafikoperationen auslösen.
10. Das Property System MUSS unabhängig von konkreten Rendering-Backends funktionieren.
11. Neue Properties MÜSSEN über definierte Schnittstellen integrierbar sein.
12. Ungültige Eigenschaften oder Werte DÜRFEN keine inkonsistenten Szenenzustände erzeugen.

## 14. Ergebnis

Das Nova Scene Property System stellt ein einheitliches, typisiertes und deklaratives Eigenschaftsmodell für grafische Elemente bereit.

Durch effektive Wertauflösung, Vererbung, Bindungen und transaktionsbasierte Zustandsänderungen ermöglicht es eine konsistente Verwaltung komplexer grafischer Eigenschaften.

In Verbindung mit Change Tracking und Dependency Tracking bildet es die Grundlage für präzise, inkrementelle und ressourcenschonende Szenenaktualisierungen.
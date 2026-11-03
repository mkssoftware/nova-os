# NPSPEC-SCENE-NODE-0001 – Nova Scene Node Model

**Status:** Angenommen  
**Version:** 0001  
**Subsystem:** Nova Scene Graph

## 1. Zweck

Nova Scene Node Model definiert die grundlegende Struktur, Eigenschaften und Lebensdauer grafischer Elemente innerhalb des Nova Scene Graph.

Scene Nodes repräsentieren grafische und strukturelle Bestandteile einer Szene.

Ziel ist ein einheitliches, erweiterbares und ressourcenschonendes Elementmodell für deklarative Grafikverarbeitung und inkrementelles Rendering.

## 2. Architekturprinzipien

- **Identity-Based:** Jeder Scene Node besitzt eine eindeutige Identität.
- **Hierarchical:** Nodes werden in einer definierten Eltern-Kind-Hierarchie organisiert.
- **Declarative:** Nodes beschreiben grafische Zustände statt Zeichenbefehle.
- **Persistent:** Nodes bleiben zwischen Rendering-Zyklen erhalten.
- **Versioned:** Zustandsänderungen werden versionsbasiert verwaltet.
- **Composable:** Komplexe Elemente entstehen durch die Kombination einfacher Nodes.
- **Incremental:** Unveränderte Nodes werden wiederverwendet.
- **Renderer-Independent:** Nodes sind unabhängig von konkreten Rendering-Backends.

## 3. Scene Node Struktur

Jeder Scene Node besitzt mindestens:

- **Node ID:** Eindeutige Identität innerhalb des definierten Identitätsraums.
- **Node Type:** Typ des grafischen Elements.
- **Parent Reference:** Referenz auf das übergeordnete Element.
- **Child References:** Geordnete Liste untergeordneter Elemente.
- **Property Set:** Grafische Eigenschaften.
- **State Version:** Version des Node-Zustands.

Optional können weitere Informationen vorhanden sein:

- Geometrie
- Transformation
- Sichtbarkeit
- Transparenz
- Clipping
- Material
- Effekte
- Inhalt
- Ressourcenreferenzen
- Semantische Informationen

## 4. Node-Typen

NovaOS unterstützt insbesondere folgende grundlegende Node-Typen:

- **Group Node:** Gruppierung untergeordneter Elemente.
- **Transform Node:** Räumliche Transformationen.
- **Geometry Node:** Geometrische Formen und Pfade.
- **Text Node:** Textdarstellung.
- **Image Node:** Bilder und Texturen.
- **Surface Node:** Einbindung grafischer Oberflächen.
- **Clip Node:** Begrenzung sichtbarer Bereiche.
- **Material Node:** Material- und Effektzuordnung.
- **Effect Node:** Grafische Effekte.
- **Custom Node:** Erweiterbare benutzerdefinierte Darstellung.

Weitere Node-Typen DÜRFEN ergänzt werden.

Die Implementierung DARF mehrere Funktionen innerhalb eines Nodes kombinieren.

## 5. Node-Hierarchie

Scene Nodes bilden eine gerichtete, azyklische Eltern-Kind-Hierarchie.

Dabei gilt:

- Jeder Node besitzt höchstens einen Parent Node.
- Ein Node DARF mehrere Child Nodes besitzen.
- Die Reihenfolge der Child Nodes ist definiert.
- Root Nodes besitzen keinen Parent Node.
- Zyklische Eltern-Kind-Beziehungen sind unzulässig.

Die Hierarchie bestimmt strukturelle Beziehungen und die Grundlage für Transformationen, Clipping und Zeichenreihenfolge.

Zusätzliche Abhängigkeiten werden durch Graphics Dependency Tracking verwaltet.

## 6. Node-Eigenschaften

Scene Nodes besitzen typisierte grafische Eigenschaften.

Diese können umfassen:

- Position und Größe
- Transformation
- Sichtbarkeit
- Deckkraft
- Zeichenreihenfolge
- Clipping
- Materialparameter
- Inhalt
- Ressourcenreferenzen

Eigenschaften MÜSSEN definierte Typen, gültige Wertebereiche und Standardwerte besitzen.

Die Verwaltung erfolgt über das Nova Scene Property System.

## 7. Transformationen

Scene Nodes können lokale Transformationen definieren.

Die effektive Transformation ergibt sich aus der Verkettung der Transformationen entlang der Szenenhierarchie.

NovaOS MUSS:

- Lokale und effektive Transformationen unterscheiden.
- Transformationen in definierter Reihenfolge anwenden.
- Änderungen übergeordneter Transformationen berücksichtigen.
- Unveränderte Transformationsergebnisse wiederverwenden können.

Transformationen DÜRFEN die logische Node-Hierarchie nicht verändern.

## 8. Sichtbarkeit und Clipping

Jeder Scene Node kann Sichtbarkeits- und Clipping-Eigenschaften besitzen.

Die effektive Sichtbarkeit berücksichtigt:

- Eigenen Sichtbarkeitszustand
- Übergeordnete Sichtbarkeitszustände
- Clipping-Bereiche
- Transformationen
- Geometrische Begrenzungen

Nicht sichtbare Nodes SOLLEN von unnötigen Rendering-Operationen ausgeschlossen werden.

Ihre logischen Zustände MÜSSEN dennoch erhalten bleiben.

## 9. Node-Zustandsänderungen

Änderungen an Scene Nodes erfolgen über definierte Zustandsoperationen.

Dazu gehören:

- Node erstellen
- Node entfernen
- Node-Eigenschaft ändern
- Child Node hinzufügen
- Child Node entfernen
- Node innerhalb der Hierarchie verschieben
- Ressourcenreferenz ändern

Änderungen MÜSSEN über Nova Graphics State Transactions konsistent veröffentlicht werden.

Unveränderte Nodes SOLLEN zwischen Zustandsversionen wiederverwendet werden.

## 10. Node-Lebenszyklus

Ein Scene Node durchläuft folgende logische Zustände:

1. **Created:** Node wurde erstellt, ist jedoch noch nicht veröffentlicht.
2. **Attached:** Node ist Bestandteil einer veröffentlichten Szene.
3. **Detached:** Node wurde aus der veröffentlichten Szenenhierarchie entfernt.
4. **Released:** Nicht mehr benötigte Node-Ressourcen wurden freigegeben.

Ein Node DARF vor seiner Veröffentlichung verändert werden.

Entfernte Nodes MÜSSEN gültig bleiben, solange aktive Scene Snapshots sie referenzieren.

Freigegebene Node-Identitäten DÜRFEN nicht zu mehrdeutigen Referenzen führen.

## 11. Ressourcenverwaltung

Scene Nodes können externe Grafikressourcen referenzieren.

Die Ressourcenverwaltung MUSS sicherstellen:

- Gültige Ressourcenreferenzen
- Kontrollierte Ressourcenlebensdauer
- Sichere gemeinsame Ressourcennutzung
- Keine vorzeitige Ressourcenfreigabe
- Erkennung ungültiger Referenzen

Scene Nodes verwalten keine hardwareabhängigen Speicherallokationen direkt.

Diese Aufgabe übernehmen die zuständigen Ressourcenmanager.

## 12. Rendering-Integration

Scene Nodes beschreiben grafische Inhalte, führen jedoch keine unmittelbaren GPU-Befehle aus.

Die Rendering-Architektur interpretiert die veröffentlichten Node-Zustände und erzeugt daraus erforderliche Rendering-Operationen.

Dabei SOLLEN:

- Unveränderte Nodes übersprungen werden.
- Gültige Rendering-Ergebnisse wiederverwendet werden.
- Änderungen gezielt verarbeitet werden.
- Nicht sichtbare Elemente ausgeschlossen werden.
- Material- und Effektabhängigkeiten berücksichtigt werden.

Custom Nodes MÜSSEN über definierte Rendering-Schnittstellen integriert werden.

## 13. Erweiterbarkeit und Isolation

Neue Node-Typen DÜRFEN über definierte Erweiterungsschnittstellen bereitgestellt werden.

Erweiterungen MÜSSEN:

- Das Scene Node Model einhalten.
- Gültige Eigenschaften bereitstellen.
- Zustandsänderungen korrekt melden.
- Ressourcen sicher referenzieren.
- Die Szenenhierarchie konsistent halten.

Anwendungen DÜRFEN keine unberechtigten Änderungen an fremden Scene Nodes durchführen.

## 14. Verbindliche Anforderungen

1. Jeder Scene Node MUSS eine eindeutige Identität besitzen.
2. Scene Nodes MÜSSEN einer definierten Szenenhierarchie zugeordnet werden können.
3. Die Eltern-Kind-Hierarchie MUSS azyklisch bleiben.
4. Node-Eigenschaften MÜSSEN typisiert und validierbar sein.
5. Zustandsänderungen MÜSSEN transaktionsbasiert veröffentlicht werden.
6. Veröffentlichte Node-Zustände MÜSSEN während aktiver Snapshots unveränderlich bleiben.
7. Unveränderte Nodes SOLLEN zwischen Zustandsversionen wiederverwendet werden.
8. Node-Änderungen MÜSSEN für Graphics Change Tracking verfügbar sein.
9. Ressourcenreferenzen MÜSSEN während ihrer Verwendung gültig bleiben.
10. Das Node-Modell MUSS unabhängig von konkreten Rendering-Backends funktionieren.
11. Neue Node-Typen MÜSSEN über definierte Schnittstellen integrierbar sein.
12. Node-Zugriffe MÜSSEN durch geeignete Berechtigungen und Isolation geschützt werden.

## 15. Ergebnis

Nova Scene Node Model definiert ein einheitliches, hierarchisches und erweiterbares Modell grafischer Elemente.

Durch persistente Identitäten, typisierte Eigenschaften, transaktionsbasierte Zustandsänderungen und kontrollierte Ressourcenverwaltung bildet es die Grundlage für den Nova Scene Graph.

Damit ermöglicht NovaOS eine effiziente, deterministische und inkrementelle Verarbeitung komplexer grafischer Szenen.
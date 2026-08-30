# ADR-FILESYSTEM-0001 – Physische und logische Struktur trennen

**Status:** Angenommen  
**Kategorie:** Filesystem / Namespace Architecture  
**Entscheidungstyp:** Architekturentscheidung  
**Gültigkeit:** Systemweit  
**Version:** 1.0  
**Datum:** 2026-08-28  

---

## 1. Kontext

Klassische Betriebssysteme bilden die für Benutzer und Anwendungen sichtbare Dateisystemstruktur weitgehend direkt auf die physische beziehungsweise dateisysteminterne Verzeichnisstruktur ab.

Typische Beispiele sind:

~~~text
/home/user/Documents/
/usr/lib/
/Program Files/
/Users/User/Documents/
~~~

Ein sichtbarer Pfad beschreibt dabei üblicherweise gleichzeitig:

- die hierarchische Organisation,
- den Namensraum,
- den Speicherort innerhalb eines Dateisystems,
- teilweise die Identität eines Objekts,
- die Zugriffsmethode auf das Objekt.

Diese enge Kopplung führt zu mehreren Einschränkungen.

Ein Objekt kann nur schwer gleichzeitig in mehreren semantisch sinnvollen Zusammenhängen erscheinen, ohne zusätzliche Mechanismen wie:

- Kopien,
- symbolische Links,
- Hardlinks,
- virtuelle Dateisysteme,
- anwendungsspezifische Datenbanken.

Außerdem werden Anwendungen dadurch häufig vom konkreten Speicherort eines Objekts abhängig.

NovaOS verfolgt dagegen ein objekt-, semantik-, namespace- und capability-orientiertes Systemmodell.

Daher werden die physische Speicherung eines Objekts und dessen logische Darstellung im Namespace grundsätzlich voneinander getrennt.

---

## 2. Entscheidung

NovaOS definiert:

> Die physische Speicherstruktur eines Objekts und seine logische Darstellung im Namespace sind voneinander unabhängige Konzepte.

Die sichtbare Datei- und Verzeichnisstruktur MUSS daher nicht der physischen Ablagestruktur entsprechen.

Die logische Struktur wird durch Namespace-Projektionen erzeugt.

Vereinfacht gilt:

~~~text
Physischer Speicher
        |
        v
      Objekt
        |
        v
 Objektidentität
        |
        +-------------------+
        |                   |
        v                   v
    Metadaten           Relationen
        |                   |
        +---------+---------+
                  |
                  v
          Semantische Typen
                  |
                  v
        Namespace-Projektion
                  |
                  v
         Logische Struktur
                  |
                  v
      Benutzer / Anwendung
~~~

Ein Objekt kann dadurch unabhängig von seinem physischen Speicherort in einem oder mehreren logischen Namespaces erscheinen.

---

## 3. Ziele

Die Trennung von physischer und logischer Struktur verfolgt insbesondere folgende Ziele:

1. Location Transparency
2. semantische Organisation von Daten
3. mehrere logische Sichten auf dasselbe Objekt
4. Vermeidung unnötiger Datenkopien
5. stabile Objektidentitäten unabhängig vom Pfad
6. flexible Benutzeransichten
7. systemweite globale Suche
8. dynamische Namespace-Projektionen
9. einheitliche Behandlung lokaler und entfernter Daten
10. Unterstützung von Arbeitsbereichen
11. Unterstützung von Solutions
12. Unterstützung des Capability-Modells
13. transaktionale Namespace-Operationen
14. bessere Self-Healing-Fähigkeiten
15. langfristige Unabhängigkeit von konkreten Storage-Technologien

---

## 4. Grundmodell

NovaOS unterscheidet mindestens zwischen:

~~~text
Physical Storage
Object
Identity
Metadata
Relations
Semantic Type
Namespace
Projection
Location
View
~~~

Diese Konzepte DÜRFEN NICHT implizit als dasselbe behandelt werden.

Insbesondere gilt:

~~~text
Path != Object Identity
Path != Physical Location
Namespace != Physical Filesystem Layout
View != Storage Layout
~~~

Ein logischer Pfad ist eine Referenz beziehungsweise Projektion innerhalb eines Namespace und nicht notwendigerweise die physische Adresse des zugrunde liegenden Objekts.

---

## 5. Physische Struktur

Die physische Struktur beschreibt, wie NovaOS Daten tatsächlich auf einem Speichermedium oder innerhalb eines Storage-Backends organisiert.

Diese Struktur ist eine Implementierungsentscheidung der jeweiligen Storage- und Filesystem-Komponenten.

Beispielsweise könnte eine Implementierung intern folgende Struktur verwenden:

~~~text
/ObjectStore/
├── 01/
│   └── ...
├── 3A/
│   └── ...
├── A7/
│   └── ...
└── FC/
    └── ...
~~~

Dies bedeutet NICHT, dass Benutzer oder Anwendungen diese Struktur sehen.

Eine zukünftige Implementierung könnte stattdessen verwenden:

- Bäume
- Hash-basierte Object Stores
- Content-Addressed Storage
- Datenbanken
- verteilte Storage-Systeme
- Remote Storage
- Cloud Storage
- Network Storage
- andere persistente Objektspeicher

Die logische Namespace-Architektur darf davon nicht abhängig sein.

---

## 6. Logische Struktur

Die logische Struktur beschreibt die Sicht, die NovaOS einem Benutzer, einer Anwendung, einem Prozess, einer Solution oder einem anderen Systemkontext bereitstellt.

Beispiel:

~~~text
Computer/
├── Benutzer/
├── Capability/
├── Laufwerke/
├── Programme/
├── Solutions/
└── System/
~~~

Diese Struktur ist nicht zwingend die physische Root-Verzeichnisstruktur eines Datenträgers.

Sie kann vollständig oder teilweise als virtuelle Namespace-Projektion erzeugt werden.

---

## 7. Namespace-Projektion

Eine Namespace-Projektion ordnet vorhandene Objekte einer logischen Hierarchie zu.

Die Projektion kann unter anderem basieren auf:

- Objektidentität
- Name
- Semantic Type
- Metadaten
- Benutzer
- Eigentümer
- Beziehungen
- Arbeitsbereich
- Solution
- Capability
- Storage Location
- Sicherheitsrichtlinien
- Data-Sovereignty-Regeln
- Systemzustand
- Benutzerpräferenzen

Beispielsweise kann ein Bildobjekt automatisch unter:

~~~text
Benutzer/<Name>/Medien/Bilder/
~~~

erscheinen, obwohl es physisch nicht in einem Verzeichnis `Medien/Bilder` gespeichert ist.

---

## 8. Mehrere Projektionen desselben Objekts

Ein Objekt DARF gleichzeitig in mehreren logischen Strukturen erscheinen.

Beispiel:

~~~text
Arbeitsbereiche/Bahnbau/Unterlagen/Bericht.pdf
~~~

und:

~~~text
Dateien/PDF/Bericht.pdf
~~~

und:

~~~text
Solutions/Bahnbau/Dokumente/Bericht.pdf
~~~

können dasselbe zugrunde liegende Objekt referenzieren.

Es entstehen dadurch NICHT automatisch drei physische Kopien.

Formal:

~~~text
NamespaceEntry A ─┐
NamespaceEntry B ─┼──> Object X
NamespaceEntry C ─┘
~~~

Das Objekt besitzt weiterhin eine eindeutige Identität.

---

## 9. Objektidentität

Die Identität eines Objekts darf nicht ausschließlich aus seinem Pfad entstehen.

Ein Objekt MUSS eine vom logischen Pfad unabhängige stabile Identität besitzen können.

Dadurch bleibt das Objekt dasselbe, wenn:

- es umbenannt wird
- eine Projektion geändert wird
- es in einem anderen Arbeitsbereich erscheint
- sein physischer Speicherort verändert wird
- es auf ein anderes Volume verschoben wird
- es auf Remote Storage migriert wird

Beispiel:

~~~text
ObjectID: 8f4c...
~~~

könnte zunächst projiziert werden als:

~~~text
Dateien/Bericht.pdf
~~~

und später als:

~~~text
Arbeitsbereiche/Projekt-A/Dokumentation/Abschlussbericht.pdf
~~~

Die Objektidentität bleibt unverändert.

Das genaue Format der Objektidentität wird durch eine separate Spezifikation definiert.

---

## 10. Semantische Projektion

Namespace-Projektionen dürfen auf Semantic Types basieren.

Beispielsweise:

~~~text
Nova.Image.Photo
Nova.Audio.Music
Nova.Video.Movie
Nova.Document.PDF
~~~

Ein Objekt mit:

~~~text
SemanticType = Nova.Image.Photo
~~~

kann dadurch automatisch unter:

~~~text
Medien/Bilder/
~~~

erscheinen.

Die physische Speicherung muss dafür nicht verändert werden.

Dies erlaubt NovaOS, Daten nach ihrer Bedeutung statt ausschließlich nach ihrem Speicherort zu organisieren.

---

## 11. Benutzerstruktur

Für Benutzer wird eine semantisch orientierte Struktur vorgesehen.

Beispiel:

~~~text
Benutzer/<Name>/
├── Arbeitsbereiche/
├── Dateien/
├── Daten/
├── Einstellungen/
├── Medien/
│   ├── Bilder/
│   ├── Musik/
│   └── Videos/
└── Temporär/
~~~

Diese Struktur stellt eine logische Benutzersicht dar.

Einzelne Bereiche können vollständig oder teilweise durch Namespace-Projektionen erzeugt werden.

---

## 12. Bereich `Daten`

Der Benutzerbereich:

~~~text
Benutzer/<Name>/Daten/
~~~

ist insbesondere für technische Daten der Namespace-Projektion vorgesehen.

Dazu können beispielsweise gehören:

~~~text
NamespaceIndex
MetadataIndex
SearchIndex
RelationIndex
WorkspaceIndex
~~~

Solche Indizes sind grundsätzlich als rekonstruierbarer Derived State zu behandeln, sofern eine spezifische Spezifikation nichts anderes festlegt.

Die alleinige autoritative Information über die Existenz eines Benutzerobjekts darf nicht ausschließlich in einem rekonstruierbaren Index gespeichert werden.

Vereinfacht:

~~~text
Authoritative Object State
          |
          v
     Derived Index
          |
          v
 Namespace Projection
~~~

Nicht:

~~~text
Index
  |
  v
einzige Datenwahrheit
~~~

---

## 13. Rekonstruktion

Eine beschädigte oder gelöschte rekonstruierbare Namespace-Datenbank darf nicht automatisch zum Verlust der zugrunde liegenden Benutzerobjekte führen.

NovaOS SOLL in der Lage sein, entsprechende Strukturen aus autoritativen Informationen erneut aufzubauen.

Beispiel:

~~~text
Object Store
     +
Metadata
     +
Relations
     |
     v
Index Rebuild
     |
     v
Namespace Projection
~~~

Dies unterstützt die Self-Healing-Architektur von NovaOS.

---

## 14. Location Transparency

Die logische Position eines Objekts darf nicht zwingend dessen tatsächliche Storage Location offenlegen.

Ein Objekt kann beispielsweise physisch liegen auf:

- interner SSD
- zweiter SSD
- HDD
- USB-Datenträger
- NAS
- Remote-System
- verteiltem Storage
- zukünftigem Storage-Backend

Trotzdem kann es unter demselben logischen Namespace erscheinen.

Beispiel:

~~~text
Arbeitsbereiche/Projekt-A/Modelle/Modell.nova
~~~

Die Anwendung muss nicht zwingend wissen, ob das Objekt physisch auf:

~~~text
Local SSD
~~~

oder:

~~~text
Company NAS
~~~

gespeichert ist.

Storage Location und Namespace Location werden daher getrennt behandelt.

---

## 15. Devices, Volumes und Locations

NovaOS unterscheidet zwischen:

~~~text
Device
Volume
Location
~~~

Diese Begriffe dürfen nicht synonym verwendet werden.

### Device

Ein Device repräsentiert ein physisches oder virtuelles Speichergerät.

Beispiel:

~~~text
Samsung SSD 990 Pro
USB Storage Device
NVMe Device
Virtual Block Device
~~~

### Volume

Ein Volume repräsentiert einen logisch adressierbaren Speicherbereich.

Beispiel:

~~~text
System
Daten
Backup
~~~

### Location

Eine Location repräsentiert einen logischen oder entfernten Speicherort.

Beispiele:

~~~text
NAS
Cloud
Remote NovaOS System
Cluster Storage
Company Storage
~~~

Die logische Namespace-Projektion kann Objekte aus allen drei Bereichen einheitlich darstellen.

---

## 16. Laufwerke

Der sichtbare Bereich:

~~~text
Computer/Laufwerke/
~~~

ist als logische beziehungsweise virtuelle Sicht zu verstehen.

Er darf Devices, Volumes und Locations darstellen, ohne dass deren Darstellung zwingend einem realen Mount-Punkt entspricht.

Beispiel:

~~~text
Laufwerke/
├── System
├── Daten
├── Backup
├── USB-Stick
└── Firmen-NAS
~~~

Die konkrete interne Adressierung wird durch separate Storage- und Namespace-Spezifikationen definiert.

NovaOS verwendet keine klassischen Laufwerksbuchstaben als primäres Identitätsmodell.

---

## 17. Namespace-Einträge

Ein Namespace-Eintrag repräsentiert eine logische Zuordnung zu einem Objekt oder einer anderen Namespace-Ressource.

Ein Namespace-Eintrag kann mindestens enthalten:

~~~text
EntryID
Parent
DisplayName
CanonicalName
TargetObject
SemanticRole
ProjectionSource
Permissions
Visibility
Attributes
~~~

Nicht jeder Namespace-Eintrag muss eine physisch gespeicherte Verzeichnisstruktur besitzen.

---

## 18. Projektionen und Relationen

Namespace-Projektionen können aus Objektbeziehungen erzeugt werden.

Ein Objekt kann beispielsweise folgende Relation besitzen:

~~~text
Object: Bericht.pdf
Relation:
    Workspace = Projekt-A
~~~

Daraus kann automatisch die Projektion:

~~~text
Arbeitsbereiche/Projekt-A/Bericht.pdf
~~~

entstehen.

Eine weitere Relation:

~~~text
SemanticType = Nova.Document.PDF
~~~

kann zusätzlich eine Projektion unter:

~~~text
Dateien/PDF/Bericht.pdf
~~~

erzeugen.

Die Projektion selbst ist nicht die primäre Datenwahrheit.

---

## 19. Umbenennen

Das Umbenennen eines Namespace-Eintrags darf nicht automatisch die Identität des zugrunde liegenden Objekts ändern.

Beispiel:

~~~text
Bericht.pdf
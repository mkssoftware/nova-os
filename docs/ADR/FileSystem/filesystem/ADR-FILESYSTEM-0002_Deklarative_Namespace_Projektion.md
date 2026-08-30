# ADR-FILESYSTEM-0002 – Deklarative Namespace-Projektion

**Status:** Angenommen  
**Kategorie:** Filesystem / Namespace Architecture  
**Entscheidungstyp:** Architekturentscheidung  
**Gültigkeit:** Systemweit  
**Version:** 1.0  
**Datum:** 2026-08-28  

---

## 1. Kontext

Mit `ADR-FILESYSTEM-0001_Physische_und_logische_Struktur_trennen` wurde festgelegt, dass NovaOS die physische Speicherung von Objekten von deren logischer Darstellung im Namespace trennt.

Damit kann die für Benutzer, Anwendungen, Solutions und Systemkomponenten sichtbare Struktur unabhängig von der tatsächlichen physischen Ablage aufgebaut werden.

Aus dieser Trennung entsteht die Notwendigkeit eines einheitlichen Mechanismus, der bestimmt:

- welche Objekte in einem Namespace erscheinen,
- unter welchem Namen sie erscheinen,
- an welcher logischen Position sie erscheinen,
- aufgrund welcher Eigenschaften sie dort erscheinen,
- für welchen Benutzer oder Kontext sie sichtbar sind,
- welche Beziehungen zwischen Objekten dargestellt werden,
- welche Semantic Types berücksichtigt werden,
- welche Storage Locations eingebunden werden,
- welche Sicherheits- und Policy-Regeln gelten.

Eine solche logische Struktur darf nicht ausschließlich durch fest programmierten Anwendungscode erzeugt werden.

NovaOS benötigt deshalb ein systemweites Modell, mit dem Namespace-Sichten beschrieben und durch das Betriebssystem reproduzierbar erzeugt werden können.

---

## 2. Entscheidung

NovaOS verwendet eine **deklarative Namespace-Projektion**.

Eine Namespace-Projektion beschreibt deklarativ:

> Welche Objekte unter welchen Bedingungen in welcher logischen Struktur erscheinen sollen.

Die Projektion beschreibt das gewünschte Ergebnis beziehungsweise die Regeln zur Erzeugung dieses Ergebnisses.

Sie beschreibt nicht zwingend die einzelnen imperativen Schritte, mit denen dieses Ergebnis hergestellt wird.

Grundprinzip:

```text
Objekte
   +
Metadaten
   +
Semantic Types
   +
Relationen
   +
Kontext
   +
Policies
   |
   v
Deklarative Projektionsdefinition
   |
   v
Projection Engine
   |
   v
Logischer Namespace
```

Damit wird die Namespace-Struktur zu einem Teil des Declarative System Model von NovaOS.

---

## 3. Grundprinzip

Eine Namespace-Projektion soll beispielsweise ausdrücken können:

```text
Zeige alle Objekte,
deren Semantic Type von Nova.Image abgeleitet ist,
im Benutzerbereich Medien/Bilder.
```

Nicht erforderlich ist eine imperative Beschreibung wie:

```text
Durchsuche Speicher A.
Durchsuche Speicher B.
Öffne Datenbank C.
Lies Metadaten D.
Erzeuge Verzeichnis E.
Erzeuge Eintrag F.
```

Die konkrete Auflösung und Optimierung übernimmt NovaOS.

Formal:

```text
Desired Namespace State
          |
          v
   Projection Engine
          |
          v
 Current Namespace State
```

Die deklarative Definition beschreibt somit den gewünschten logischen Zustand.

---

## 4. Ziele

Die deklarative Namespace-Projektion verfolgt insbesondere folgende Ziele:

1. Trennung von Namespace-Definition und Storage-Implementierung
2. einheitliche Namespace-Erzeugung
3. semantische Datenorganisation
4. dynamische logische Sichten
5. mehrere Projektionen desselben Objekts
6. kontextabhängige Sichten
7. benutzerspezifische Sichten
8. Solution-spezifische Sichten
9. Workspace-spezifische Sichten
10. Capability-basierte Sichten
11. Location Transparency
12. Policy-basierte Sichtbarkeit
13. Data-Sovereignty-Integration
14. rekonstruierbare Namespace-Strukturen
15. transaktionale Änderungen
16. systemweite Introspection
17. Self-Healing
18. Optimierbarkeit durch NovaOS
19. langfristige Erweiterbarkeit
20. Unabhängigkeit von konkreten Dateisystemen

---

## 5. Deklaratives Modell

Eine Projektion besteht konzeptionell aus einer Menge von Regeln.

Eine Regel kann mindestens folgende Aspekte beschreiben:

```text
Source
Selector
Filter
SemanticType
Relation
Target
Naming
Grouping
Ordering
Visibility
Permissions
Context
Policy
Priority
ConflictResolution
Lifecycle
```

Nicht jede Projektion muss alle Eigenschaften verwenden.

Eine einfache Projektion kann beispielsweise lediglich definieren:

```text
Source:
    UserObjects

Filter:
    SemanticType is Nova.Image

Target:
    Benutzer/<CurrentUser>/Medien/Bilder
```

---

## 6. Quelle einer Projektion

Eine Projektion kann Objekte aus unterschiedlichen Quellen beziehen.

Beispiele:

```text
User Objects
System Objects
Capabilities
Programs
Solutions
Workspaces
Volumes
Devices
Locations
Remote Locations
Object Stores
Registries
Search Results
Relations
```

Eine Projektionsquelle ist nicht notwendigerweise ein physisches Verzeichnis.

Beispielsweise kann:

```text
Source = Capabilities
```

bedeuten, dass die Capability Registry abgefragt wird.

Ebenso kann:

```text
Source = UserObjects
```

Objekte aus mehreren physischen Storage Locations umfassen.

---

## 7. Selektion

Projektionen müssen Objekte anhand definierter Eigenschaften auswählen können.

Mögliche Selektionskriterien sind unter anderem:

```text
ObjectID
ObjectType
SemanticType
Name
Metadata
Relation
Owner
User
Workspace
Solution
Capability
Location
Volume
Device
Trust
Permission
Policy
State
Time
Tags
Attributes
```

Selektionsregeln können miteinander kombiniert werden.

Beispiel:

```text
SemanticType is Nova.Image.Photo
AND
Owner is CurrentUser
```

---

## 8. Semantic Types

Semantic Types sind ein primärer Mechanismus für semantische Namespace-Projektionen.

Beispielsweise können Objekte mit:

```text
Nova.Image.Photo
Nova.Image.Drawing
Nova.Image.Render
```

unter einer gemeinsamen Sicht:

```text
Medien/Bilder/
```

erscheinen.

Eine Projektion darf Vererbung beziehungsweise Typkompatibilität berücksichtigen.

Beispiel:

```text
Select:
    SemanticType is-a Nova.Image
```

kann alle kompatiblen Bildtypen einschließen.

Dadurch ist die Namespace-Projektion nicht ausschließlich von Dateiendungen abhängig.

---

## 9. Metadaten

Projektionen dürfen Metadaten verwenden.

Beispiele:

```text
Author
Created
Modified
Project
Category
Tags
Rating
Language
Location
Device
Application
Source
```

Damit können beispielsweise dynamische Sichten entstehen:

```text
Dateien/
└── Dieses Jahr/
```

oder:

```text
Medien/
└── Favoriten/
```

oder:

```text
Arbeitsbereiche/
└── Aktive Projekte/
```

Diese Strukturen müssen nicht physisch existieren.

---

## 10. Relationen

Objektrelationen dürfen Namespace-Projektionen erzeugen.

Beispiel:

```text
Object:
    Bericht.pdf

Relation:
    belongs-to Workspace Projekt-A
```

kann zu:

```text
Arbeitsbereiche/
└── Projekt-A/
    └── Bericht.pdf
```

führen.

Ein weiteres Objekt kann dieselbe Relation besitzen und dadurch automatisch im gleichen Arbeitsbereich erscheinen.

Die Namespace-Struktur wird damit aus den semantischen Beziehungen der Objekte abgeleitet.

---

## 11. Ziel einer Projektion

Jede Projektion definiert einen logischen Zielbereich.

Beispiele:

```text
Benutzer/<User>/Medien/Bilder/
Benutzer/<User>/Arbeitsbereiche/
Capability/image/
Solutions/
Laufwerke/
System/Services/
```

Das Ziel beschreibt eine logische Namespace-Position.

Es beschreibt nicht zwingend einen physischen Speicherort.

---

## 12. Benutzerspezifische Projektionen

Projektionen dürfen vom aktuellen Benutzer abhängen.

Beispielsweise:

```text
Target:
    Benutzer/<CurrentUser>/Medien/Bilder
```

Der Platzhalter:

```text
<CurrentUser>
```

wird durch den aktuellen Benutzerkontext aufgelöst.

Damit kann dieselbe Projektionsdefinition für mehrere Benutzer verwendet werden.

Beispiel:

```text
Benutzer/Maik/Medien/Bilder/
Benutzer/Anna/Medien/Bilder/
```

können aus derselben deklarativen Regel entstehen.

---

## 13. Benutzerstruktur

Die festgelegte semantische Benutzerstruktur lautet grundsätzlich:

```text
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
```

Diese Struktur darf vollständig oder teilweise durch deklarative Namespace-Projektionen erzeugt werden.

Insbesondere Bereiche wie:

```text
Arbeitsbereiche
Dateien
Medien
```

können semantische Sichten auf Objekte darstellen.

---

## 14. Namespace-Projektion im Bereich `Daten`

Der Bereich:

```text
Benutzer/<Name>/Daten/
```

ist unter anderem für technische Daten vorgesehen, welche die Namespace-Projektion unterstützen.

Dazu können gehören:

```text
NamespaceIndex
MetadataIndex
RelationIndex
WorkspaceIndex
SearchIndex
ProjectionCache
```

Diese Daten stellen grundsätzlich Derived State dar, soweit eine nachgeordnete Spezifikation nichts anderes festlegt.

Die deklarative Projektionsdefinition beziehungsweise die zugrunde liegenden autoritativen Objektinformationen dürfen nicht ausschließlich von einem rekonstruierbaren Index abhängen.

---

## 15. Mehrere Projektionen

Ein Objekt darf gleichzeitig durch beliebig viele gültige Projektionsregeln erfasst werden.

Beispiel:

```text
ObjectID:
    8f4c...

Name:
    Bericht.pdf

SemanticType:
    Nova.Document.PDF

Relations:
    Workspace = Projekt-A
    Solution = de.nova.bbau
```

Daraus können gleichzeitig folgende Sichten entstehen:

```text
Benutzer/Maik/Dateien/PDF/Bericht.pdf
```

```text
Benutzer/Maik/Arbeitsbereiche/Projekt-A/Bericht.pdf
```

```text
Solutions/de.nova.bbau/Dokumente/Bericht.pdf
```

Alle Einträge können auf dieselbe ObjectID zeigen.

---

## 16. Keine automatische Datenkopie

Das Erzeugen einer weiteren Projektion darf standardmäßig keine physische Kopie des zugrunde liegenden Objekts erzeugen.

Es gilt:

```text
Projection != Copy
```

Beispiel:

```text
Projection A ─┐
Projection B ─┼──> Object X
Projection C ─┘
```

Die Entscheidung über physische Replikation gehört zur Storage-, Synchronisations- oder Data-Sovereignty-Architektur und nicht zur Namespace-Projektion selbst.

---

## 17. Kontextabhängige Projektionen

Projektionen dürfen abhängig vom aktuellen Kontext unterschiedliche Ergebnisse erzeugen.

Kontext kann beispielsweise sein:

```text
CurrentUser
CurrentProcess
CurrentProgram
CurrentSolution
CurrentWorkspace
CurrentDevice
CurrentLocation
CurrentSecurityContext
CurrentTrustContext
CurrentPolicyContext
```

Damit kann dasselbe logische Ziel für unterschiedliche Kontexte unterschiedliche Inhalte darstellen.

---

## 18. Prozessbezogene Projektionen

NovaOS darf für Prozesse eigene Namespace-Projektionen erzeugen.

Dies ist insbesondere für private Programmabhängigkeiten relevant.

Beispiel:

```text
Programme/
└── ExampleProgram/
    └── SYS/
        └── Libraries/
            └── ExampleLibrary/
```

Für Prozesse dieses Programms kann deklarativ projiziert werden:

```text
Programme/ExampleProgram/SYS/Libraries/ExampleLibrary
```

nach:

```text
/System/Libraries/ExampleLibrary
```

Der globale System-Namespace wird dadurch nicht physisch verändert.

Andere Prozesse müssen diese Projektion nicht sehen.

---

## 19. Application Namespace Overlay

Das `SYS`-Modell für Programme wird als spezielle Form einer Namespace-Projektion beziehungsweise eines Namespace-Overlays behandelt.

Vereinfacht:

```text
Global System Namespace
          +
Program SYS Namespace
          |
          v
Effective Program Namespace
```

Beispiel:

```text
Global:
/System/Libraries/

Private:
Programme/App/SYS/Libraries/

Effective App View:
/System/Libraries/
```

Die genaue Prioritäts-, Konflikt- und Sicherheitssemantik wird durch separate ADRs und NPSPECs festgelegt.

---

## 20. Capability-Projektionen

Capabilities besitzen eine pfadunabhängige Identität.

Die Capability-ID folgt dem Schema:

```text
<domain>.<authority>.<namespace...>.<name>
```

Beispiel:

```text
de.nova.image.filter.gaussian
```

wird interpretiert als:

```text
Domain:    de
Authority: nova
Namespace: image.filter
Name:      gaussian
```

Eine Kategorie wie:

```text
Capability/image/
```

ist lediglich eine organisatorische oder logische Projektion.

Die Kategorie ist nicht Bestandteil der Capability-Identität.

Damit kann dieselbe Capability gegebenenfalls in mehreren Kategorien beziehungsweise Sichten erscheinen.

---

## 21. Solution-Projektionen

Solutions dürfen eigene Namespace-Projektionen definieren beziehungsweise anfordern.

Eine Solution kann beispielsweise relevante:

- Dateien,
- Arbeitsbereiche,
- Capabilities,
- Ressourcen,
- Datenobjekte

in einer für die Solution geeigneten logischen Struktur darstellen.

Diese Struktur verändert nicht automatisch die physische Speicherung der Objekte.

---

## 22. Workspace-Projektionen

Arbeitsbereiche können aus Relationen und Metadaten erzeugt werden.

Beispiel:

```text
Relation:
    Workspace = Projekt-A
```

führt zu einer Projektion unter:

```text
Benutzer/<User>/Arbeitsbereiche/Projekt-A/
```

Ein Workspace ist damit nicht auf ein klassisches physisches Verzeichnis beschränkt.

Er kann Objekte aus unterschiedlichen:

```text
Volumes
Locations
Solutions
Storage Backends
```

zusammenführen.

---

## 23. Laufwerke als Projektion

Der Bereich:

```text
Computer/Laufwerke/
```

wird als logische Sicht auf Storage-Ressourcen behandelt.

Er kann enthalten:

```text
Devices
Volumes
Locations
```

Beispiel:

```text
Laufwerke/
├── System
├── Daten
├── Backup
├── USB-Stick
└── Firmen-NAS
```

Die Einträge müssen nicht klassischen Mount-Punkten entsprechen.

---

## 24. System-Projektionen

Auch Teile des System-Namespace dürfen deklarativ erzeugt werden.

Die kanonische Systemtaxonomie umfasst unter anderem:

```text
System/
├── Boot/
├── Drivers/
├── Foundation/
├── Framework/
├── HAL/
├── Kernel/
├── Libraries/
├── Locale/
├── Modules/
├── Policy/
├── Recovery/
├── Registry/
├── Resources/
├── Runtime/
├── Security/
├── Services/
├── Trust/
└── Updates/
```

NovaOS besitzt bewusst einen zentralen Bereich:

```text
System/Framework/
```

im Singular.

Nicht jeder sichtbare Systembereich muss einer identischen physischen Verzeichnisstruktur entsprechen.

---

## 25. Statische Projektionen

Eine Projektion darf statisch sein.

Beispiel:

```text
Create logical namespace:
    System/Framework
```

Eine solche Projektion erzeugt eine bekannte logische Struktur unabhängig davon, ob momentan Objekte darin vorhanden sind.

---

## 26. Dynamische Projektionen

Eine Projektion darf dynamisch aus dem aktuellen Systemzustand erzeugt werden.

Beispiel:

```text
Laufwerke/
```

kann sich automatisch ändern, wenn:

- ein USB-Gerät verbunden wird,
- ein Volume verfügbar wird,
- eine NAS-Location erreichbar wird,
- eine Remote Location getrennt wird.

Die deklarative Regel bleibt bestehen, während sich das Ergebnis dynamisch ändert.

---

## 27. Persistente Projektionen

Projektionen dürfen persistent definiert sein.

Eine persistente Projektion bleibt über:

- Neustarts,
- Benutzeranmeldungen,
- Prozessstarts

hinweg erhalten.

Die Projektionsdefinition ist dabei persistent.

Das materialisierte beziehungsweise gecachte Projektionsergebnis muss nicht persistent sein.

---

## 28. Temporäre Projektionen

NovaOS darf temporäre Projektionen erzeugen.

Beispiele:

```text
Process Projection
Session Projection
Program Projection
Solution Projection
Workspace Projection
Temporary Search Projection
```

Eine temporäre Projektion kann automatisch verschwinden, wenn ihr Kontext endet.

---

## 29. Suchprojektionen

Suchergebnisse können als temporäre Namespace-Projektion dargestellt werden.

Beispiel:

```text
Suche/
└── "Bahnbau 2026"/
    ├── Bericht.pdf
    ├── Messung.csv
    └── Strecke.nova
```

Diese Ansicht stellt keine physische Verzeichnisstruktur dar.

Sie ist eine Projektion des Suchergebnisses.

---

## 30. Gruppierung

Projektionen dürfen Objekte dynamisch gruppieren.

Beispiele:

```text
Dateien/
├── PDF/
├── Tabellen/
├── Präsentationen/
└── Quellcode/
```

oder:

```text
Medien/Bilder/
├── Heute/
├── Diese Woche/
├── Dieses Jahr/
└── Älter/
```

Gruppierungen können aus:

- Semantic Types,
- Metadaten,
- Zeitinformationen,
- Relationen,
- Policies

abgeleitet werden.

---

## 31. Sortierung

Eine Projektion darf eine Standard-Sortierung definieren.

Beispiele:

```text
Name
Created
Modified
Type
SemanticType
Relevance
Relation
Priority
CustomMetadata
```

Sortierung beeinflusst nicht die Identität oder physische Speicherung eines Objekts.

---

## 32. Benennung

Eine Projektion darf bestimmen, wie ein Objekt innerhalb der Projektion angezeigt wird.

Dabei muss zwischen mindestens folgenden Konzepten unterschieden werden:

```text
Object Identity
Canonical Name
Projection Name
Display Name
Localized Display Name
```

Ein anderer Anzeigename innerhalb einer Projektion darf die ObjectID nicht verändern.

---

## 33. Lokalisierung

Projektionsnamen dürfen lokalisiert werden.

Ein kanonischer interner Namespace kann beispielsweise:

```text
Users
```

verwenden, während die deutsche Benutzeroberfläche:

```text
Benutzer
```

anzeigt.

Die lokalisierte Darstellung darf die interne Identität des Namespace nicht verändern.

---

## 34. Sichtbarkeit

Projektionsregeln dürfen Sichtbarkeitsregeln enthalten.

Beispielsweise:

```text
VisibleTo:
    CurrentUser
```

oder:

```text
VisibleWhen:
    DeveloperMode = true
```

oder:

```text
VisibleWhen:
    Permission(SystemDeveloper) = granted
```

Sichtbarkeit ist von Zugriffsberechtigung zu unterscheiden.

Ein unsichtbares Objekt kann weiterhin existieren.

---

## 35. Berechtigungen

Eine Namespace-Projektion darf keine Berechtigungen erzeugen, die für das zugrunde liegende Objekt nicht vorhanden sind.

Es gilt:

```text
Projection Visibility
        !=
Object Authorization
```

Eine Projektion darf ein Objekt nur dann nutzbar machen, wenn der aktuelle Sicherheitskontext Zugriff auf das Objekt besitzt.

Die effektive Berechtigung kann von mehreren Ebenen abhängen:

```text
Object Permission
Namespace Permission
User Permission
Process Permission
Capability Permission
Solution Permission
Policy
Trust
Data Sovereignty
Information Flow
```

---

## 36. Trust

Projektionsregeln dürfen Trust-Eigenschaften berücksichtigen.

Beispielsweise kann eine Systemansicht ausschließlich Capabilities anzeigen, die bestimmte Trust-Anforderungen erfüllen.

Beispiel:

```text
Select:
    ObjectType = Capability
    AND
    TrustLevel >= RequiredLevel
```

Die genaue Trust-Semantik wird durch Nova.Trust definiert.

---

## 37. Data Sovereignty

Namespace-Projektionen müssen Data-Sovereignty-Regeln respektieren.

Die gemeinsame logische Darstellung mehrerer Locations bedeutet nicht, dass Daten zwischen diesen Locations verschoben werden dürfen.

Beispiel:

```text
Workspace Projekt-A
├── Lokales Dokument
├── Firmen-NAS-Dokument
└── Remote-Dokument
```

Die gemeinsame Projektion erzeugt keine automatische Storage-Migration.

---

## 38. Information Flow

Projektionsregeln müssen mit systemweiten Information-Flow-Regeln kompatibel sein.

Ein Objekt darf nicht allein durch seine Projektion in einen Sicherheitsbereich gelangen, in den ein Informationsfluss verboten ist.

Die Projection Engine muss entsprechende Policy-Entscheidungen berücksichtigen.

---

## 39. Konflikte

Mehrere Projektionsregeln können denselben logischen Zielnamen erzeugen.

Beispiel:

```text
Projekt-A/Bericht.pdf
Projekt-A/Bericht.pdf
```

wobei beide Einträge unterschiedliche ObjectIDs besitzen.

NovaOS benötigt deshalb eine definierte Konfliktbehandlung.

Mögliche Strategien können sein:

```text
Disambiguation
Priority
Namespace Qualification
User Resolution
Policy Resolution
Projection-Specific Naming
```

Die genaue Konfliktsemantik wird in einer NPSPEC festgelegt.

Stilles Überschreiben ist standardmäßig nicht zulässig.

---

## 40. Prioritäten

Projektionsregeln dürfen Prioritäten besitzen.

Dies ist insbesondere für Namespace-Overlays erforderlich.

Beispiel:

```text
Program Private SYS
        >
Global System Fallback
```

Eine höhere Projektionspriorität darf jedoch keine Sicherheits- oder Trust-Regeln umgehen.

---

## 41. Auflösung

Ein logischer Pfad wird durch den Namespace Resolver und die Projection Engine aufgelöst.

Vereinfacht:

```text
Logical Path
     |
     v
Namespace Resolver
     |
     v
Projection Selection
     |
     v
Projection Rule Evaluation
     |
     v
ObjectID
     |
     v
Storage Resolution
     |
     v
Physical Representation
```

Die einzelnen Ebenen dürfen intern gecacht oder optimiert werden.

---

## 42. Materialisierung

Eine Projektion muss nicht vollständig materialisiert werden.

NovaOS darf Projektionen:

```text
eager
lazy
on-demand
cached
partially materialized
```

auswerten.

Beispielsweise muss ein Namespace mit Millionen Objekten nicht beim Systemstart vollständig erzeugt werden.

Die logische Semantik muss unabhängig von der gewählten Materialisierungsstrategie gleich bleiben.

---

## 43. Lazy Evaluation

Projektionsregeln dürfen erst ausgewertet werden, wenn der entsprechende Namespace tatsächlich angefordert wird.

Beispiel:

```text
Benutzer/Maik/Medien/Bilder/
```

muss nicht vollständig aufgelöst werden, solange kein Prozess diesen Bereich öffnet oder abfragt.

Dies reduziert:

- Startzeit,
- Speicherverbrauch,
- unnötige Storage-Zugriffe,
- Remote-Zugriffe.

---

## 44. Caching

Ergebnisse von Projektionen dürfen gecacht werden.

Beispiele:

```text
ProjectionCache
NamespaceCache
RelationCache
MetadataCache
ResolutionCache
```

Caches sind Derived State.

Sie müssen:

- invalidierbar,
- rekonstruierbar,
- auf Konsistenz prüfbar

sein.

Ein Cache darf nicht die einzige autoritative Quelle einer persistenten Objektbeziehung darstellen.

---

## 45. Aktualisierung

Ändern sich Eigenschaften eines Objekts, muss NovaOS betroffene Projektionen aktualisieren können.

Beispiel:

```text
SemanticType:
    Nova.Document.Text
```

ändert sich zu:

```text
Nova.Document.PDF
```

Dann kann sich die logische Projektion automatisch ändern.

Beispielsweise:

```text
Dateien/Text/
```

zu:

```text
Dateien/PDF/
```

Die physische Speicherung muss dabei nicht verändert werden.

---

## 46. Ereignismodell

Die Projection Engine soll auf relevante Systemereignisse reagieren können.

Beispiele:

```text
ObjectCreated
ObjectDeleted
MetadataChanged
SemanticTypeChanged
RelationAdded
RelationRemoved
LocationAvailable
LocationUnavailable
VolumeMounted
VolumeRemoved
UserChanged
PolicyChanged
TrustChanged
WorkspaceChanged
SolutionChanged
```

Dadurch können Namespace-Sichten inkrementell aktualisiert werden.

---

## 47. Transaktionen

Änderungen, die mehrere Projektionszustände betreffen, sollen transaktional ausgeführt werden.

Beispiel:

```text
Objekt erhält Workspace-Relation
        |
        +--> Workspace Projection
        +--> Search Index
        +--> Relation Index
        +--> Namespace Cache
```

Die Operation soll nicht dauerhaft in einem teilweise aktualisierten Zustand verbleiben.

---

## 48. Rebuild

Da Projektionen deklarativ definiert sind, soll NovaOS sie aus den autoritativen Informationen rekonstruieren können.

Vereinfacht:

```text
Projection Definitions
        +
Objects
        +
Metadata
        +
Relations
        +
Policies
        |
        v
Projection Rebuild
        |
        v
Namespace
```

Dies ist ein wesentlicher Bestandteil der Self-Healing-Fähigkeit.

---

## 49. Self-Healing

NovaOS soll Inkonsistenzen zwischen:

```text
Projection Definition
Object State
Metadata
Relations
Indexes
Materialized Namespace State
```

erkennen können.

Wenn die autoritativen Informationen vorhanden sind, sollen abgeleitete Projektionszustände wiederhergestellt werden können.

---

## 50. Introspection

Deklarative Projektionen müssen grundsätzlich introspektierbar sein.

NovaOS soll nachvollziehen können:

- warum ein Objekt in einem Namespace erscheint,
- welche Projektionsregel verantwortlich ist,
- welche Quelle verwendet wurde,
- welche Filter erfüllt wurden,
- welche Relation die Projektion erzeugt hat,
- welche Policy angewendet wurde,
- welcher Kontext gilt.

Beispiel:

```text
Object:
    Bericht.pdf

VisibleAt:
    Arbeitsbereiche/Projekt-A/Bericht.pdf

Reason:
    Projection: WorkspaceDocuments
    Relation: belongs-to(Project-A)
```

Dies ist insbesondere für Debugging, Administration und Architecture Introspection wichtig.

---

## 51. Erklärbarkeit

Eine Projektion soll nicht als undurchsichtiger Mechanismus behandelt werden.

Für Diagnosewerkzeuge soll NovaOS eine Erklärungskette bereitstellen können.

Beispiel:

```text
Namespace Entry
      |
      v
Projection Rule
      |
      v
Matched Selector
      |
      v
Matched Relation
      |
      v
ObjectID
      |
      v
Storage Location
```

Damit kann nachvollzogen werden, woher ein sichtbarer Eintrag stammt.

---

## 52. Determinismus

Bei identischem:

```text
Object State
Metadata State
Relation State
Projection Definition
Policy State
Context
```

soll eine deterministische Projektionsregel dasselbe logische Ergebnis erzeugen.

Nichtdeterministische Projektionen müssen explizit als solche definiert werden.

Der Deterministic Mode von NovaOS kann strengere Anforderungen an Projektionsregeln stellen.

---

## 53. Performance

Die deklarative Architektur darf nicht bedeuten, dass jede Namespace-Abfrage sämtliche Objekte des Systems durchsuchen muss.

NovaOS darf zur Optimierung verwenden:

```text
Indexes
Query Planning
Incremental Evaluation
Caching
Lazy Evaluation
Materialized Views
Prefetching
Parallel Evaluation
Structured Concurrency
```

Die Projection Engine darf anhand von Datenmenge, Storage und Laufzeitbedingungen selbst geeignete Ausführungsstrategien auswählen.

---

## 54. Ressourcenökonomie

Namespace-Projektionen unterliegen der systemweiten Ressourcenökonomie.

Die Projection Engine darf berücksichtigen:

```text
CPU Budget
Memory Budget
I/O Budget
Energy Budget
Latency Requirement
Deadline
Network Cost
Storage Cost
```

Eine Projektion darf deshalb abhängig vom Execution Contract unterschiedlich optimiert werden, ohne ihre logische Semantik zu verändern.

---

## 55. Remote Locations

Deklarative Projektionen dürfen Objekte aus Remote Locations einschließen.

Beispiel:

```text
Workspace Projekt-A
        |
        +--> Local SSD
        +--> NAS
        +--> Remote NovaOS System
```

Die logische Workspace-Struktur kann einheitlich dargestellt werden.

Die Projection Engine beziehungsweise Storage-Schicht ist für die tatsächliche Auflösung verantwortlich.

---

## 56. Offline-Zustände

Ein projiziertes Objekt darf sichtbar bleiben, obwohl seine physische Location momentan nicht erreichbar ist.

Der Zustand kann beispielsweise sein:

```text
Available
Unavailable
Offline
Cached
PendingSync
Remote
```

Der Namespace-Eintrag muss deshalb nicht automatisch entfernt werden.

---

## 57. Legacy-Kompatibilität

Legacy-Anwendungen dürfen über Projektionen eine klassische hierarchische Dateisystemsicht erhalten.

Beispielsweise kann eine Anwendung sehen:

```text
Documents/
Pictures/
Downloads/
```

obwohl NovaOS intern semantische Namespace-Projektionen verwendet.

Kompatibilitätsprojektionen dürfen die native NovaOS-Architektur nicht bestimmen.

---

## 58. Änderbarkeit

Projektionsdefinitionen können je nach Typ:

```text
System Defined
User Defined
Solution Defined
Program Defined
Workspace Defined
Policy Defined
```

sein.

Nicht jeder Kontext darf beliebige Projektionen erzeugen.

Die Berechtigung zur Definition oder Veränderung einer Projektion wird separat kontrolliert.

---

## 59. Systemprojektionen

Kritische Systemprojektionen werden vom Betriebssystem kontrolliert.

Beispiele:

```text
System/
Capability/
Programme/
Solutions/
Laufwerke/
```

Nicht privilegierte Anwendungen dürfen diese Projektionen nicht beliebig überschreiben oder manipulieren.

---

## 60. Benutzerprojektionen

Benutzer dürfen im erlaubten Kontext eigene Sichten auf ihre Daten definieren können.

Beispiele:

```text
Meine Projekte
Favoriten
Fotos 2026
Arbeitsdokumente
Zuletzt bearbeitet
```

Solche Sichten können als benutzerspezifische Projektionen umgesetzt werden.

---

## 61. Solution-definierte Projektionen

Solutions dürfen innerhalb ihres erlaubten Namespace eigene Projektionen definieren.

Eine Solution darf dadurch eine domänenspezifische Sicht auf vorhandene Objekte bereitstellen.

Beispiel:

```text
Bahnbau/
├── Projekte/
├── Strecken/
├── Messungen/
├── Maschinen/
└── Berichte/
```

Diese Struktur kann vollständig aus bestehenden Objekten, Semantic Types und Relationen entstehen.

---

## 62. Capability-definierte Erweiterungen

Capabilities dürfen Projektionsfunktionen oder Projektionsinformationen bereitstellen, wenn dies durch ihre Berechtigungen erlaubt ist.

Eine Capability darf jedoch nicht automatisch globale Namespace-Bereiche verändern.

Systemweite Änderungen erfordern die dafür vorgesehenen Berechtigungen und Policies.

---

## 63. Declarative System Model

Namespace-Projektionen sind Bestandteil des Declarative System Model von NovaOS.

Damit gilt grundsätzlich:

```text
Desired State
     |
     v
System Evaluation
     |
     v
Effective State
```

für Namespace-Strukturen ebenso wie für andere deklarative Systembereiche.

Die Namespace-Projektion soll deshalb mit dem allgemeinen deklarativen Systemmodell kompatibel sein.

---

## 64. Hot Replacement und Live Evolution

Projektionsdefinitionen sollen grundsätzlich aktualisierbar sein, ohne dass das gesamte Betriebssystem neu gestartet werden muss.

Eine neue Projektionsdefinition kann nach erfolgreicher:

```text
Validation
Trust Check
Policy Check
Compatibility Check
```

aktiviert werden.

Bestehende Namespace-Sichten können anschließend inkrementell angepasst werden.

---

## 65. Versionierung

Projektionsdefinitionen müssen versionierbar sein können.

Eine Änderung der Projektionssemantik darf nicht unkontrolliert bestehende Benutzerstrukturen verändern.

Bei inkompatiblen Änderungen können:

```text
Migration
Compatibility Rules
Fallback
Rollback
```

erforderlich sein.

---

## 66. Validierung

Vor Aktivierung einer Projektionsdefinition muss NovaOS deren strukturelle und semantische Gültigkeit prüfen können.

Zu prüfen sind mindestens:

```text
Syntax
Selectors
Target Namespace
Permissions
Policy Compatibility
Trust Requirements
Conflict Rules
Referenced Semantic Types
Referenced Relations
Referenced Capabilities
```

Ungültige Projektionsdefinitionen dürfen nicht unkontrolliert aktiviert werden.

---

## 67. Sicherheit

Eine Projektionsdefinition ist potenziell sicherheitsrelevant.

Sie könnte andernfalls versuchen:

- geschützte Objekte sichtbar zu machen,
- Systembereiche zu überschreiben,
- Namespace-Konflikte zu erzeugen,
- Daten aus getrennten Sicherheitskontexten zusammenzuführen,
- Information-Flow-Regeln zu umgehen.

Deshalb unterliegt ihre Aktivierung dem NovaOS-Sicherheits-, Trust- und Policy-Modell.

---

## 68. Audit

Sicherheitsrelevante Änderungen an Projektionsdefinitionen sollen auditierbar sein.

Ein Audit-Eintrag kann beispielsweise enthalten:

```text
ProjectionID
Version
Actor
Timestamp
Change
TrustContext
PolicyDecision
Result
```

---

## 69. Projektionsidentität

Jede persistente Projektionsdefinition soll eine stabile Identität besitzen.

Diese Identität ist unabhängig von ihrem Anzeigenamen.

Konzeptionell:

```text
ProjectionID
Version
Name
Owner
Scope
Definition
```

Das konkrete ID-Schema wird durch eine NPSPEC festgelegt.

---

## 70. Scope

Jede Projektion besitzt einen definierten Scope.

Mögliche Scopes sind:

```text
System
User
Session
Process
Program
Solution
Workspace
Temporary
```

Der Scope bestimmt unter anderem:

- Lebensdauer,
- Sichtbarkeit,
- Berechtigungsgrenzen,
- Kontext,
- Priorität.

---

## 71. Lebensdauer

Die Lebensdauer einer Projektion kann an ihren Scope gekoppelt sein.

Beispiele:

```text
System Projection
    -> persistent

User Projection
    -> persistent für Benutzer

Session Projection
    -> bis Session-Ende

Process Projection
    -> bis Prozessende

Program Projection
    -> solange Programmkontext aktiv

Temporary Projection
    -> definierte TTL oder explizites Entfernen
```

---

## 72. Projektionsabhängigkeiten

Eine Projektion darf von anderen Systemobjekten oder Projektionen abhängen.

Beispiele:

```text
Semantic Type Registry
Capability Registry
Workspace Registry
Relation Service
Storage Location
Another Projection
```

Abhängigkeiten müssen explizit nachvollziehbar sein.

Zyklische Projektionsabhängigkeiten müssen erkannt und kontrolliert behandelt werden.

---

## 73. Fehlerbehandlung

Kann eine Projektion nicht vollständig ausgewertet werden, muss NovaOS zwischen unterschiedlichen Fehlerklassen unterscheiden können.

Beispiele:

```text
SourceUnavailable
PermissionDenied
PolicyDenied
InvalidDefinition
DependencyMissing
Conflict
StorageUnavailable
Timeout
ResourceLimit
```

Ein Fehler in einer Projektion darf nicht automatisch den gesamten globalen Namespace unbrauchbar machen.

---

## 74. Graceful Degradation

Wenn einzelne Quellen nicht verfügbar sind, darf eine Projektion teilweise funktionsfähig bleiben, sofern ihre Semantik dies erlaubt.

Beispiel:

```text
Workspace/
├── Lokale Daten        [verfügbar]
├── NAS-Daten           [offline]
└── Remote-Daten        [verfügbar]
```

Die nicht verfügbare Quelle darf entsprechend gekennzeichnet werden.

---

## 75. Architekturregeln

Aus dieser ADR ergeben sich folgende verbindliche Regeln:

1. NovaOS verwendet deklarative Namespace-Projektionen als primären Mechanismus für virtuelle und semantische Namespace-Sichten.
2. Eine Projektionsdefinition beschreibt Regeln und gewünschten Zustand, nicht zwingend imperative Ausführungsschritte.
3. Namespace-Projektionen dürfen aus Objekten, Metadaten, Semantic Types, Relationen und Kontext erzeugt werden.
4. Ein Objekt darf gleichzeitig durch mehrere Projektionen sichtbar sein.
5. Eine zusätzliche Projektion erzeugt standardmäßig keine physische Datenkopie.
6. Projektionen dürfen benutzer-, prozess-, programm-, Solution- und Workspace-spezifisch sein.
7. Projektionsdefinitionen besitzen einen definierten Scope.
8. Projektionsdefinitionen müssen validierbar sein.
9. Projektionen müssen Sicherheits-, Trust-, Policy-, Data-Sovereignty- und Information-Flow-Regeln respektieren.
10. Projektions-Caches und materialisierte Sichten sind Derived State.
11. Rekonstruierbare Projektionen müssen aus autoritativen Informationen wiederherstellbar sein.
12. Projektionsregeln sollen introspektierbar und erklärbar sein.
13. Konflikte zwischen Projektionen müssen deterministisch oder explizit auflösbar behandelt werden.
14. Systemprojektionen dürfen nicht durch nicht privilegierte Prozesse beliebig verändert werden.
15. Projektionsauswertung darf lazy, gecacht oder inkrementell erfolgen.
16. Optimierungen dürfen die deklarative Semantik nicht verändern.
17. Projektionsänderungen sollen transaktional erfolgen.
18. Deklarative Namespace-Projektionen sind Bestandteil des Declarative System Model von NovaOS.
19. Physische Storage Locations bleiben von der logischen Projektion getrennt.
20. Ein logischer Namespace-Eintrag ist nicht automatisch ein physisches Verzeichnis.

---

## 76. Konsequenzen

### Positive Konsequenzen

Die Entscheidung ermöglicht:

- semantische Dateiansichten,
- dynamische Benutzerstrukturen,
- Workspace-basierte Organisation,
- Solution-spezifische Datenansichten,
- private Programm-Namespaces,
- Capability-Kategorien ohne Identitätskopplung,
- Location Transparency,
- Remote-Storage-Integration,
- mehrere Sichten ohne Datenkopien,
- globale Suche,
- Self-Healing,
- Architecture Introspection,
- zentrale Policy-Kontrolle,
- systemweite Konsistenz,
- flexible zukünftige UI-Konzepte.

### Negative Konsequenzen

Die Architektur erhöht die Komplexität insbesondere in:

- Namespace Resolver,
- Projection Engine,
- Caching,
- Indexierung,
- Konfliktauflösung,
- Sicherheitsprüfung,
- Debugging,
- Recovery,
- Legacy-Kompatibilität,
- Performanceoptimierung.

Zusätzlich werden klare Spezifikationen für:

```text
Projection Definition
Projection Scope
Projection Priority
Projection Conflict Resolution
Projection Lifecycle
Namespace Resolution
Object Identity
Metadata
Relations
Semantic Types
```

benötigt.

Diese zusätzliche Komplexität wird akzeptiert.

---

## 77. Verworfene Alternative A – Ausschließlich imperative Namespace-Erzeugung

Ein Modell, bei dem Anwendungen und Systemdienste Namespace-Einträge ausschließlich durch imperative Befehle erzeugen, wurde als primäres Modell verworfen.

Beispiel:

```text
CreateFolder(...)
CreateEntry(...)
MoveEntry(...)
CreateLink(...)
```

Solche Operationen bleiben als APIs möglich, bilden aber nicht das fundamentale Modell für dynamische semantische Sichten.

Grund:

- gewünschter Zustand ist schwerer rekonstruierbar,
- geringere Self-Healing-Fähigkeit,
- schlechtere Introspection,
- stärkere Kopplung an Ausführungslogik.

---

## 78. Verworfene Alternative B – Anwendungsspezifische Projektionen ohne Systemmodell

Ein Modell, bei dem jede Anwendung ihre eigenen virtuellen Ordner und Datenansichten vollständig selbst implementiert, wurde verworfen.

Dies würde zu:

- inkompatiblen Metadatenmodellen,
- mehrfachen Indizes,
- mehrfacher Suchlogik,
- unterschiedlichen Berechtigungsmodellen,
- redundanter Implementierung

führen.

NovaOS stellt deshalb ein gemeinsames systemweites Projektionsmodell bereit.

---

## 79. Verworfene Alternative C – Projektionen nur für die Benutzeroberfläche

Namespace-Projektionen ausschließlich als Explorer-Darstellung zu behandeln wurde verworfen.

Projektionen sind Bestandteil des System-Namespace und können auch von:

```text
Applications
Solutions
Capabilities
Services
Shell
APIs
NovaLang
NovaDOS
System Tools
```

verwendet werden.

Der Explorer ist lediglich ein möglicher Client der Namespace-Architektur.

---

## 80. Verworfene Alternative D – Physische Materialisierung jeder Projektion

Ein Modell, bei dem jede Projektion als reales Verzeichnis beziehungsweise als physische Linkstruktur materialisiert werden muss, wurde verworfen.

Grund:

- unnötiger Storage-Aufwand,
- schlechte Skalierbarkeit,
- komplexe Synchronisation,
- hohe Kosten bei dynamischen Sichten,
- Probleme bei Remote Locations.

Projektionen dürfen vollständig virtuell bleiben.

---

## 81. Abhängigkeiten

Diese ADR baut insbesondere auf folgender Entscheidung auf:

```text
ADR-FILESYSTEM-0001_Physische_und_logische_Struktur_trennen
```

Sie setzt außerdem die Existenz beziehungsweise spätere Definition folgender Konzepte voraus:

```text
Object Identity
Semantic Types
Metadata
Relations
Namespace Resolver
Projection Engine
Policy
Trust
Storage Resolution
```

---

## 82. Abhängige ADRs

Diese Entscheidung bildet insbesondere eine Grundlage für:

```text
ADR-FILESYSTEM-0003_Pfad_unabhaengige_Objektidentitaet
ADR-FILESYSTEM-0004_Mehrfache_logische_Sichten_auf_ein_Objekt
ADR-FILESYSTEM-0005_Semantische_Datei_und_Objektklassifikation
ADR-FILESYSTEM-0006_Rebuildable_Derived_State_fuer_Indizes
ADR-FILESYSTEM-0007_Namespace_Projektion_als_Systemdienst
ADR-FILESYSTEM-0008_Transaktionale_Namespace_Aenderungen
ADR-STORAGE-0002_Laufwerke_als_virtuelle_Benutzersicht
ADR-STORAGE-0004_Location_Transparency_im_Namespace
ADR-USERSPACE-0001_Semantische_Benutzerordnerstruktur
ADR-USERSPACE-0002_Arbeitsbereiche_als_eigene_Systemobjekte
ADR-USERSPACE-0004_Benutzerspezifischer_Datenbereich
ADR-USERSPACE-0007_Benutzerindizes_als_rekonstruierbarer_Zustand
ADR-PROGRAM-0003_Private_Systemabhaengigkeiten_ueber_SYS_Overlay
ADR-PROGRAM-0004_Prozessbezogener_System_Namespace
ADR-CAPABILITY-0005_Kategorieordner_nur_als_Organisation
ADR-WORKSPACE-0001_Workspace_als_semantisches_Systemobjekt
ADR-INDEX-0003_Namespace_Index_als_Derived_State
```

---

## 83. Zugehörige NPSPECs

Die technische Umsetzung wird insbesondere durch folgende Spezifikationen konkretisiert:

```text
NPSPEC-FILESYSTEM-NAMESPACE-0001
NPSPEC-FILESYSTEM-NAMESPACE-0002
NPSPEC-FILESYSTEM-PROJECTION-0001
NPSPEC-FILESYSTEM-PROJECTION-0002
NPSPEC-FILESYSTEM-OBJECTID-0001
NPSPEC-FILESYSTEM-SEMANTIC-0001
NPSPEC-FILESYSTEM-RELATION-0001
NPSPEC-FILESYSTEM-TRANSACTION-0001
NPSPEC-FILESYSTEM-PERMISSION-0001
NPSPEC-INDEX-NAMESPACE-0001
NPSPEC-INDEX-REBUILD-0001
NPSPEC-NAMESPACE-OVERLAY-0001
NPSPEC-NAMESPACE-OVERLAY-0002
NPSPEC-NAMESPACE-PROCESS-0001
NPSPEC-NAMESPACE-APPLICATION-0001
NPSPEC-NAMESPACE-USER-0001
NPSPEC-NAMESPACE-SYSTEM-0001
NPSPEC-NAMESPACE-VIRTUAL-0001
NPSPEC-NAMESPACE-RESOLUTION-0001
NPSPEC-NAMESPACE-PERMISSION-0001
NPSPEC-NAMESPACE-CACHE-0001
```

---

## 84. Endgültige Entscheidung

NovaOS verwendet deklarative Namespace-Projektionen als systemweiten Mechanismus zur Erzeugung logischer, virtueller, semantischer und kontextabhängiger Namespace-Sichten.

Das grundlegende Modell lautet:

```text
Authoritative Objects
        +
Metadata
        +
Semantic Types
        +
Relations
        +
Context
        +
Policies
        |
        v
Declarative Projection Definitions
        |
        v
Projection Engine
        |
        v
Logical Namespace
```

Die Projektionsdefinition beschreibt, welche logische Sicht entstehen soll.

NovaOS entscheidet innerhalb der geltenden Verträge und Policies, wie diese Sicht effizient erzeugt, aktualisiert, gecacht, rekonstruiert und bereitgestellt wird.

Damit ist die deklarative Namespace-Projektion ein grundlegender Bestandteil des NovaOS Declarative System Model und die zentrale Verbindung zwischen der objektorientierten physischen Datenhaltung und der für Benutzer, Programme, Capabilities, Solutions und Arbeitsbereiche sichtbaren logischen Struktur.
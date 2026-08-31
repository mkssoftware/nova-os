# ADR-STORAGE-0001 – VFS als Kompatibilitäts- und Projektionsschicht

## Status

Angenommen

## Kategorie

Kernel / Storage / VFS / Filesystem / Compatibility

## Kontext

NovaOS soll unterschiedliche Dateisysteme, Storage-Provider und bestehende Software unterstützen können.

Klassische Betriebssysteme verwenden dafür häufig ein Virtual File System als zentrale Abstraktion für:

- Dateien,
- Verzeichnisse,
- Mounts,
- Dateisystemtreiber.

NovaOS besitzt jedoch ein weitergehendes Systemmodell mit:

- benannten Volumes,
- Capabilities,
- Semantic Types,
- Data/Object Pipelining,
- Data Sovereignty,
- Location Transparency,
- expliziten Systemobjekten.

Ein klassisches VFS darf deshalb nicht zum grundlegenden Objekt- oder Datenmodell von NovaOS werden.

---

## Entscheidung

NovaOS verwendet ein VFS als **Kompatibilitäts- und Projektionsschicht über dem nativen Storage- und Objektmodell**.

```text
Applications / Compatibility APIs
              ↓
             VFS
              ↓
Storage / Object Abstractions
              ↓
Filesystem / Storage Provider
```

Das VFS stellt eine hierarchische Datei-/Verzeichnisansicht bereit, ohne vorauszusetzen, dass alle zugrunde liegenden Ressourcen tatsächlich klassische Dateien sind.

---

## Grundprinzip

```text
The VFS exposes a filesystem view.

It does not define
the entire storage model.
```

---

## Projektionsmodell

Das VFS darf unterschiedliche Ressourcen in einen gemeinsamen Namespace projizieren.

Dazu können gehören:

- klassische Dateisysteme,
- lokale Volumes,
- Netzwerk-Storage,
- virtuelle Ressourcen,
- Systemobjekte,
- kompatible externe Storage-Provider.

Eine VFS-Node ist damit nicht zwingend identisch mit einer physischen Datei.

---

## Benannte Volumes

NovaOS verwendet keine Laufwerksbuchstaben als natives Storage-Modell.

Volumes besitzen stabile Namen beziehungsweise Identitäten.

Beispiel:

```text
System:/
Benutzer:/
Daten:/
Recovery:/
```

Das VFS projiziert diese Volumes in den sichtbaren Namespace.

Kompatibilitätsschichten dürfen bei Bedarf andere Namensmodelle emulieren.

---

## Dateisystemtreiber

Konkrete Dateisysteme werden über Provider angebunden.

```text
VFS
 ├── Native Nova Filesystem
 ├── FAT
 ├── NTFS
 ├── ext
 └── weitere Provider
```

Das VFS definiert gemeinsame Operationen und Semantik, während formatspezifische Details beim jeweiligen Provider verbleiben.

---

## Native Semantik vor kleinster gemeinsamer Schnittmenge

Das VFS darf NovaOS nicht dauerhaft auf die Fähigkeiten klassischer Dateisysteme beschränken.

Erweiterte Eigenschaften wie:

- Semantic Types,
- Provenance,
- Data-Sovereignty-Metadaten,
- Capabilities,
- transaktionale Operationen

dürfen über das klassische Datei-/Verzeichnismodell hinausgehen.

Provider ohne entsprechende native Fähigkeiten können diese Eigenschaften über geeignete Projektions- oder Metadatenmechanismen abbilden.

---

## Location Transparency

Eine VFS-Ressource kann lokal, remote oder anderweitig bereitgestellt sein.

Der Namespace darf diese Unterschiede abstrahieren.

Kosten, Latenzen, Offline-Zustände und Fehler dürfen jedoch nicht verborgen werden, wenn sie für die Operation relevant sind.

```text
Same logical interface
≠
same physical cost
```

---

## Security und Authority

Ein Pfad oder Dateiname stellt keine Zugriffsberechtigung dar.

Zugriffe bleiben an:

- Identity,
- Authority,
- Capabilities,
- Security Policy,
- Data Sovereignty

gebunden.

Das VFS darf keine schwächere parallele Sicherheitsarchitektur etablieren.

---

## Transaktionale Operationen

VFS-Operationen sollen mit der transaktionalen Systemarchitektur von NovaOS kombinierbar sein.

Mehrstufige Storage-Operationen können dadurch atomar beziehungsweise rollbackfähig ausgeführt werden, sofern die beteiligten Provider dies unterstützen.

Fehlende Provider-Fähigkeiten müssen explizit sichtbar bleiben.

---

## Kompatibilität

POSIX-, Windows- oder andere Kompatibilitätsschichten dürfen ihre erwartete Dateisystemsemantik auf das NovaOS-VFS projizieren.

```text
Native Nova API
       ┐
POSIX  ├── VFS → Storage Providers
Win32  ┘
```

Kompatibilitätsanforderungen dürfen das native NovaOS-Modell jedoch nicht definieren.

---

## Architecture Introspection

Das VFS muss kontrolliert introspektierbar sein.

Mindestens relevant sind:

- Volumes,
- Mounts beziehungsweise Projektionen,
- Provider,
- unterstützte Fähigkeiten,
- Ressourcenidentität,
- Location,
- relevante Storage-Eigenschaften.

---

## Normative Anforderungen

1. NovaOS MUSS eine gemeinsame VFS-Abstraktion für dateibasierte Storage-Zugriffe bereitstellen.
2. Das VFS MUSS als Kompatibilitäts- und Projektionsschicht behandelt werden.
3. Das VFS DARF NICHT das vollständige native Daten- und Objektmodell von NovaOS definieren.
4. Unterschiedliche Dateisysteme und Storage-Provider MÜSSEN über gemeinsame VFS-Schnittstellen integrierbar sein.
5. NovaOS MUSS benannte beziehungsweise identitätsbasierte Volumes statt Laufwerksbuchstaben als natives Modell verwenden.
6. VFS-Nodes DÜRFEN Projektionen nichtklassischer Ressourcen darstellen.
7. Erweiterte NovaOS-Semantik DARF NICHT auf die kleinste gemeinsame Dateisystemfunktionalität reduziert werden.
8. Pfade und Namen DÜRFEN NICHT als Authority behandelt werden.
9. Security- und Data-Sovereignty-Regeln MÜSSEN auch über das VFS vollständig erhalten bleiben.
10. Location Transparency DARF relevante Kosten und Fehler nicht verbergen.
11. VFS-Operationen SOLLEN mit transaktionalen Systemoperationen integrierbar sein.
12. Kompatibilitätsschichten DÜRFEN fremde Dateisystemsemantiken projizieren, ohne das native NovaOS-Modell zu verändern.
13. Provider-Fähigkeiten und VFS-Projektionen MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Unterstützung bestehender Dateisysteme und Anwendungen,
- einheitlicher Namespace über unterschiedliche Storage-Provider,
- native NovaOS-Konzepte bleiben unabhängig vom klassischen Dateimodell,
- zukünftige Storage-Technologien können integriert werden,
- POSIX- und andere Kompatibilität kann oberhalb des nativen Modells realisiert werden.

### Negative Konsequenzen

- zusätzliche Projektionsschicht,
- nicht jede native NovaOS-Eigenschaft lässt sich verlustfrei auf klassische Dateisysteme abbilden,
- Provider besitzen unterschiedliche Fähigkeiten,
- Kompatibilitätssemantik kann zusätzliche Übersetzungen erfordern.

---

## Verworfene Alternativen

### Klassisches VFS als vollständiges Storage-Modell

Verworfen.

Dies würde NovaOS dauerhaft an das traditionelle Datei-/Verzeichnisparadigma binden.

### Kein VFS

Verworfen.

Bestehende Dateisysteme, Anwendungen und Kompatibilitäts-APIs benötigen eine gemeinsame Dateisystemabstraktion.

### Laufwerksbuchstaben als natives Volume-Modell

Verworfen.

Volumes werden über stabile Namen und Identitäten adressiert.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-ARCH-0003_Declarative_System_Model`
- `ADR-ARCH-0004_Transactionale_Systemoperationen`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0008_Semantic_Types`
- `ADR-ARCH-0009_Einheitliches_Daten_und_Objekt_Pipelining`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0014_Explicit_Contracts`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-VFS-0001`
- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-NAMESPACE-0001`
- `NPSPEC-STORAGE-PROVIDER-0001`
- `NPSPEC-STORAGE-PROJECTION-0001`
- `NPSPEC-STORAGE-VFS-TEST-0001`

---

## Ergebnis

Das NovaOS-VFS bildet eine Brücke zwischen klassischer Dateisystemsemantik und der umfassenderen nativen Storage-Architektur:

```text
Filesystem View
       ↓
      VFS
       ↓
Projection / Compatibility
       ↓
Native Storage and Object Model
       ↓
Storage Providers
```

Das VFS bleibt damit ein wichtiges Kompatibilitätswerkzeug, ohne zum fundamentalen Datenmodell von NovaOS zu werden.
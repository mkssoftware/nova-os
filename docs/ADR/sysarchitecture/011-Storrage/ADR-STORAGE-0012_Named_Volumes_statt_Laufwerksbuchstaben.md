# ADR-STORAGE-0012 – Named Volumes statt Laufwerksbuchstaben

## Status

Angenommen

## Kategorie

Storage / Volumes / Namespace / VFS

## Kontext

Laufwerksbuchstaben wie `C:`, `D:` oder `E:` sind ein historisches Adressierungsmodell, bei dem Storage-Ressourcen an kurze, kontextabhängige Bezeichner gebunden werden.

Für NovaOS ist dieses Modell ungeeignet. Volumes können lokal, entfernbar, virtuell, verschlüsselt, repliziert oder über unterschiedliche Storage Provider bereitgestellt werden.

NovaOS benötigt deshalb stabile und verständliche Volume-Identitäten, die nicht von der Reihenfolge erkannter Datenträger abhängen.

---

## Entscheidung

NovaOS verwendet **Named Volumes mit stabiler Volume Identity anstelle von Laufwerksbuchstaben**.

Beispiele:

```text
System:/
Benutzer:/
Daten:/
Recovery:/
```

Ein Volume besitzt dabei mindestens zwei getrennte Konzepte:

```text
Volume Identity
      ↓
Stable system identity

Volume Name
      ↓
Human-readable namespace name
```

Der Name dient der verständlichen Adressierung. Die stabile Volume Identity bleibt auch bei Umbenennung erhalten.

---

## Grundprinzip

```text
Identify storage by identity and meaning,
not by discovery order.
```

---

## Volume Identity

Jedes native Volume besitzt eine stabile Identität.

Diese darf nicht davon abhängen:

- an welchem Controller das Volume angeschlossen ist,
- in welcher Reihenfolge Geräte erkannt wurden,
- welcher Mount-Zeitpunkt vorliegt,
- welcher Name aktuell angezeigt wird.

Damit kann NovaOS ein Volume zuverlässig wiedererkennen.

---

## Volume Names

Volumes können verständliche Namen erhalten.

```text
System:/
Benutzer:/
Projekte:/
Medien:/
Backup:/
```

Volume Names sind Namespace-Bezeichner und keine physische Geräteadresse.

Namenskonflikte müssen durch die Storage-Namespace-Architektur eindeutig auflösbar sein.

---

## Verhältnis zum VFS

Named Volumes bilden Einstiegspunkte in den VFS-Namespace.

```text
Volume Identity
      ↓
Named Volume
      ↓
VFS Namespace
      ↓
Storage Objects
```

Das VFS projiziert die Inhalte des jeweiligen Volumes, während dessen tatsächlicher Storage Provider verborgen werden kann.

Relevante Location-, Availability- und Performance-Eigenschaften bleiben introspektierbar.

---

## Trennung von Volume und Device

Ein Volume ist nicht mit einem physischen Storage Device gleichzusetzen.

```text
Device ≠ Volume
```

Ein Device kann mehrere Volumes enthalten.

Umgekehrt kann ein logisches Volume abhängig von der späteren Storage-Architektur auf mehreren Devices oder Providern basieren.

Damit bleibt das Volume-Modell unabhängig von konkreten Partitionierungs- und Hardwaremodellen.

---

## System-Volumes

Bestimmte Rollen können durch wohldefinierte System-Volumes repräsentiert werden.

Beispiele:

```text
System:/
Benutzer:/
Boot:/
Recovery:/
```

Die Rolle eines Volumes und sein sichtbarer Name müssen dabei logisch unterscheidbar bleiben.

Eine lokalisierte Benutzeroberfläche darf beispielsweise einen anderen Anzeigenamen verwenden, ohne die stabile Systemidentität zu verändern.

---

## Wechselmedien

Entfernbare Datenträger erhalten ebenfalls stabile beziehungsweise aus ihren persistenten Metadaten ableitbare Volume Identities.

Ein erneutes Anschließen soll nicht dazu führen, dass sich die logische Identität allein aufgrund einer anderen Geräteerkennungsreihenfolge ändert.

---

## Kompatibilität

Kompatibilitätsschichten dürfen Laufwerksbuchstaben emulieren, wenn bestehende Software diese erwartet.

```text
Legacy Application
      ↓
C:\
      ↓
Compatibility Projection
      ↓
System:/
```

Diese Laufwerksbuchstaben sind ausschließlich Teil der jeweiligen Kompatibilitätsumgebung.

Sie werden nicht Bestandteil des nativen NovaOS-Storage-Modells.

---

## Security und Data Sovereignty

Ein Volume Name stellt keine Authority dar.

```text
Volume Name ≠ Permission
```

Der Zugriff auf ein benanntes Volume und seine Objekte unterliegt weiterhin den regulären Security-, Capability- und Data-Sovereignty-Regeln.

Auch remote oder virtuell bereitgestellte Named Volumes dürfen dadurch keine Sicherheitsgrenzen umgehen.

---

## Location Transparency

Named Volumes dürfen unterschiedliche Storage Locations repräsentieren.

Eine Anwendung kann daher dieselbe grundlegende Namespace-Semantik verwenden, unabhängig davon, ob ein Volume lokal oder über einen anderen Storage Provider bereitgestellt wird.

Location Transparency darf relevante Kosten, Offline-Zustände und Fehler nicht verschleiern.

---

## Architecture Introspection

NovaOS muss Volumes kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- Volume Identity,
- Volume Name,
- Systemrolle,
- Storage Provider,
- Location,
- Verfügbarkeit,
- relevante Capabilities.

---

## Normative Anforderungen

1. NovaOS MUSS Named Volumes als natives Volume-Modell verwenden.
2. Laufwerksbuchstaben DÜRFEN NICHT Bestandteil des nativen Storage-Namespace sein.
3. Jedes native Volume MUSS eine stabile Volume Identity besitzen.
4. Volume Identity und Volume Name MÜSSEN getrennte Konzepte sein.
5. Eine Umbenennung DARF die Volume Identity NICHT verändern.
6. Volume Identity DARF NICHT von Geräteerkennungs- oder Anschlussreihenfolge abhängen.
7. Volumes MÜSSEN unabhängig von physischen Storage Devices modelliert werden.
8. Named Volumes MÜSSEN als Einstiegspunkte in den VFS-Namespace verwendbar sein.
9. Systemrollen und benutzerseitige Anzeigenamen SOLLEN getrennt modellierbar sein.
10. Wechselmedien SOLLEN bei erneutem Anschluss stabil wiedererkennbar sein.
11. Kompatibilitätsschichten DÜRFEN Laufwerksbuchstaben emulieren.
12. Emulierte Laufwerksbuchstaben DÜRFEN das native Volume-Modell NICHT beeinflussen.
13. Volume Names DÜRFEN NICHT als Authority behandelt werden.
14. Volume Identity, Name, Rolle und Provider MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- stabile Storage-Identitäten,
- verständliche Pfade,
- keine Abhängigkeit von Geräteerkennungsreihenfolgen,
- bessere Unterstützung von Wechselmedien und virtuellen Volumes,
- saubere Trennung zwischen Hardware und Storage-Namespace,
- Laufwerksbuchstaben bleiben bei Bedarf kompatibel emulierbar.

### Negative Konsequenzen

- Namenskonflikte benötigen definierte Regeln,
- Legacy-Anwendungen benötigen gegebenenfalls eine Laufwerksbuchstaben-Projektion,
- Volume Identity, Systemrolle und Anzeigename müssen getrennt verwaltet werden.

---

## Verworfene Alternativen

### Laufwerksbuchstaben als natives Modell

Verworfen.

Sie sind nicht stabil genug und koppeln den Namespace unnötig an historische Kompatibilitätssemantik.

### Nur numerische Volume IDs

Verworfen.

Sie eignen sich für stabile Identität, sind aber als primäre menschliche Adressierung ungeeignet.

### Physisches Device als Volume Identity

Verworfen.

Volumes und physische Devices sind unterschiedliche Architekturkonzepte.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0013_Architecture_Introspection`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-VOLUME-0001`
- `NPSPEC-STORAGE-VOLUME-ID-0001`
- `NPSPEC-STORAGE-VOLUME-NAME-0001`
- `NPSPEC-STORAGE-VOLUME-NAMESPACE-0001`
- `NPSPEC-STORAGE-VOLUME-COMPAT-0001`
- `NPSPEC-STORAGE-VOLUME-TEST-0001`

---

## Ergebnis

NovaOS adressiert Volumes über stabile Identitäten und verständliche Namen:

```text
Stable Volume Identity
          ↓
     Named Volume
          ↓
       Daten:/
          ↓
     VFS Namespace
```

Laufwerksbuchstaben bleiben ausschließlich eine optionale Legacy-Projektion.

Die zentrale Architekturregel lautet:

```text
Volumes have identities and names.

They do not need letters.
```
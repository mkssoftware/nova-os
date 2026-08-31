# ADR-STORAGE-0017 – Stabile localeunabhängige Dateinamenidentität

## Status

Angenommen

## Kategorie

Storage / Naming / Identity / Localization / VFS

## Kontext

NovaOS verwendet stabile Object- und Volume-Identitäten, während Namen primär der menschlichen Navigation und Darstellung dienen.

Problematisch wird dies bei systemdefinierten Dateien, Verzeichnissen und Storage-Objekten, wenn deren Identität unmittelbar an einen lokalisierten Namen gekoppelt wird.

Beispielsweise können unterschiedliche Sprachumgebungen verschiedene sichtbare Namen verwenden:

```text
Dokumente
Documents
Documents
Documenti
```

Der sichtbare Name darf deshalb nicht gleichzeitig die stabile systeminterne Identität darstellen.

---

## Entscheidung

NovaOS trennt bei systemdefinierten Storage-Objekten die **stabile Namensidentität** vom **lokalisierten Anzeigenamen**.

```text
Stable Name Identity
        ↓
Localization
        ↓
Displayed Name
```

Systemkomponenten adressieren solche Objekte über stabile Identitäten. Benutzeroberflächen dürfen dagegen lokalisierte Namen darstellen.

---

## Grundprinzip

```text
Identity is stable.

Display names are localizable.
```

---

## Stabile Namensidentität

Systemrelevante Dateien, Verzeichnisse und Namespace-Einträge können eine stabile Namensidentität besitzen.

Diese bleibt unabhängig von:

- Systemsprache,
- Benutzeroberflächensprache,
- Region,
- Übersetzung,
- aktuellem Anzeigenamen.

Eine Änderung der Sprache verändert damit nicht die zugrunde liegende Storage-Identität.

---

## Verhältnis zur Object Identity

Die stabile Namensidentität ersetzt nicht die Object Identity.

```text
Object Identity
      ≠
Stable Name Identity
      ≠
Display Name
```

Die Object Identity identifiziert das konkrete Storage Object.

Die stabile Namensidentität beschreibt dessen definierte Rolle innerhalb eines Namespace.

Der Display Name dient ausschließlich der Darstellung für Benutzer.

---

## Lokalisierte Darstellung

Die Benutzeroberfläche darf eine stabile Namensidentität entsprechend der aktiven Sprache darstellen.

Beispiel:

```text
Stable Name Identity:
nova.user.documents

German:
Dokumente

English:
Documents
```

Eine Sprachänderung benötigt dadurch keine physische Umbenennung des Storage Objects.

---

## Benutzerdefinierte Namen

Die Trennung ist insbesondere für systemdefinierte oder semantisch bekannte Namespace-Einträge relevant.

Normale benutzerdefinierte Dateien benötigen nicht automatisch eine zusätzliche stabile Namensidentität.

```text
Urlaub 2026.jpg
Projekt NovaOS.nova
Notizen.txt
```

Ihre stabile Identifikation erfolgt weiterhin über das native Object-Storage-Modell.

---

## VFS und Legacy-Kompatibilität

Das VFS darf lokalisierte oder kompatibilitätsspezifische Namen projizieren, ohne die zugrunde liegende Identität zu verändern.

```text
Stable Namespace Identity
          ↓
        VFS
       /   \
Nova View   Legacy View
```

Kompatibilitätsschichten können dadurch erwartete Verzeichnisnamen bereitstellen, ohne das native Storage-Modell davon abhängig zu machen.

---

## Referenzen

Systemkomponenten, Capabilities und persistente interne Referenzen sollen keine lokalisierten Dateinamen verwenden, wenn eine stabile Identität verfügbar ist.

Dadurch bleiben Referenzen auch nach:

- Sprachwechsel,
- Umbenennung,
- Migration,
- Namespace-Projektion

gültig.

---

## Security

Ein stabiler Name ist keine Zugriffsberechtigung.

```text
Stable Name Identity ≠ Authority
```

Security- und Capability-Prüfungen müssen weiterhin auf den dafür vorgesehenen Identitäten und Authority-Mechanismen basieren.

---

## Normative Anforderungen

1. NovaOS MUSS stabile Namensidentitäten unabhängig von lokalisierten Anzeigenamen unterstützen.
2. Systemdefinierte Namespace-Einträge SOLLEN stabile Namensidentitäten verwenden.
3. Eine Änderung der Systemsprache DARF die stabile Namensidentität NICHT verändern.
4. Eine Lokalisierung DARF keine physische Umbenennung des zugrunde liegenden Storage Objects erfordern.
5. Object Identity, Stable Name Identity und Display Name MÜSSEN getrennte Konzepte bleiben.
6. Systemkomponenten SOLLEN stabile Identitäten statt lokalisierter Pfadnamen verwenden.
7. Persistente interne Referenzen SOLLEN nicht von der aktuellen UI-Sprache abhängen.
8. Das VFS DARF unterschiedliche Namen für dieselbe stabile Namespace-Identität projizieren.
9. Kompatibilitätsschichten DÜRFEN eigene Namensprojektionen bereitstellen.
10. Benutzerdefinierte Dateien MÜSSEN nicht automatisch eine separate stabile Namensidentität erhalten.
11. Eine stabile Namensidentität DARF NICHT als Authority behandelt werden.
12. Lokalisierung und Projektion DÜRFEN die zugrunde liegende Object Identity NICHT verändern.

---

## Konsequenzen

### Positive Konsequenzen

- Sprachwechsel ohne Umbenennung von Dateien oder Verzeichnissen,
- stabile interne Referenzen,
- saubere Trennung von Identität und Darstellung,
- bessere internationale Unterstützung,
- einfachere Legacy- und Namespace-Projektionen.

### Negative Konsequenzen

- zusätzliche Identitätsebene für systemdefinierte Namespace-Einträge,
- VFS und UI müssen Anzeigenamen auflösen,
- Debugging muss zwischen internem und sichtbarem Namen unterscheiden.

---

## Verworfene Alternativen

### Lokalisierte Namen als interne Identität

Verworfen.

Sprachänderungen würden Pfade und Referenzen verändern.

### Ausschließlich nicht lokalisierte englische Systemnamen

Verworfen.

Dies würde die technische Identität zwar stabilisieren, aber Darstellung und Identität unnötig miteinander koppeln.

### Object Identity gleichzeitig als sichtbaren Namen verwenden

Verworfen.

Technische Objektidentitäten sind nicht als benutzerfreundliche Namespace-Namen geeignet.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0001_VFS_als_Kompatibilitäts_und_Projektionsschicht`
- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0011_Legacy_Dateiprojektion_ohne_Duplikation`
- `ADR-STORAGE-0012_Named_Volumes_statt_Laufwerksbuchstaben`
- `ADR-ARCH-0007_Location_Transparency_mit_sichtbaren_Kosten_und_Fehlern`
- `ADR-ARCH-0019_Evolvable_System_Architecture`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-NAME-IDENTITY-0001`
- `NPSPEC-STORAGE-NAME-LOCALIZATION-0001`
- `NPSPEC-STORAGE-NAME-PROJECTION-0001`
- `NPSPEC-STORAGE-NAMESPACE-IDENTITY-0001`
- `NPSPEC-STORAGE-NAME-IDENTITY-TEST-0001`

---

## Ergebnis

NovaOS trennt die technische Identität eines systemdefinierten Namespace-Eintrags von seiner sprachabhängigen Darstellung:

```text
Object Identity
      ↓
Stable Name Identity
      ↓
Localized Display Name
```

Damit können Namen für Benutzer lokalisiert und projiziert werden, während systeminterne Referenzen stabil bleiben.

Die zentrale Architekturregel lautet:

```text
Localize the name.

Never localize the identity.
```
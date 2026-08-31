# ADR-STORAGE-0015 – Checksumming für Integrität

## Status

Angenommen

## Kategorie

Storage / Integrity / Checksumming / Data Protection

## Kontext

Persistente Daten können durch Hardwarefehler, Übertragungsfehler, fehlerhafte Firmware, Speicherdefekte oder Softwarefehler beschädigt werden.

Ein Storage-System darf daher nicht davon ausgehen, dass erfolgreich geschriebene Daten dauerhaft unverändert bleiben.

Content-Addressed Storage stellt bereits Integritätsinformationen für CAS-Inhalte bereit. NovaOS benötigt jedoch eine allgemeinere Integritätsarchitektur, die auch Daten und Strukturen außerhalb des CAS-Modells erfassen kann.

---

## Entscheidung

NovaOS verwendet **Checksumming als native Storage-Integritätsfunktion**.

Relevante persistente Daten und Storage-Metadaten können mit Integritätsprüfsummen geschützt werden.

```text
Data / Metadata
      ↓
   Checksum
      ↓
Store
      ↓
Read
      ↓
Verify
```

Eine erkannte Abweichung wird als Integritätsfehler behandelt und darf nicht stillschweigend ignoriert werden.

---

## Grundprinzip

```text
Stored does not mean intact.

Verify what matters.
```

---

## Daten und Metadaten

Checksumming soll sowohl Nutzdaten als auch kritische Storage-Metadaten schützen können.

```text
Storage Integrity
 ├── Payload Integrity
 └── Metadata Integrity
```

Insbesondere Metadaten über Objektidentitäten, Versionen, Transaktionen, Beziehungen und Storage-Strukturen dürfen nicht grundsätzlich ungeschützt bleiben.

---

## End-to-End-Integrität

Soweit möglich soll die Integritätsprüfung den Datenpfad über mehrere Storage-Schichten hinweg abdecken.

```text
Logical Data
     ↓
Storage Stack
     ↓
Provider
     ↓
Physical Storage
     ↓
Read Back
     ↓
Verification
```

Damit können Fehler erkannt werden, die unterhalb einer einzelnen Storage-Schicht entstehen.

---

## Verhältnis zu CAS

Content-Addressed Storage verwendet eine Content Identity, die gleichzeitig zur Integritätsprüfung eines immutable Inhalts verwendet werden kann.

CAS und allgemeines Checksumming bleiben dennoch getrennte Konzepte.

```text
CAS Digest
→ Content Identity + Integrity

Storage Checksum
→ Integrity
```

Nicht jede Storage-Prüfsumme muss gleichzeitig eine globale Content Identity darstellen.

---

## Algorithmusagilität

NovaOS legt auf ADR-Ebene keinen einzelnen Checksum- oder Hashalgorithmus dauerhaft fest.

Checksummen müssen algorithmusidentifizierbar und versionierbar sein.

Damit können zukünftige Verfahren eingeführt und veraltete Verfahren kontrolliert ersetzt werden.

Kryptographische Anforderungen werden dort verbindlich, wo Checksummen zusätzlich Bestandteil eines Security- oder Trust-Modells sind.

---

## Verifikation

Checksummen können abhängig von Storage Contract und Policy geprüft werden:

- beim Lesen,
- bei Scrubbing,
- bei Replikation,
- bei Migration,
- bei Recovery.

Kritische Daten dürfen stärkere Verifikationsanforderungen besitzen als temporäre oder rekonstruierbare Daten.

---

## Integritätsfehler

Ein Checksum-Mismatch muss als expliziter Integritätsfehler behandelt werden.

```text
Checksum Match
    ├── Yes → Data accepted
    └── No  → Integrity Fault
```

Beschädigte Daten dürfen nicht allein deshalb als gültig behandelt werden, weil sie technisch lesbar sind.

---

## Recovery

Wenn eine gültige alternative Kopie oder Version verfügbar ist, darf NovaOS diese zur Wiederherstellung verwenden.

Mögliche Quellen sind:

- Replikate,
- Snapshots,
- frühere Versionen,
- redundante Storage-Strukturen,
- rekonstruierbare Daten.

Checksumming erkennt dabei die Beschädigung; die konkrete Recovery-Strategie bleibt eine separate Storage-Funktion.

---

## Background Scrubbing

Storage Provider dürfen Daten periodisch oder policygesteuert überprüfen, um latente Beschädigungen vor dem eigentlichen Zugriff zu erkennen.

Scrubbing muss mit Resource Economy, Energieverbrauch und I/O-Auslastung koordiniert werden.

---

## Security und Trust

Checksumming schützt primär die Integrität gespeicherter Daten.

Eine gültige Checksum beweist nicht:

- die Herkunft eines Objekts,
- die Identität des Erstellers,
- dessen Vertrauenswürdigkeit,
- dessen Zugriffsberechtigung.

Dafür bleiben Provenance, Signaturen und Nova.Trust verantwortlich.

---

## Architecture Introspection

NovaOS muss den Integritätszustand kontrolliert introspektierbar machen.

Relevant sind insbesondere:

- verwendetes Checksum-Verfahren,
- Verifikationsstatus,
- erkannte Integritätsfehler,
- betroffener Storage Provider,
- Recovery-Status.

---

## Normative Anforderungen

1. NovaOS MUSS native Checksumming-Unterstützung für persistente Storage-Daten bereitstellen.
2. Kritische Storage-Metadaten MÜSSEN durch geeignete Integritätsmechanismen schützbar sein.
3. Checksummen MÜSSEN algorithmusidentifizierbar und versionierbar sein.
4. NovaOS DARF keinen einzelnen Checksum-Algorithmus dauerhaft als unveränderliche Architekturannahme voraussetzen.
5. Storage Contracts MÜSSEN festlegen können, welche Integritätsgarantien erforderlich sind.
6. Ein erkannter Checksum-Mismatch MUSS als Integritätsfehler behandelt werden.
7. Beschädigte Daten DÜRFEN NICHT stillschweigend als gültig akzeptiert werden.
8. CAS Content Digests DÜRFEN gleichzeitig zur Integritätsprüfung verwendet werden.
9. Allgemeine Storage Checksums MÜSSEN NICHT zwangsläufig Content Identities darstellen.
10. Checksummen SOLLEN bei Migration, Replikation und Recovery zur Verifikation verwendbar sein.
11. Storage Provider SOLLEN Background Scrubbing unterstützen können.
12. Scrubbing MUSS in die Resource Economy integrierbar sein.
13. Checksumming DARF Provenance, Signaturen oder Nova.Trust NICHT ersetzen.
14. Integritätsfehler und Verifikationszustände MÜSSEN kontrolliert introspektierbar sein.

---

## Konsequenzen

### Positive Konsequenzen

- Erkennung stiller Datenkorruption,
- Schutz kritischer Storage-Metadaten,
- bessere Grundlage für Self-Healing und Recovery,
- Integritätsprüfung bei Migration und Replikation,
- langfristige Algorithmusagilität.

### Negative Konsequenzen

- zusätzlicher Storage- und Rechenaufwand,
- Verifikation und Scrubbing verursachen I/O,
- erkannte Korruption benötigt eine separate Recovery-Strategie.

---

## Verworfene Alternativen

### Checksumming nur für CAS

Verworfen.

Auch nicht-CAS-basierte Daten und Storage-Metadaten benötigen Integritätsschutz.

### Ausschließlich Hardwarefehlerkorrektur verwenden

Verworfen.

Hardwaremechanismen schützen nicht den vollständigen End-to-End-Datenpfad.

### Eine feste Hashfunktion für alle Zeiten definieren

Verworfen.

Die Storage-Architektur muss auf zukünftige Algorithmen und Sicherheitsanforderungen reagieren können.

---

## Abhängigkeiten

Diese ADR steht insbesondere in Beziehung zu:

- `ADR-STORAGE-0002_Natives_Object_Storage_Modell`
- `ADR-STORAGE-0003_Content_Addressed_Storage`
- `ADR-STORAGE-0004_Transactional_Storage`
- `ADR-STORAGE-0005_Versioned_Storage`
- `ADR-STORAGE-0008_Data_Provenance_im_Storage_Modell`
- `ADR-STORAGE-0014_Snapshots_und_Rollback`
- `ADR-ARCH-0015_Graceful_Degradation`
- `ADR-ARCH-0016_Fail_Safe_Defaults`

---

## Zugehörige NPSPECs

- `NPSPEC-STORAGE-CHECKSUM-0001`
- `NPSPEC-STORAGE-CHECKSUM-METADATA-0001`
- `NPSPEC-STORAGE-INTEGRITY-VERIFY-0001`
- `NPSPEC-STORAGE-SCRUB-0001`
- `NPSPEC-STORAGE-INTEGRITY-RECOVERY-0001`
- `NPSPEC-STORAGE-CHECKSUM-TEST-0001`

---

## Ergebnis

NovaOS betrachtet Datenintegrität als überprüfbare Storage-Eigenschaft:

```text
Store
  ↓
Checksum
  ↓
Verify
  ↓
Valid
  or
Integrity Fault
```

Checksumming bildet damit die Erkennungsschicht für Storage-Korruption und liefert die Grundlage für Recovery und Self-Healing.

Die zentrale Architekturregel lautet:

```text
Do not assume integrity.

Verify it.
```
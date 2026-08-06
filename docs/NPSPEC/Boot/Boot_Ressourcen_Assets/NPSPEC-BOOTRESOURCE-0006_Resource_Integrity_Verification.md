# NPSPEC-BOOTRESOURCE-0006
# Resource Integrity Verification

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0006 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0004, NPSPEC-BOOTRESOURCE-0005, NPSPEC-BOOTSECURITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert das **Resource Integrity Verification System** der NovaOS-Bootoberfläche.

Das System stellt sicher, dass jede Ressource vor ihrer Verwendung vollständig auf Integrität, Konsistenz und Authentizität überprüft wird. Dadurch wird verhindert, dass beschädigte, manipulierte oder unvollständige Ressourcen in den Boot Manager geladen werden.

Die Integritätsprüfung erfolgt unabhängig vom Ressourcentyp und ist verpflichtender Bestandteil jedes Ladevorgangs.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- Icons
- Schriftarten
- Themes
- Sprachpakete
- Animationen
- Cursor
- Binärressourcen
- Boot Asset Packages

---

# 3. Ziele

Das Integritätssystem muss:

- jede Ressource validieren
- Manipulation erkennen
- Beschädigungen erkennen
- deterministisch arbeiten
- kryptografische Prüfsummen unterstützen
- digitale Signaturen berücksichtigen

---

# 4. Architektur

```text
Resource Request

↓

Resource Loader

↓

Integrity Verifier

├── Header Validator
├── Checksum Validator
├── Signature Validator
├── Dependency Validator
└── Format Validator

↓

Resource Registry

↓

Anwendung
```

---

# 5. Komponenten

Das System besteht aus:

- Integrity Manager
- Checksum Validator
- Signature Validator
- Header Validator
- Dependency Validator
- Version Validator
- Error Reporter

---

# 6. Grundprinzip

Vor jeder Registrierung erfolgt:

```text
Ressource

↓

Header prüfen

↓

Prüfsumme prüfen

↓

Signatur prüfen

↓

Version prüfen

↓

Abhängigkeiten prüfen

↓

Registrierung
```

Erst nach erfolgreicher Validierung darf eine Ressource verwendet werden.

---

# 7. Integritätsprüfungen

Für jede Ressource werden geprüft:

- Header
- Dateigröße
- Ressourcentyp
- Versionsnummer
- Prüfsumme
- Indexeintrag
- Metadaten

---

# 8. Prüfsummen

Unterstützt werden:

- CRC32 (schnelle Erkennung)
- SHA-256 (Standard)
- SHA-512 (optional)

Die verwendete Prüfsumme wird im Resource Index gespeichert.

---

# 9. Digitale Signaturen

Optional unterstützt:

- Ed25519
- ECDSA P-256

Signaturen schützen:

- Header
- Manifest
- Ressourcendaten
- Metadaten

Nicht signierte Ressourcen können abhängig von der Sicherheitsrichtlinie zugelassen oder abgelehnt werden.

---

# 10. Versionsprüfung

Vor dem Laden werden geprüft:

- Ressourcenversion
- Paketversion
- Formatversion
- Mindestversion des Boot Managers

Nicht kompatible Versionen dürfen nicht geladen werden.

---

# 11. Abhängigkeitsprüfung

Alle referenzierten Ressourcen müssen:

- vorhanden sein
- gültig sein
- kompatibel sein

Fehlende oder ungültige Abhängigkeiten führen zum Abbruch des Ladevorgangs.

---

# 12. Formatvalidierung

Je nach Ressourcentyp werden geprüft:

- Dateiformat
- Magic Number
- Headerstruktur
- interne Tabellen
- Größenangaben

Ungültige Formate werden verworfen.

---

# 13. Fehlerbehandlung

Bei einem Integritätsfehler:

```text
Fehler erkannt

↓

Ladevorgang abbrechen

↓

Fallback-Ressource

↓

Fehler protokollieren
```

Der Boot Manager bleibt weiterhin funktionsfähig.

---

# 14. Vertrauensmodell

Das System unterscheidet:

- vertrauenswürdig
- signiert
- unsigniert
- beschädigt
- unbekannt

Nur vertrauenswürdige Ressourcen dürfen ohne Warnung verwendet werden.

---

# 15. Sicherheitsrichtlinien

Unterstützt werden:

- permissiv
- standard
- strikt

Im Modus **strikt** werden ausschließlich gültig signierte Ressourcen akzeptiert.

---

# 16. Cacheintegration

Die Integritätsprüfung erfolgt nur beim ersten Laden.

Bereits validierte Ressourcen behalten ihren Vertrauensstatus bis zur Freigabe.

---

# 17. Speicherverwaltung

Das Integritätssystem verwendet:

- statische Prüfpuffer
- Hash-Kontexte
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(n)-Hash-Berechnung
- O(1)-Status-Lookup
- O(1)-Versionprüfung
- deterministische Laufzeit

Bereits validierte Ressourcen werden nicht erneut geprüft, solange sie im Cache verbleiben.

---

# 19. API

```c
typedef enum
{
    NOVA_RESOURCE_TRUST_UNKNOWN,
    NOVA_RESOURCE_TRUST_VALID,
    NOVA_RESOURCE_TRUST_SIGNED,
    NOVA_RESOURCE_TRUST_INVALID,
    NOVA_RESOURCE_TRUST_CORRUPTED
}
nova_resource_trust_t;

nova_result_t
nova_resource_integrity_initialize(void);

nova_result_t
nova_resource_verify(
    const nova_resource_descriptor_t* descriptor);

nova_resource_trust_t
nova_resource_trust_status(
    uint64_t resource_id);

bool
nova_resource_signature_valid(
    uint64_t resource_id);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Resource-ID
- Prüfsummenverfahren
- Signaturstatus
- Vertrauensstatus
- Validierungszeit
- Fehlercode
- Version
- Sicherheitsrichtlinie

---

# 21. Testfälle

Zu testen sind:

- gültige Ressourcen
- beschädigte Header
- falsche Prüfsummen
- ungültige Signaturen
- fehlende Abhängigkeiten
- Versionskonflikte
- verschiedene Sicherheitsrichtlinien
- Cache-Wiederverwendung
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Ressource vor der Verwendung validiert wird.
- Prüfsummen und Signaturen korrekt überprüft werden.
- beschädigte oder manipulierte Ressourcen abgelehnt werden.
- Sicherheitsrichtlinien korrekt angewendet werden.
- Abhängigkeiten vollständig geprüft werden.
- validierte Ressourcen im Cache nicht erneut geprüft werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0006 – Resource Integrity Verification
- ADR-BOOTRESOURCE-0004 – Resource Loading
- ADR-BOOTSECURITY-0001 – Secure Resource Validation
- ADR-BOOTRESOURCE-0002 – Boot Asset Package

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0004 – Resource Loading
- NPSPEC-BOOTRESOURCE-0005 – Resource Compression
- NPSPEC-BOOTSECURITY-0001 – Boot Security Framework

---

# 24. Zusammenfassung

Das **Resource Integrity Verification System** definiert die standardisierte Überprüfung sämtlicher Ressourcen innerhalb der NovaOS-Bootoberfläche. Es validiert Header, Prüfsummen, digitale Signaturen, Versionen und Abhängigkeiten, bevor eine Ressource registriert und verwendet werden darf. Durch deterministische Prüfabläufe, konfigurierbare Sicherheitsrichtlinien, statische Speicherverwaltung und Integration in den Resource Loader stellt das System sicher, dass ausschließlich unveränderte und vertrauenswürdige Ressourcen auf BIOS- und UEFI-basierten Systemen verwendet werden.
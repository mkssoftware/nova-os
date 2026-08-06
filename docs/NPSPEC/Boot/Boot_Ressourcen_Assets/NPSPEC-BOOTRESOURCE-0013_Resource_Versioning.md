# NPSPEC-BOOTRESOURCE-0013
# Resource Versioning

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0013 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0001, NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0006, NPSPEC-BOOTRESOURCE-0011 |

---

# 1. Ziel

Diese Spezifikation definiert das **Resource Versioning System** der NovaOS-Bootoberfläche.

Das System beschreibt die Versionierung sämtlicher Ressourcen innerhalb des Boot Managers. Jede Ressource besitzt eine eindeutige Versionsinformation, anhand derer Kompatibilität, Aktualisierungen und Integritätsprüfungen durchgeführt werden können.

Versionen werden ausschließlich durch den Resource Manager verwaltet.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- Icons
- Schriftarten
- Themes
- Animationen
- Sprachpakete
- Cursor
- Asset Packages
- sämtliche zukünftigen Ressourcentypen

---

# 3. Ziele

Das Versionierungssystem muss:

- eindeutige Versionsinformationen bereitstellen
- Kompatibilitätsprüfungen ermöglichen
- parallele Versionen unterstützen
- deterministisch arbeiten
- Rollbacks ermöglichen
- Erweiterbarkeit gewährleisten

---

# 4. Architektur

```text
Boot Asset Package

↓

Manifest

↓

Version Manager

↓

Compatibility Checker

↓

Resource Manager

↓

Anwendung
```

---

# 5. Komponenten

Das System besteht aus:

- Version Manager
- Compatibility Checker
- Version Registry
- Dependency Resolver
- Manifest Reader
- Validation Engine

---

# 6. Grundprinzip

Jede Ressource besitzt eine definierte Version.

```text
Resource

↓

Version

↓

Kompatibilitätsprüfung

↓

Registrierung
```

Nur kompatible Versionen dürfen geladen werden.

---

# 7. Versionsschema

Verwendet wird:

```text
Major.Minor.Patch.Build
```

Beispiel:

```text
2.5.1.18
```

Alle vier Komponenten sind numerisch.

---

# 8. Major-Version

Eine neue Major-Version bedeutet:

- inkompatible Änderungen
- neue Datenstrukturen
- geänderte Schnittstellen

Unterschiedliche Major-Versionen gelten standardmäßig als inkompatibel.

---

# 9. Minor-Version

Eine Minor-Version beschreibt:

- neue Funktionen
- Erweiterungen
- zusätzliche Ressourcen

Abwärtskompatibilität soll gewährleistet sein.

---

# 10. Patch-Version

Patch-Versionen enthalten:

- Fehlerkorrekturen
- Optimierungen
- keine strukturellen Änderungen

Patch-Versionen gelten als vollständig kompatibel.

---

# 11. Buildnummer

Die Buildnummer dient:

- internen Builds
- Fehleranalyse
- Reproduzierbarkeit

Sie beeinflusst die Kompatibilitätsentscheidung nicht.

---

# 12. Kompatibilität

Vor dem Laden wird geprüft:

- Ressourcenversion
- Paketversion
- Mindestversion
- Maximalversion

Nicht kompatible Ressourcen werden abgelehnt.

---

# 13. Abhängigkeiten

Ressourcen können Mindestversionen anderer Ressourcen definieren.

Beispiel:

```text
Theme

↓

Font >= 2.0

↓

Animation >= 1.5
```

Abhängigkeiten werden vollständig geprüft.

---

# 14. Rollback

Mehrere Versionen derselben Ressource können vorhanden sein.

Der Resource Manager kann auf eine ältere kompatible Version zurückfallen.

---

# 15. Themeintegration

Themes können explizit Versionen referenzieren.

Fehlt die angeforderte Version, erfolgt eine Kompatibilitätsprüfung.

---

# 16. Sicherheit

Das System verhindert:

- Versionskonflikte
- manipulierte Versionsnummern
- doppelte Versionseinträge
- inkonsistente Abhängigkeiten
- ungültige Manifestinformationen

Alle Versionsinformationen werden validiert.

---

# 17. Speicherverwaltung

Das Versionierungssystem verwendet:

- statische Versionstabellen
- Registry-Pools
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 18. Performance

Anforderungen:

- O(1)-Versions-Lookup
- O(1)-Kompatibilitätsprüfung
- O(n)-Abhängigkeitsprüfung
- deterministische Laufzeit

Versionstabellen werden beim Laden eines Asset Packages vorbereitet.

---

# 19. API

```c
typedef struct
{
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t build;
}
nova_resource_version_t;

nova_result_t
nova_resource_version_initialize(void);

bool
nova_resource_version_compatible(
    const nova_resource_version_t* required,
    const nova_resource_version_t* available);

const nova_resource_version_t*
nova_resource_version(
    uint64_t resource_id);
```

---

# 20. Diagnosedaten

Zu erfassen sind:

- Resource-ID
- Version
- Mindestversion
- Maximalversion
- Kompatibilitätsstatus
- Versionskonflikte
- Rollbacks
- Fehler

---

# 21. Testfälle

Zu testen sind:

- Major-Versionen
- Minor-Versionen
- Patch-Versionen
- Buildnummern
- Rollbacks
- Versionskonflikte
- Abhängigkeiten
- beschädigte Manifestdaten
- Fehlerfälle

---

# 22. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- jede Ressource eine vollständige Versionsinformation besitzt.
- inkompatible Versionen erkannt werden.
- Rollbacks unterstützt werden.
- Abhängigkeiten anhand der Versionen geprüft werden.
- Themeversionen korrekt verarbeitet werden.
- keine ungültigen Versionen akzeptiert werden.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 23. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0013 – Resource Versioning
- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTRESOURCE-0006 – Resource Integrity Verification
- ADR-BOOTTHEME-0001 – Theme System

## NPSPECs

- NPSPEC-BOOTRESOURCE-0001 – Boot Resource Manager
- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0006 – Resource Integrity Verification
- NPSPEC-BOOTRESOURCE-0011 – Theme Resources

---

# 24. Zusammenfassung

Das **Resource Versioning System** definiert die standardisierte Versionierung sämtlicher Ressourcen innerhalb der NovaOS-Bootoberfläche. Es verwaltet Versionsnummern, prüft Kompatibilitäten und Abhängigkeiten und unterstützt parallele Versionen sowie Rollback-Szenarien. Durch deterministische Versionsprüfung, zentrale Verwaltung und statische Speicherstrukturen gewährleistet das System eine konsistente und sichere Nutzung versionierter Ressourcen auf BIOS- und UEFI-basierten Systemen.
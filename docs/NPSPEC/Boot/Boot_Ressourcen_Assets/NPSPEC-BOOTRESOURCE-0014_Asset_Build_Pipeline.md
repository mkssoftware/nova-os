# NPSPEC-BOOTRESOURCE-0014
# Asset Build Pipeline

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTRESOURCE-0014 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Resource System |
| Priorität | Hoch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTRESOURCE-0002, NPSPEC-BOOTRESOURCE-0003, NPSPEC-BOOTRESOURCE-0005, NPSPEC-BOOTRESOURCE-0006, NPSPEC-BOOTRESOURCE-0013, NPSPEC-BOOTSECURITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Asset Build Pipeline** der NovaOS-Bootoberfläche.

Die Asset Build Pipeline beschreibt den vollständigen Prozess zur Erstellung von Boot Asset Packages aus den Quelldateien. Während des Build-Prozesses werden Ressourcen validiert, optimiert, versioniert, optional komprimiert, kryptographisch abgesichert und zu einem standardisierten Boot Asset Package zusammengeführt.

Die Pipeline wird ausschließlich während der Erstellung eines Releases oder einer Entwicklungsbuild ausgeführt und ist nicht Bestandteil der Laufzeitumgebung.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Bilder
- SVG-Icons
- Schriftarten
- Themes
- Animationen
- Sprachpakete
- Cursor
- Konfigurationsdateien
- Boot Asset Packages

---

# 3. Ziele

Die Asset Build Pipeline muss:

- reproduzierbare Builds erzeugen
- Ressourcen validieren
- Versionen vergeben
- Asset Packages erzeugen
- Integrität sicherstellen
- automatisierbar sein

---

# 4. Architektur

```text
Quellressourcen

↓

Validator

↓

Konverter

↓

Optimierer

↓

Versionierer

↓

Komprimierung

↓

Manifest Generator

↓

Resource Index Generator

↓

Signierung

↓

Boot Asset Package
```

---

# 5. Komponenten

Die Pipeline besteht aus:

- Asset Scanner
- Resource Validator
- Format Converter
- Optimizer
- Version Manager
- Compression Engine
- Manifest Generator
- Index Generator
- Signature Generator
- Package Builder

---

# 6. Grundprinzip

Der Buildprozess erfolgt in einer festen Reihenfolge:

```text
Dateien

↓

Prüfen

↓

Optimieren

↓

Versionieren

↓

Paket erzeugen

↓

Signieren
```

Jeder Schritt besitzt definierte Ein- und Ausgaben.

---

# 7. Ressourcenerkennung

Der Asset Scanner erkennt automatisch:

- PNG
- SVG
- Fonts
- Theme-Dateien
- Sprachdateien
- Animationen
- Cursor
- Binärdateien

Nicht unterstützte Formate führen zu einer Build-Warnung oder einem Fehler.

---

# 8. Validierung

Vor der weiteren Verarbeitung werden geprüft:

- Dateiformat
- Dateigröße
- Header
- Integrität
- Unicode-Konsistenz
- Ressourcentyp

Ungültige Ressourcen werden nicht in das Paket aufgenommen.

---

# 9. Optimierung

Optional werden durchgeführt:

- PNG-Optimierung
- SVG-Bereinigung
- Font-Subset-Erzeugung
- Entfernung ungenutzter Metadaten
- Sortierung von Ressourcen

Die Optimierung darf die Funktionalität nicht verändern.

---

# 10. Versionierung

Für jede Ressource werden erzeugt:

- Resource-ID
- Versionsnummer
- Buildnummer

Zusätzlich erhält jedes Asset Package eine eigene Versionsinformation.

---

# 11. Komprimierung

Abhängig vom Ressourcentyp können verwendet werden:

- keine Komprimierung
- LZ4
- Zstandard

Das verwendete Verfahren wird im Resource Index gespeichert.

---

# 12. Manifest

Die Pipeline erzeugt automatisch ein Manifest mit:

- Paketinformationen
- Versionsinformationen
- Ressourcenliste
- Abhängigkeiten
- Mindestversionen
- Prüfsummen

Das Manifest ist Bestandteil jedes Asset Packages.

---

# 13. Resource Index

Für jede Ressource wird ein Indexeintrag erzeugt.

Dieser enthält:

- Resource-ID
- Offset
- Größe
- Prüfsumme
- Komprimierungsverfahren
- Versionsnummer

---

# 14. Signierung

Optional wird das fertige Paket digital signiert.

Unterstützt werden:

- Ed25519
- ECDSA P-256

Die Signatur schützt das vollständige Asset Package.

---

# 15. Reproduzierbarkeit

Die Pipeline muss deterministische Ergebnisse erzeugen.

Bei identischen Eingabedaten entstehen:

- identische Resource-IDs
- identische Offsets
- identische Prüfsummen
- identische Pakete

Zeitstempel oder zufällige Werte dürfen das Ergebnis nicht beeinflussen.

---

# 16. Buildprofile

Unterstützt werden:

- Debug
- Development
- Release

Buildprofile können Komprimierung, Optimierungen und Signierung beeinflussen.

---

# 17. Sicherheit

Die Pipeline verhindert:

- doppelte Resource-IDs
- beschädigte Dateien
- ungültige Versionen
- fehlerhafte Manifeste
- manipulierte Paketinhalte

Alle Schritte werden vor dem Paketabschluss validiert.

---

# 18. Speicherverwaltung

Die Pipeline verwendet:

- temporäre Build-Puffer
- Arbeitsverzeichnisse
- Streaming-Verarbeitung großer Dateien

Der Speicherverbrauch soll unabhängig von der Gesamtgröße des Asset Packages skalieren.

---

# 19. Performance

Anforderungen:

- parallele Verarbeitung unabhängiger Ressourcen (optional)
- O(n)-Dateiverarbeitung
- deterministische Build-Reihenfolge
- inkrementelle Builds (optional)

Die Pipeline soll auch große Ressourcenmengen effizient verarbeiten.

---

# 20. API

```c
typedef struct
{
    const char* input_directory;
    const char* output_package;
    const char* build_profile;
}
nova_asset_build_config_t;

nova_result_t
nova_asset_build_initialize(void);

nova_result_t
nova_asset_build_execute(
    const nova_asset_build_config_t* config);

bool
nova_asset_build_validate(
    const char* input_directory);

nova_result_t
nova_asset_build_sign(
    const char* package_path);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Anzahl Ressourcen
- Builddauer
- Optimierungen
- Komprimierungsverhältnis
- Signaturstatus
- Warnungen
- Fehler
- erzeugte Paketgröße

---

# 22. Testfälle

Zu testen sind:

- vollständiger Build
- fehlerhafte Ressourcen
- doppelte IDs
- beschädigte Dateien
- verschiedene Buildprofile
- PNG-Optimierung
- SVG-Optimierung
- Signierung
- reproduzierbare Builds
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- aus gültigen Quellen ein vollständiges Boot Asset Package erzeugt wird.
- sämtliche Ressourcen validiert werden.
- Manifest und Resource Index automatisch erstellt werden.
- reproduzierbare Builds möglich sind.
- optionale Komprimierung und Signierung korrekt funktionieren.
- fehlerhafte Ressourcen den Build eindeutig kennzeichnen.
- identische Eingaben identische Ausgaben erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTRESOURCE-0014 – Asset Build Pipeline
- ADR-BOOTRESOURCE-0002 – Boot Asset Package
- ADR-BOOTRESOURCE-0006 – Resource Integrity Verification
- ADR-BOOTSECURITY-0001 – Secure Resource Validation

## NPSPECs

- NPSPEC-BOOTRESOURCE-0002 – Boot Asset Package
- NPSPEC-BOOTRESOURCE-0003 – Resource Index
- NPSPEC-BOOTRESOURCE-0005 – Resource Compression
- NPSPEC-BOOTRESOURCE-0006 – Resource Integrity Verification
- NPSPEC-BOOTRESOURCE-0013 – Resource Versioning
- NPSPEC-BOOTSECURITY-0001 – Boot Security Framework

---

# 25. Zusammenfassung

Die **Asset Build Pipeline** definiert den standardisierten Erstellungsprozess für Boot Asset Packages der NovaOS-Bootoberfläche. Sie übernimmt die Validierung, Optimierung, Versionierung, Komprimierung, Manifest- und Indexerstellung sowie die optionale digitale Signierung aller Ressourcen. Durch reproduzierbare Builds, deterministische Verarbeitung und automatische Qualitätssicherung stellt die Pipeline sicher, dass ausschließlich konsistente, überprüfbare und effizient strukturierte Ressourcenpakete für BIOS- und UEFI-basierte Bootumgebungen erzeugt werden.
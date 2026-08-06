# NPSPEC-BOOTTEST-0006
# Resolution Compatibility Tests

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEST-0006 |
| Titel | Resolution Compatibility Tests |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot Testing |
| Priorität | Kritisch |
| Gültigkeitsbereich | Gesamte NovaOS Boot UI |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEST-0001, NPSPEC-BOOTLAYOUT-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTDESIGN-0001, NPSPEC-BOOTPERF-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **Resolution Compatibility Tests** der NovaOS Bootoberfläche.

Die Tests überprüfen, dass sämtliche Benutzeroberflächen auf allen unterstützten Bildschirmauflösungen, Seitenverhältnissen und Skalierungsstufen korrekt dargestellt werden. Ziel ist eine vollständig responsive und deterministische Darstellung ohne Layoutfehler, abgeschnittene Inhalte oder unzugängliche Bedienelemente.

---

# 2. Geltungsbereich

Diese Spezifikation gilt für:

- Boot Manager
- Recovery
- Installer
- Self-Healing
- Diagnoseoberflächen
- Dialogsystem
- sämtliche Boot Controls
- sämtliche Themes

---

# 3. Ziele

Die Tests müssen:

- sämtliche Auflösungen validieren
- Layoutfehler erkennen
- Skalierungsprobleme identifizieren
- unterschiedliche Seitenverhältnisse unterstützen
- automatisierbar sein
- reproduzierbare Ergebnisse liefern

---

# 4. Testarchitektur

Das Testsystem besteht aus:

- Resolution Manager
- Display Simulator
- Layout Validator
- Screenshot Generator
- Difference Analyzer
- Report Generator

Alle Komponenten arbeiten unabhängig von der Rendering Engine.

---

# 5. Unterstützte Auflösungen

Mindestens folgende Auflösungen werden getestet:

| Auflösung | Seitenverhältnis |
|-----------|------------------|
| 640×480 | 4:3 |
| 800×600 | 4:3 |
| 1024×768 | 4:3 |
| 1280×720 | 16:9 |
| 1280×800 | 16:10 |
| 1366×768 | 16:9 |
| 1600×900 | 16:9 |
| 1920×1080 | 16:9 |
| 2560×1440 | 16:9 |
| 3840×2160 | 16:9 |

---

# 6. Seitenverhältnisse

Zu validieren sind:

- 4:3
- 5:4
- 16:9
- 16:10
- 21:9
- Ultra-Wide

---

# 7. Skalierungsstufen

Unterstützte Skalierungen:

- 100 %
- 125 %
- 150 %
- 175 %
- 200 %

Alle Layouts müssen ohne Informationsverlust funktionieren.

---

# 8. Testablauf

Standardablauf:

1. Testumgebung initialisieren
2. Auflösung setzen
3. Theme laden
4. Oberfläche rendern
5. Layout prüfen
6. Screenshot erzeugen
7. Referenzvergleich durchführen
8. Ergebnis protokollieren

---

# 9. Layoutvalidierung

Zu prüfen sind:

- Positionierung
- Abstände
- Größen
- Ausrichtung
- Sichtbarkeit
- Clipping
- Überlappungen

---

# 10. Control-Validierung

Für jedes Control werden geprüft:

- Mindestgröße
- Maximalgröße
- Lesbarkeit
- Fokusrahmen
- Erreichbarkeit
- Klickbereich

---

# 11. Textvalidierung

Zu prüfen sind:

- Zeilenumbrüche
- Ellipsis
- Unicode-Darstellung
- Skalierung
- Fontgrößen
- Lesbarkeit

---

# 12. Dialogvalidierung

Alle Dialoge müssen:

- vollständig sichtbar sein
- korrekt skaliert werden
- keinen Inhalt abschneiden
- vollständig bedienbar bleiben

---

# 13. Navigation

Zu validieren sind:

- Tastaturfokus
- Scrollbereiche
- Listen
- Menüs
- Dialognavigation

Alle Bedienelemente müssen erreichbar bleiben.

---

# 14. Grafikvalidierung

Geprüft werden:

- PNG
- SVG
- Icons
- Schatten
- Glasmaterial
- Farbverläufe
- Transparenzen

---

# 15. Performance

Für jede Auflösung werden gemessen:

- FPS
- Framezeit
- Speicherverbrauch
- Renderzeit
- Layoutzeit

---

# 16. Fehlererkennung

Das Framework erkennt:

- abgeschnittene Controls
- überlappende Controls
- falsche Skalierung
- Layoutfehler
- Textüberlauf
- fehlende Scrollbereiche
- Clipping

---

# 17. Automatisierung

Alle Tests müssen:

- skriptgesteuert
- unbeaufsichtigt
- reproduzierbar
- CI-kompatibel

sein.

---

# 18. Regression

Resolution Compatibility Tests werden automatisch ausgeführt bei Änderungen an:

- Layout Engine
- Rendering Engine
- Themes
- Controls
- Typografie
- Skalierungslogik

---

# 19. Sicherheit

Die Tests dürfen:

- keine Bootkonfiguration verändern
- keine Ressourcen überschreiben
- keine Benutzerdaten speichern

Alle Tests erfolgen in einer isolierten Testumgebung.

---

# 20. Speicherverwaltung

Das Testsystem verwendet:

- Screenshot Pool
- Layout Buffer
- Validation Buffer
- Report Buffer

Alle Speicherbereiche werden vor Testbeginn reserviert.

Heap-Allokationen während einzelner Testschritte sind nicht zulässig.

---

# 21. API

```c
typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t scale_percent;
    bool layout_valid;
    bool rendering_valid;
    uint32_t detected_errors;
}
nova_resolution_test_result_t;

nova_result_t
nova_resolution_test_initialize(void);

nova_result_t
nova_resolution_test_execute(
    uint32_t width,
    uint32_t height,
    uint32_t scale_percent);

const nova_resolution_test_result_t*
nova_resolution_test_results(void);

nova_result_t
nova_resolution_test_generate_report(void);
```

---

# 22. Diagnosedaten

Zu protokollieren sind:

- Auflösung
- Seitenverhältnis
- Skalierungsstufe
- Theme
- Layoutfehler
- Renderfehler
- FPS
- Framezeit
- Screenshots
- Vergleichsergebnisse

---

# 23. Testfälle

Zu testen sind:

- sämtliche unterstützten Auflösungen
- Ultra-Wide
- High-DPI
- Themewechsel
- Dialoge
- Listen
- Boot Manager
- Recovery
- Installer
- Accessibility

---

# 24. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- sämtliche unterstützten Auflösungen erfolgreich validiert werden.
- keine Layoutfehler oder abgeschnittenen Inhalte auftreten.
- alle Bedienelemente vollständig erreichbar bleiben.
- Referenzvergleiche innerhalb der definierten Toleranzen liegen.
- Performancewerte innerhalb der definierten Budgets bleiben.
- Regressionen automatisch erkannt werden.
- vollständige Testberichte erzeugt werden.
- sämtliche Ergebnisse über die Test-API verfügbar sind.

---

# 25. Referenzen

## ADRs

- ADR-BOOTTEST-0006 – Resolution Compatibility Tests
- ADR-BOOTLAYOUT-0001 – Responsive Layout Engine
- ADR-BOOTRENDER-0001 – Rendering Engine

## NPSPECs

- NPSPEC-BOOTTEST-0001 – Boot UI Test Architecture
- NPSPEC-BOOTLAYOUT-0001 – Layout Engine
- NPSPEC-BOOTRENDER-0001 – Rendering Engine
- NPSPEC-BOOTDESIGN-0001 – Boot Design Language
- NPSPEC-BOOTPERF-0001 – Boot UI Performance Requirements

---

# 26. Zusammenfassung

Die **Resolution Compatibility Tests** stellen sicher, dass die NovaOS-Bootoberfläche auf allen unterstützten Bildschirmauflösungen, Seitenverhältnissen und Skalierungsstufen konsistent dargestellt wird. Das Testframework überprüft Layout, Typografie, Grafiken, Navigation und Performance automatisiert und erkennt zuverlässig Darstellungsfehler, Skalierungsprobleme oder Regressionen. Dadurch wird eine vollständig responsive und hardwareunabhängige Benutzeroberfläche für die gesamte Bootumgebung gewährleistet.
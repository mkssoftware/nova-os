# NPSPEC-BOOTTEXT-0002
# UTF-8 Text Processing

| Feld | Wert |
|------|------|
| Dokument-ID | NPSPEC-BOOTTEXT-0002 |
| Version | 1.0 |
| Status | Draft |
| Kategorie | Boot UI – Text System |
| Priorität | Kritisch |
| Gültigkeitsbereich | NovaOS Boot Manager |
| Autor | NovaOS Architecture |
| Abhängigkeiten | NPSPEC-BOOTTEXT-0001, NPSPEC-BOOTFONT-0001, NPSPEC-BOOTRENDER-0001, NPSPEC-BOOTACCESSIBILITY-0001 |

---

# 1. Ziel

Diese Spezifikation definiert die **UTF-8 Text Processing Engine** der NovaOS-Bootoberfläche.

Die UTF-8 Text Processing Engine übernimmt das Dekodieren, Validieren und Normalisieren sämtlicher UTF-8-Zeichenketten innerhalb der Bootumgebung. Sie stellt sicher, dass alle Textdaten unabhängig von Sprache oder Zeichensatz konsistent verarbeitet werden.

UTF-8 ist die einzige externe Textkodierung innerhalb der Bootoberfläche.

---

# 2. Geltungsbereich

Die Spezifikation gilt für:

- UI-Texte
- Dialoge
- Menüs
- Labels
- Buttons
- Statusmeldungen
- Recovery
- Installer
- Fehlermeldungen
- Lokalisierungsdateien

---

# 3. Ziele

Das System muss:

- UTF-8 vollständig unterstützen
- Unicode korrekt dekodieren
- ungültige Sequenzen erkennen
- deterministisch arbeiten
- sprachunabhängig sein
- hohe Performance bieten

---

# 4. Architektur

```text
UTF-8 Input

↓

UTF-8 Decoder

↓

Unicode Validator

↓

Normalizer

↓

Code Point Stream

↓

Text Layout

↓

Renderer
```

---

# 5. Komponenten

Die UTF-8-Verarbeitung besteht aus:

- UTF-8 Decoder
- Unicode Validator
- Normalizer
- Code Point Reader
- Error Handler
- Text Stream Generator

---

# 6. Grundprinzip

Alle Texte werden ausschließlich als UTF-8 gespeichert.

Verarbeitung:

```text
UTF-8

↓

Unicode Code Points

↓

Layout

↓

Glyph Lookup

↓

Rendering
```

Interne Komponenten arbeiten ausschließlich mit Unicode-Codepunkten.

---

# 7. Unterstützte Kodierung

Unterstützt wird:

```text
UTF-8
```

Nicht unterstützt:

- ANSI
- ISO-8859
- UTF-7
- UCS-2

UTF-16 darf ausschließlich intern als optionale Optimierung verwendet werden.

---

# 8. Dekodierung

Unterstützt werden:

- 1-Byte-Sequenzen
- 2-Byte-Sequenzen
- 3-Byte-Sequenzen
- 4-Byte-Sequenzen

Ungültige Bytefolgen werden erkannt.

---

# 9. Unicode

Unterstützt werden:

- Basic Multilingual Plane
- Supplementary Planes (optional)

Jeder Code Point besitzt:

- Unicode-Wert
- Kategorie
- Gültigkeitsstatus

---

# 10. Normalisierung

Optional unterstützt:

- NFC
- NFD

Standardmäßig verwendet das System NFC.

Alle Lokalisierungen müssen in NFC gespeichert werden.

---

# 11. Fehlerbehandlung

Ungültige UTF-8-Sequenzen werden:

- erkannt
- protokolliert
- ersetzt

Standardzeichen:

```text
U+FFFD
```

Die Verarbeitung wird niemals abgebrochen.

---

# 12. Zeichenklassen

Die Engine unterscheidet:

- Buchstaben
- Zahlen
- Satzzeichen
- Leerzeichen
- Steuerzeichen
- Symbole

Die Klassifizierung basiert ausschließlich auf Unicode.

---

# 13. Zeilenenden

Unterstützt werden:

```text
LF

CRLF

CR
```

Intern erfolgt die Normalisierung auf:

```text
LF
```

---

# 14. Byte Order Mark

Unterstützt wird:

```text
UTF-8 BOM
```

Der BOM wird beim Einlesen automatisch entfernt.

---

# 15. Internationalisierung

Unterstützt werden:

- Deutsch
- Englisch
- Französisch
- Spanisch
- Italienisch
- Japanisch
- Chinesisch

Weitere Sprachen können ohne Änderungen an der Engine ergänzt werden.

---

# 16. Accessibility

Alle Textdaten bleiben nach der UTF-8-Verarbeitung vollständig für:

- Screen Reader
- Braille-Ausgabe
- Sprachsynthese

verfügbar.

---

# 17. Sicherheit

Das System verhindert:

- überlange UTF-8-Sequenzen
- ungültige Unicode-Bereiche
- abgeschnittene Sequenzen
- Buffer Overflows
- rekursive Dekodierung

Alle Eingaben werden vollständig validiert.

---

# 18. Speicherverwaltung

Das UTF-8-System verwendet:

- statische Decoder
- feste Code-Point-Puffer
- Objektpools

Heap-Allokationen während der Laufzeit sind verboten.

---

# 19. Performance

Anforderungen:

- O(n)-Dekodierung
- O(n)-Validierung
- O(n)-Normalisierung
- deterministische Laufzeit

Die Laufzeit ist ausschließlich von der Anzahl der Eingabebytes abhängig.

---

# 20. API

```c
typedef struct
{
    uint32_t codepoint;
    bool valid;
}
nova_unicode_character_t;

nova_result_t
nova_utf8_initialize(void);

nova_result_t
nova_utf8_decode(
    const char* utf8,
    nova_unicode_character_t* output,
    size_t capacity);

bool
nova_utf8_validate(
    const char* utf8);

nova_result_t
nova_utf8_normalize(
    const char* utf8,
    char* output,
    size_t capacity);
```

---

# 21. Diagnosedaten

Zu erfassen sind:

- Anzahl Code Points
- Dekodierungszeit
- Unicode-Fehler
- Ersetzungen
- Normalisierung
- Speicherverbrauch

---

# 22. Testfälle

Zu testen sind:

- ASCII
- UTF-8
- 2-Byte-Sequenzen
- 3-Byte-Sequenzen
- 4-Byte-Sequenzen
- BOM
- Unicode-Fehler
- NFC
- Internationalisierung
- Fehlerfälle

---

# 23. Akzeptanzkriterien

Die Spezifikation gilt als erfüllt, wenn:

- UTF-8 vollständig unterstützt wird.
- sämtliche Unicode-Codepunkte korrekt dekodiert werden.
- ungültige Sequenzen erkannt und ersetzt werden.
- ausschließlich UTF-8 als externe Kodierung verwendet wird.
- NFC standardmäßig eingesetzt wird.
- Accessibility vollständig erhalten bleibt.
- keine Heap-Allokationen während der Laufzeit stattfinden.
- BIOS- und UEFI-Systeme identisches Verhalten erzeugen.

---

# 24. Referenzen

## ADRs

- ADR-BOOTTEXT-0002 – UTF-8 Text Processing
- ADR-BOOTTEXT-0001 – Boot Text Engine
- ADR-BOOTFONT-0001 – Font Management
- ADR-BOOTACCESS-0001 – Accessibility

## NPSPECs

- NPSPEC-BOOTTEXT-0001 – Boot Text Engine
- NPSPEC-BOOTFONT-0001 – Font Management
- NPSPEC-BOOTRENDER-0001 – Boot Rendering Engine
- NPSPEC-BOOTACCESSIBILITY-0001 – Accessibility Framework

---

# 25. Zusammenfassung

Die **UTF-8 Text Processing Engine** definiert die standardisierte Verarbeitung sämtlicher Textdaten innerhalb der NovaOS-Bootoberfläche. Sie dekodiert UTF-8, validiert Unicode-Sequenzen, normalisiert Texte im NFC-Format und erzeugt einen konsistenten Unicode-Codepunktstrom für die nachgelagerten Layout- und Rendering-Komponenten. Durch deterministische Algorithmen, robuste Fehlerbehandlung und statische Speicherverwaltung bildet sie die Grundlage für eine zuverlässige, internationale und sichere Textverarbeitung auf BIOS- und UEFI-basierten Systemen.
# NPSPEC-UNITS-0003
# Unit Registry and Conversion Engine

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0003 |
| Titel | Unit Registry and Conversion Engine |
| Version | 1.0.0 |
| Teil | 2 von 6 |
| Kapitel | 11–20 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 11. Registry Lifecycle

Die Registry besitzt folgende Lebenszyklusphasen:

1. Boot Registry
2. Core Registry Initialisierung
3. Laden der signierten System-Registry
4. Laden optionaler Erweiterungen
5. Runtime-Betrieb
6. Schreibgeschützter Shutdown
7. Persistierung

Während der Bootphase sind ausschließlich schreibgeschützte Systemeinträge verfügbar.

---

# 12. Registry Storage Model

Die Registry wird logisch in mehrere Tabellen unterteilt.

```text
Registry
├── Unit Table
├── Dimension Table
├── Prefix Table
├── Namespace Table
├── Alias Table
├── Constant Table
└── Metadata Table
```

Alle Tabellen besitzen:

- Versionsnummer
- Prüfsumme
- Signatur
- Änderungszeitpunkt
- Eigentümer

---

# 13. System Registry Loading

Die System-Registry wird während des Systemstarts geladen.

Reihenfolge:

1. Signatur prüfen
2. Formatversion prüfen
3. Prüfsumme prüfen
4. Tabellen laden
5. Handles erzeugen
6. Cache initialisieren
7. Registry freigeben

Schlägt einer dieser Schritte fehl, darf Nova ausschließlich mit der minimalen Boot-Registry starten.

---

# 14. Unit Registration

Neue Einheiten werden ausschließlich über die öffentliche Registry-API registriert.

```c
nova_result_t nova_unit_register(
    const nova_unit_descriptor_t* descriptor,
    nova_unit_uuid_t* out_uuid
);
```

Vor der Registrierung prüft Nova:

- UUID eindeutig
- Namespace gültig
- Dimension vorhanden
- Symbol eindeutig
- Konvertierung vollständig
- Descriptor gültig

---

# 15. Dimension Registration

Neue Dimensionen dürfen nur registriert werden, wenn sie nicht bereits existieren.

```c
nova_result_t nova_dimension_register(
    const nova_dimension_descriptor_t* descriptor,
    nova_dimension_uuid_t* out_uuid
);
```

Dimensionen sind unveränderlich.

---

# 16. Alias Registry

Eine Einheit kann mehrere Aliase besitzen.

Beispiel:

```text
meter
metre
m
```

Alle Aliase verweisen auf dieselbe UUID.

Aliasnamen dürfen niemals die Identität einer Einheit bestimmen.

---

# 17. Prefix Registry

Die Prefix Registry verwaltet sämtliche Präfixe.

Mindestens:

```text
yocto
zepto
atto
femto
pico
nano
micro
milli
centi
deci
deca
hecto
kilo
mega
giga
tera
peta
exa
zetta
yotta
```

Jeder Präfix besitzt:

- Name
- Symbol
- Faktor
- Potenz
- Version

---

# 18. Unit Systems

Nova unterstützt mehrere Einheitensysteme.

Beispiele:

- SI
- Imperial
- US Customary
- Nautisch
- Astronomisch
- Medizinisch
- Informationstechnik

Intern erfolgt jede Berechnung über die kanonische SI-Repräsentation.

---

# 19. Registry Lookup

Die Registry stellt mehrere Suchmechanismen bereit.

```c
nova_unit_handle_t nova_unit_find_by_uuid(...);
nova_unit_handle_t nova_unit_find_by_name(...);
nova_unit_handle_t nova_unit_find_by_symbol(...);
```

Die Suchreihenfolge ist:

1. UUID
2. Vollqualifizierter Name
3. Symbol
4. Alias

---

# 20. Registry Validation

Vor jeder Aktivierung wird die Registry vollständig validiert.

Geprüft werden:

- doppelte UUIDs
- doppelte Symbole
- ungültige Dimensionen
- fehlerhafte Präfixe
- ungültige Konvertierungsfunktionen
- fehlerhafte Signaturen
- Versionskonflikte
- zyklische Referenzen

Bei kritischen Fehlern wird die betroffene Registry nicht aktiviert.

---

# Ausblick auf Teil 3

Kapitel 21–30 behandeln:

- Conversion Graph
- Linear Converter
- Affine Converter
- Logarithmic Converter
- Contextual Converter
- Conversion Cache
- Runtime Optimierung
- Thread Safety
- Lock-Free Reads
- Performancemodell

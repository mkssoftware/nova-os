# NPSPEC-UNITS-0003
# Unit Registry and Conversion Engine

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-UNITS-0003 |
| Teil | 5 von 6 |
| Kapitel | 41–50 |
| Version | 1.0.0 |
| Status | Angenommen |
| Letzte Änderung | 2026-07-13 |

---

# 41. Sicherheitsmodell

Die Unit Registry ist ein sicherheitsrelevanter Systemdienst.

Grundprinzipien:

- Least Privilege
- Secure by Default
- Signierte Systemeinträge
- Reproduzierbare Konvertierungen
- Vollständige Nachvollziehbarkeit

Nur autorisierte Komponenten dürfen die globale Registry verändern.

---

# 42. Vertrauensstufen

Jeder Registry-Eintrag besitzt eine Vertrauensstufe.

```text
SYSTEM
VERIFIED
SIGNED
TRUSTED
USER
EXPERIMENTAL
UNTRUSTED
```

Systemeinträge dürfen nicht durch Einträge mit niedrigerer Vertrauensstufe überschrieben werden.

---

# 43. Digitale Signaturen

System-Registry-Dateien sollen digital signiert sein.

Geprüft werden:

- Integrität
- Herausgeber
- Versionskette
- Zeitstempel

Fehlgeschlagene Prüfungen verhindern das Aktivieren der betroffenen Registry.

---

# 44. Zugriffsrechte

Die Registry verwendet rollenbasierte Berechtigungen.

Beispiele:

| Rolle | Rechte |
|---|---|
| Kernel | Vollzugriff |
| Systemdienst | Lesen/Schreiben |
| Administrator | Erweiterungen |
| Anwendung | Lesen |
| Plugin | Eingeschränkt |

Alle Änderungen werden protokolliert.

---

# 45. Sandboxing

Plugins arbeiten in isolierten Ausführungsumgebungen.

Ein Plugin darf:

- eigene Einheiten registrieren
- eigene Präfixe definieren
- eigene Konvertierer bereitstellen

Ein Plugin darf nicht:

- Systemeinheiten ersetzen
- UUIDs wiederverwenden
- System-Namespaces verändern

---

# 46. Performance-Monitoring

Die Runtime sammelt optionale Leistungsmetriken.

Messgrößen:

- Lookup-Zeit
- Cache-Trefferquote
- Konvertierungen pro Sekunde
- Speicherverbrauch
- Registrierungsdauer

Die Erfassung muss abschaltbar sein.

---

# 47. Diagnose

Die Engine stellt Diagnosefunktionen bereit.

```c
nova_result_t nova_registry_verify(void);
nova_result_t nova_registry_dump(FILE* output);
```

Diagnosen umfassen:

- Registry-Konsistenz
- fehlerhafte Referenzen
- doppelte Symbole
- ungültige Konvertierungen

---

# 48. Logging

Alle sicherheitsrelevanten Ereignisse können protokolliert werden.

Beispiele:

- Registrierung neuer Einheiten
- fehlgeschlagene Signaturprüfung
- Zugriffsverletzungen
- fehlerhafte Plugins
- Registry-Rollback

Das Logging ist konfigurierbar.

---

# 49. Integritätsprüfung

Beim Systemstart werden geprüft:

1. Prüfsummen
2. Signaturen
3. UUID-Eindeutigkeit
4. Referenzintegrität
5. Versionskompatibilität

Fehlerhafte Einträge werden isoliert und nicht aktiviert.

---

# 50. Testframework

Die Unit Engine besitzt ein standardisiertes Testsystem.

Pflichttests:

- Registry-Aufbau
- Unit Lookup
- Dimensionsprüfung
- Präfixe
- Aliasauflösung
- Konvertierungen
- Thread-Sicherheit
- Performance
- Persistenz
- Rollback
- Signaturprüfung

Referenztests müssen auf allen unterstützten Architekturen identische Ergebnisse liefern.

---

# Ausblick auf Teil 6

Kapitel 51–60 schließen die Spezifikation mit:

- Referenzimplementierung
- Best Practices
- Migrationsregeln
- Qualitätsrichtlinien
- Compliance
- Zertifizierung
- Wartung
- Roadmap
- Anhängen
- Schlussbestimmungen

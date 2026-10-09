
# NPSPEC-STUDIO-PUBLISHING-0001 – NovaLang Studio Publishing

## Status

Angenommen

## Kategorie

NovaLang Studio / Veröffentlichung / Distribution

## Zweck

Definiert die Veröffentlichung von NovaLang-Programmen, NovaOS-Solutions und Studio-Erweiterungen.

Ziel ist die sichere, nachvollziehbare und möglichst einfache Bereitstellung fertiger Software über lokale und entfernte Veröffentlichungsziele.

## Architektur

Das Publishing-System besteht aus:

- **Publishing Manager:** Steuerung der Veröffentlichung.
- **Target Provider:** Anbindung verschiedener Veröffentlichungsziele.
- **Release Validator:** Prüfung der Veröffentlichungsfähigkeit.
- **Version Manager:** Verwaltung von Release-Versionen.
- **Upload Manager:** Übertragung der Pakete.
- **Publishing History:** Dokumentation veröffentlichter Versionen.

Die Veröffentlichung verwendet ausschließlich validierte Artefakte aus `NPSPEC-STUDIO-PACKAGING-0001`.

## Veröffentlichungsziele

Unterstützt werden:

- Lokale Verzeichnisse
- Netzwerkspeicher
- NovaOS Software Center
- Private Repositories
- Externe Paket-Repositories

Weitere Veröffentlichungsziele können über Provider ergänzt werden.

## Veröffentlichungsprozess

1. Paket und Veröffentlichungsziel auswählen.
2. Version und Metadaten prüfen.
3. Abhängigkeiten und Integrität validieren.
4. Erforderliche Signaturen prüfen.
5. Paket übertragen.
6. Veröffentlichung bestätigen und dokumentieren.

Fehlgeschlagene Übertragungen müssen sicher wiederholbar sein.

## Release-Verwaltung

Jede Veröffentlichung besitzt:

- Eindeutige Release-ID
- Paket- und Versionsidentität
- Zielplattform
- Veröffentlichungsziel
- Integritätsnachweis
- Veröffentlichungsstatus

Bereits veröffentlichte Versionen dürfen nicht unbemerkt überschrieben werden.

## Solution-Integration

Die dauerhafte Solution-GUID bleibt bei regulären Veröffentlichungen erhalten.

Änderungen an Capability-Anforderungen müssen im Paketmanifest nachvollziehbar sein.

Die Veröffentlichung selbst erteilt keine Berechtigungen. Installation und Ausführung unterliegen weiterhin der NovaOS-Autorisierung.

## Normative Anforderungen

1. NovaLang Studio MUSS einen integrierten Publishing Manager bereitstellen.
2. Lokale und entfernte Veröffentlichungsziele MÜSSEN unterstützt werden.
3. Veröffentlichungen MÜSSEN auf validierten Paketen basieren.
4. Paketversionen und Release-Identitäten MÜSSEN eindeutig sein.
5. Integrität und erforderliche Signaturen MÜSSEN vor der Veröffentlichung geprüft werden.
6. Übertragungen MÜSSEN abbrechbar und bei Fehlern kontrolliert wiederholbar sein.
7. Veröffentlichte Versionen MÜSSEN nachvollziehbar bleiben.
8. Sicherheitsrelevante Solution-Änderungen MÜSSEN erkennbar sein.
9. Veröffentlichungen DÜRFEN keine Capability-Berechtigungen erteilen.
10. Netzwerkzugriffe MÜSSEN über autorisierte NovaOS-Capabilities erfolgen.
11. Publishing MUSS auch ohne grafische Studio-Oberfläche funktionieren.
12. Das Publishing-System MUSS ohne KI vollständig funktionieren.

## Ergebnis

NovaLang Studio erhält eine integrierte Veröffentlichungsinfrastruktur, mit der Programme, Solutions und Erweiterungen sicher, versioniert und nachvollziehbar verteilt werden können.

# NPSPEC-USB-UASP-0001 – USB Attached SCSI Protocol

## Status

Angenommen

## Kategorie

USB / UASP / Massenspeicher

## Zweck

Diese Spezifikation definiert UASP-Unterstützung für performante USB-Massenspeicher.

## Modell

UASP nutzt mehrere Endpoints für Command, Status, Data-In und Data-Out und erlaubt parallele SCSI-Kommandos.

## Anforderungen

- UASP MUSS Tag-basierte Kommandos unterstützen.
- Mehrere ausstehende Requests MÜSSEN geordnet abgeschlossen werden können.
- Stream- und Queue-Ressourcen MÜSSEN begrenzt und accountingfähig sein.

## Fallback

Wenn UASP nicht initialisiert werden kann, DARF NovaOS auf BOT zurückfallen, sofern das Gerät BOT anbietet.

## Akzeptanzkriterien

- Parallele Read-Requests können korrekt completed werden.
- Status-Information wird dem richtigen Request zugeordnet.
- Fehlerhafte Tags führen nicht zu falscher Datenzuordnung.


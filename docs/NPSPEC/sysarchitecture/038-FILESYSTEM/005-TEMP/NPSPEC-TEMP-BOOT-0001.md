# NPSPEC-TEMP-BOOT-0001 – Nova Boot Temporary Resources

## Status

Angenommen

## Kategorie

Temporary Resources / Boot

## Zweck

NovaOS definiert temporäre Ressourcen, die während des Bootvorgangs benötigt werden und nach Abschluss der jeweiligen Bootphase nicht dauerhaft bestehen bleiben müssen.

## Grundprinzipien

```text
Boot Temp ≠ Persistent Boot Data
Boot Temp ≠ /Boot
Boot Temp ≠ Recovery Data
Boot Lifetime → Temp Lifetime
Boot Completion → Resource Cleanup
```

## Boot-Bindung

Boot-Temp-Ressourcen werden einem konkreten Bootvorgang zugeordnet:

```text
BootID
   ↓
Boot Temp Scope
   ↓
TempResource
```

Ressourcen eines vorherigen Bootvorgangs dürfen nicht unkontrolliert in einen neuen Bootvorgang übernommen werden.

## Lebenszyklus

```text
Boot Start
    ↓
Allocate
    ↓
Boot Phase
    ↓
Release
    ↓
Boot Complete
    ↓
Reclaim Remaining Resources
```

Ressourcen können bereits nach Abschluss ihrer jeweiligen Bootphase freigegeben werden.

## Ressourcentypen

Boot-Temp kann beispielsweise enthalten:

```text
Loader Buffers
Decompression Buffers
Verification Data
Temporary Module Data
Boot Logs
Hardware Discovery Data
Initialization Data
```

Persistente Boot-Konfigurationen und Recovery-Daten gehören nicht zu Boot-Temp.

## Übergabe an das System

Einige während des Bootens erzeugte Informationen können vom gestarteten System benötigt werden.

Diese müssen explizit übertragen werden:

```text
Boot Temp Resource
       ↓
Explicit Handoff
       ↓
System-owned Resource
```

Die Übergabe ändert Eigentümer und Lebenszyklus der Ressource.

## Fehler und Recovery

Bei fehlgeschlagenem Boot dürfen relevante Diagnoseinformationen gezielt für Recovery oder Fehleranalyse erhalten werden.

Dies muss ausdrücklich definiert sein und darf nicht dazu führen, dass beliebige Boot-Temp-Ressourcen dauerhaft bestehen bleiben.

## Resource Economy

Boot-Temp muss mit begrenztem Speicher arbeiten können.

Nicht mehr benötigte Ressourcen sollen möglichst früh freigegeben werden, insbesondere während früher Bootphasen mit eingeschränkten Ressourcen.

## Sicherheit

Sicherheitsrelevante temporäre Daten wie Schlüsselmaterial, Verifikationsdaten oder entschlüsselte Zwischenwerte müssen nach ihrer Verwendung sicher bereinigbar sein.

Boot-Temp erzeugt keine zusätzliche Authority.

## Normative Anforderungen

1. Boot-Temp-Ressourcen MÜSSEN einem Bootvorgang zuordenbar sein.
2. Boot-Temp DARF nicht mit persistenten Daten unter `/Boot` gleichgesetzt werden.
3. Ressourcen SOLLEN nach ihrer letzten benötigten Bootphase freigegeben werden.
4. Nach erfolgreichem Boot MÜSSEN verbleibende Boot-Temp-Ressourcen reclaimable sein.
5. Eine Übergabe an das laufende System MUSS explizit erfolgen.
6. Übergebene Ressourcen MÜSSEN einen neuen eindeutigen Lebenszyklus erhalten.
7. Fehlgeschlagene Bootvorgänge DÜRFEN Diagnoseinformationen kontrolliert erhalten.
8. Boot-Temp MUSS mit begrenzten Ressourcen funktionieren können.
9. Sensible temporäre Boot-Daten MÜSSEN sicher bereinigbar sein.
10. Boot-Temp DARF keine zusätzliche Authority erzeugen.

## Abhängigkeiten

- `NPSPEC-TEMP-MODEL-0001`
- `NPSPEC-ARCH-RESOURCEECONOMY-0001`
- `NPSPEC-BOOT-ARCH-0001`
- `NPSPEC-BOOT-RECOVERY-0001`

## Ergebnis

NovaOS besitzt einen klar definierten temporären Ressourcenbereich für den Bootvorgang. Kurzlebige Bootdaten können frühzeitig freigegeben, notwendige Ressourcen explizit an das gestartete System übergeben und sensible Zwischenwerte kontrolliert bereinigt werden.
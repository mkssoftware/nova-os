# Nova Studio – Projektdateiformat (project.xml)

Der Entwicklungsauftrag nennt `.nproj`/`.nsln` als Beispiel-Dateinamen.
Die tatsächlich implementierte, versionierte Projektdatei heißt
`project.xml` und liegt im Projektwurzelordner (ein Ordner = ein
Projekt; es gibt noch kein Mehrprojekt-"Solution"-Konzept, siehe
Einschränkungen unten).

## Format (Version 1.0)

```xml
<?xml version="1.0" encoding="utf-8"?>
<NovaProject version="1.0">
  <Identity>
    <Name>MeinProjekt</Name>
    <Type>NovaLang Application</Type>
    <Language>NovaLang 1.0</Language>
  </Identity>
  <Build>
    <Configurations>
      <Config name="Debug"   optimize="false" debugSymbols="true"/>
      <Config name="Release" optimize="true"  debugSymbols="false"/>
    </Configurations>
    <Target>x86_32</Target>
    <Output>Build/</Output>
  </Build>
  <Sources>
    <File path="src/main.nova"/>
  </Sources>
</NovaProject>
```

## Felder

| Element | Bedeutung | Tatsächlich ausgelesen/verwendet? |
|---|---|---|
| `<Name>` | Projektname, erscheint in Titelleiste | ✅ ja (`OpenProject()`) |
| `<Type>`, `<Language>` | Beschreibend | ❌ nein, rein informativ |
| `<Configurations>` (Debug/Release) | Build-Konfigurationen | ❌ nein - `DoBuild()` kennt nur eine feste Konfiguration (keine Optimierungsstufen-/Debug-Symbol-Unterscheidung, da `novac.exe` dafür aktuell keine Schalter hat) |
| `<Target>` | Zielarchitektur | ❌ nein, rein informativ (muss `x86_32` sein - war bis 2026-11-24 fälschlich `x86_64` im Vorlagen-Template, siehe `IMPLEMENTATION_STATUS.md`) |
| `<Output>` | Ausgabeordner für Build-Artefakte | ❌ nein - `DoBuild()` legt Artefakte aktuell NEBEN der Quelldatei ab, nicht in einem separaten Output-Ordner |
| `<Sources><File path="..."/></Sources>` | Liste der Projekt-Quelldateien, relativ zum Projektordner, `/`-getrennt | ✅ ja - vollständig: wird beim Öffnen gelesen (`OpenProject()`) UND beim Speichern neuer Dateien automatisch ergänzt (`ProjectRegisterSource()`) |

**Wichtig:** Das erste `<File>`-Element in `<Sources>` ist die
**Einstiegsdatei** (`Project::entryFile`) - die einzige Datei, die
"Erstellen" tatsächlich an `novac.exe` übergibt (siehe
`CROSS_COMPILATION.md` zur Compiler-Einschränkung "eine Datei pro
Aufruf"). Weitere `<File>`-Einträge werden nachverfolgt (z. B. für einen
künftigen Projektmappen-Explorer), aber NICHT mitkompiliert.

## Parser-Implementierung

Kein vollwertiger XML-Parser (das Format ist bewusst einfach und
selbst kontrolliert) - einfache Textsuche nach `<Name>...</Name>` bzw.
`<File path="...">`-Mustern (`OpenProject()`/`ProjectRegisterSource()`
in `src/main.cpp`). Funktioniert zuverlässig für genau das Format, das
`DoNewProject()` (`src/dialogs.cpp`) selbst erzeugt - kein beliebiges
XML (Kommentare, CDATA, abweichende Attributreihenfolge o. Ä. werden
nicht unterstützt).

## Fehlende Elemente aus dem ursprünglichen Auftrag

- Kein `.nsln`-Mehrprojekt-Format (eine "Solution" mit mehreren
  Projekten) - nicht implementiert.
- Keine Projektreferenzen (`<ProjectReference>` o. ä.) - nicht
  implementiert, und ohne Mehrdatei-Compiler-Unterstützung (siehe
  `CROSS_COMPILATION.md`) ohnehin nicht sinnvoll nutzbar.
- Keine SDK-/Bibliotheksreferenzen im Projektformat - siehe
  `SDK_SPECIFICATION.md`.

# NPSPEC-COMPUTE-DICOM-0001 — DICOM Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / DICOM  
**Komponente:** NovaOS DICOM Compute  
**Bezeichner:** `NPSPEC-COMPUTE-DICOM-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RUNTIME-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-HPC-0001`  
**Kurzname:** DICOM Compute  

---

## 1. Zweck

Diese Spezifikation definiert die gemeinsame DICOM-Compute-Infrastruktur der NovaOS-Compute-Architektur.

DICOM steht für:

`Digital Imaging and Communications in Medicine`

DICOM Compute stellt Funktionen zur strukturierten Verarbeitung medizinischer Bild- und Metadaten bereit.

Die Infrastruktur MUSS insbesondere folgende Aufgaben ermöglichen:

- DICOM-Dateien erkennen,
- DICOM-Datensätze einlesen,
- DICOM-Metadaten interpretieren,
- Bilddaten dekodieren,
- Serien rekonstruieren,
- Volumendaten erzeugen,
- Pixelwerte physikalisch interpretieren,
- geometrische Beziehungen zwischen Bildern bestimmen,
- Multiframe-Datensätze verarbeiten,
- komprimierte Pixelinformationen dekodieren,
- DICOM-Daten validieren,
- DICOM-Daten anonymisieren beziehungsweise de-identifizieren,
- große Bilddatensätze parallel verarbeiten,
- medizinische Bilddaten für weitere Compute-Module bereitstellen.

DICOM Compute ist keine medizinische Diagnosekomponente.

Die Infrastruktur DARF medizinische Daten verarbeiten, transformieren und analysieren, MUSS jedoch zwischen technischer Bildverarbeitung und medizinischer Interpretation unterscheiden.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> DICOM-Dateiformat, DICOM-Datensatz, Metadaten, Pixelrepräsentation, Bildgeometrie, Serie, Volumen, Codec und Compute-Backend werden logisch voneinander getrennt.

Dadurch SOLL dieselbe DICOM-Infrastruktur unterschiedliche Modalitäten und Verarbeitungspipelines unterstützen können.

Beispiele:

- CT,
- MR,
- PET,
- SPECT,
- Ultraschall,
- Röntgen,
- Mammographie,
- Angiographie,
- Strahlentherapie,
- Structured Reports,
- Segmentierungen,
- Parametric Maps.

---

## 3. Ziele

DICOM Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. DICOM-Dateierkennung,
2. DICOM File Meta Information,
3. DICOM Data Sets,
4. Data Elements,
5. Tags,
6. Value Representations,
7. Value Multiplicity,
8. Transfer Syntax,
9. Explicit VR,
10. Implicit VR,
11. Little Endian,
12. Pixel Data,
13. komprimierte Pixel Data,
14. Multiframe-Daten,
15. Sequenzen,
16. verschachtelte Datensätze,
17. Serien,
18. Studien,
19. Patientenbezug,
20. Instanzen,
21. SOP Classes,
22. SOP Instances,
23. Bildgeometrie,
24. Pixelabstände,
25. Slice-Positionen,
26. Volumenrekonstruktion,
27. Rescale-Operationen,
28. Windowing,
29. Farbmodelle,
30. LUT-Verarbeitung,
31. De-Identifikation,
32. Validierung,
33. Fehlerdiagnostik,
34. CPU-Verarbeitung,
35. GPU-Verarbeitung,
36. parallele Verarbeitung,
37. Streaming,
38. Checkpointing für große Verarbeitungspipelines,
39. deterministische Verarbeitung,
40. Erweiterbarkeit.

---

## 4. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- PACS,
- RIS,
- HIS,
- elektronische Patientenakten,
- medizinische Diagnosen,
- Therapieentscheidungen,
- vollständige DICOM-Netzwerkdienste,
- vollständige DICOMweb-Infrastruktur,
- konkrete Benutzeroberflächen,
- konkrete medizinische Viewer,
- konkrete KI-Diagnosemodelle,
- regulatorische Zulassungsverfahren.

Diese Funktionen MÜSSEN durch separate Spezifikationen oder Anwendungen definiert werden.

---

## 5. Sicherheitsgrundsatz

Medizinische Bilddaten können besonders schützenswerte personenbezogene Informationen enthalten.

DICOM Compute MUSS deshalb Datenverarbeitung ermöglichen, ohne vorauszusetzen, dass Daten frei weitergegeben oder dauerhaft gespeichert werden dürfen.

Zugriffsschutz, Datenschutz, Berechtigungen und persistente Speicherung MÜSSEN durch die zuständigen NovaOS-Sicherheits- und Datenrichtlinien kontrolliert werden.

---

## 6. DICOM Object Model

Die grundlegende logische Struktur lautet:

    DICOMObject
     |
     +-- FileMetaInformation
     |
     +-- DataSet
          |
          +-- DataElement
          +-- Sequence
          +-- PixelData

---

## 7. DICOM File

Eine DICOM-Datei SOLL als:

`DICOMFile`

repräsentiert werden.

---

## 8. File Preamble

DICOM-Dateien DÜRFEN einen 128-Byte-Preamble-Bereich enthalten.

---

## 9. DICM Prefix

Bei DICOM-Dateien mit File Format SOLL der Prefix:

`DICM`

erkannt werden.

Das Fehlen des Prefix DARF nicht automatisch bedeuten, dass die Daten kein gültiger DICOM-Datensatz sind.

---

## 10. File Meta Information

File Meta Information MUSS separat vom eigentlichen Data Set repräsentiert werden können.

---

## 11. Data Set

Der eigentliche DICOM-Datensatz wird als:

`DICOMDataSet`

bezeichnet.

---

## 12. Data Element

Ein DICOM Data Element wird als:

`DICOMElement`

repräsentiert.

Ein Element MUSS mindestens besitzen können:

- Tag,
- Value Representation,
- Value Length,
- Value.

---

## 13. Tag

Ein DICOM-Tag besteht aus:

`Group`

und:

`Element`

Konzeptionell:

`(gggg,eeee)`

---

## 14. Tag Representation

Ein Tag MUSS verlustfrei als 32-Bit-Wert oder äquivalente strukturierte Darstellung repräsentierbar sein.

---

## 15. Standard Tag Dictionary

DICOM Compute SOLL ein versioniertes Dictionary standardisierter DICOM-Tags verwenden können.

---

## 16. Private Tags

Private Tags MÜSSEN erhalten und verarbeitet werden können, auch wenn ihre Semantik unbekannt ist.

---

## 17. Unknown Tags

Unbekannte Tags DÜRFEN nicht ohne explizite Policy verworfen werden.

---

## 18. Value Representation

Die Value Representation wird abgekürzt:

`VR`

---

## 19. VR Support

Eine vollständige Implementierung SOLL alle relevanten standardisierten DICOM-VRs verarbeiten können.

Dazu gehören unter anderem:

- AE,
- AS,
- AT,
- CS,
- DA,
- DS,
- DT,
- FD,
- FL,
- IS,
- LO,
- LT,
- OB,
- OD,
- OF,
- OL,
- OV,
- OW,
- PN,
- SH,
- SL,
- SQ,
- SS,
- ST,
- SV,
- TM,
- UC,
- UI,
- UL,
- UN,
- UR,
- US,
- UT,
- UV.

---

## 20. Unknown VR

Unbekannte oder nicht interpretierbare VR-Daten SOLLEN verlustfrei als Rohdaten erhalten werden können.

---

## 21. Value Multiplicity

Die Anzahl der Werte eines Elements wird als:

`VM`

bezeichnet.

---

## 22. Multi-Value Elements

Mehrfachwerte MÜSSEN unterstützt werden.

---

## 23. Sequence

DICOM-Sequenzen werden über:

`SQ`

repräsentiert.

---

## 24. Sequence Item

Eine Sequence MUSS beliebig viele Items enthalten können.

---

## 25. Nested Sequence

Verschachtelte Sequenzen MÜSSEN unterstützt werden.

---

## 26. Recursion Protection

Beim Parsen verschachtelter Sequenzen MUSS eine Schutzstrategie gegen bösartig oder fehlerhaft extrem tief verschachtelte Strukturen existieren.

---

## 27. Transfer Syntax

Die Transfer Syntax MUSS anhand ihrer UID identifiziert werden können.

---

## 28. Transfer Syntax UID

Eine Transfer Syntax wird über:

`TransferSyntaxUID`

referenziert.

---

## 29. Implicit VR Little Endian

Implicit VR Little Endian MUSS unterstützt werden.

---

## 30. Explicit VR Little Endian

Explicit VR Little Endian MUSS unterstützt werden.

---

## 31. Deflated Transfer Syntax

Deflated Explicit VR Little Endian SOLL unterstützt werden können.

---

## 32. Unsupported Transfer Syntax

Eine nicht unterstützte Transfer Syntax MUSS einen strukturierten Fehler erzeugen.

---

## 33. Codec Architecture

Komprimierte Transfer Syntaxes MÜSSEN über austauschbare Codecs unterstützt werden können.

Konzeptionell:

    PixelData
        |
        v
    TransferSyntax
        |
        v
       Codec
        |
        v
    DecodedPixels

---

## 34. Codec Interface

Ein Codec SOLL mindestens bereitstellen:

- Capability Query,
- Decode,
- optional Encode,
- Error Reporting.

---

## 35. JPEG

JPEG-basierte DICOM-Pixelkompression SOLL unterstützt werden können.

---

## 36. JPEG Lossless

JPEG Lossless SOLL unterstützt werden können.

---

## 37. JPEG-LS

JPEG-LS SOLL unterstützt werden können.

---

## 38. JPEG 2000

JPEG-2000-basierte Transfer Syntaxes SOLLEN unterstützt werden können.

---

## 39. RLE

DICOM RLE Lossless SOLL unterstützt werden können.

---

## 40. Codec Independence

Der DICOM-Parser DARF nicht fest an einen einzelnen Bildcodec gekoppelt sein.

---

## 41. Pixel Data

Pixelinformationen werden über ein Pixel-Data-Objekt repräsentiert.

Konzeptionell:

`DICOMPixelData`

---

## 42. Native Pixel Data

Unkomprimierte Pixel Data MUSS unterstützt werden.

---

## 43. Encapsulated Pixel Data

Encapsulated Pixel Data MUSS architektonisch unterstützt werden.

---

## 44. Fragment

Encapsulated Pixel Data MUSS fragmentierte Daten repräsentieren können.

---

## 45. Basic Offset Table

Eine vorhandene Basic Offset Table SOLL interpretiert werden können.

---

## 46. Extended Offset Tables

Erweiterte Offset-Informationen SOLLEN unterstützt werden können.

---

## 47. Frame

Eine einzelne Bildeinheit wird als:

`DICOMFrame`

bezeichnet.

---

## 48. Single Frame

Single-Frame-DICOM-Instanzen MÜSSEN unterstützt werden.

---

## 49. Multi Frame

Multiframe-DICOM-Instanzen MÜSSEN architektonisch unterstützt werden.

---

## 50. Number of Frames

Die Anzahl der Frames MUSS bestimmt werden können.

---

## 51. Frame Access

Ein einzelner Frame SOLL dekodiert werden können, ohne zwingend alle Frames vollständig zu dekodieren.

---

## 52. Lazy Frame Decode

Die Runtime SOLL Lazy Decoding unterstützen können.

---

## 53. Streaming Decode

Große Multiframe-Instanzen SOLLEN frameweise verarbeitet werden können.

---

## 54. Rows

Die Bildhöhe MUSS aus dem DICOM-Datensatz bestimmbar sein.

---

## 55. Columns

Die Bildbreite MUSS bestimmbar sein.

---

## 56. Samples Per Pixel

Die Anzahl der Samples pro Pixel MUSS berücksichtigt werden.

---

## 57. Bits Allocated

`BitsAllocated`

MUSS berücksichtigt werden.

---

## 58. Bits Stored

`BitsStored`

MUSS berücksichtigt werden.

---

## 59. High Bit

`HighBit`

MUSS berücksichtigt werden.

---

## 60. Pixel Representation

Signed und Unsigned Pixel Representation MÜSSEN unterstützt werden.

---

## 61. Pixel Validation

Inkonsistente Pixel-Metadaten MÜSSEN erkannt werden.

---

## 62. Photometric Interpretation

Die Photometric Interpretation MUSS ausgewertet werden können.

---

## 63. MONOCHROME1

`MONOCHROME1`

MUSS unterstützt werden können.

---

## 64. MONOCHROME2

`MONOCHROME2`

MUSS unterstützt werden können.

---

## 65. RGB

`RGB`

SOLL unterstützt werden.

---

## 66. YBR

Relevante YBR-Farbrepräsentationen SOLLEN unterstützt werden können.

---

## 67. Palette Color

Palette-Color-DICOM-Bilder SOLLEN unterstützt werden können.

---

## 68. Planar Configuration

Planar Configuration MUSS bei relevanten Mehrkanalbildern berücksichtigt werden.

---

## 69. Pixel Pipeline

Die logische Bildverarbeitung SOLL in getrennte Stufen zerlegt werden:

    Stored Values
         |
         v
    Modality Transform
         |
         v
    VOI Transform
         |
         v
    Presentation Transform
         |
         v
    Display Values

---

## 70. Stored Pixel Value

Der rohe gespeicherte Pixelwert MUSS unabhängig von physikalisch transformierten Werten zugänglich bleiben können.

---

## 71. Rescale Slope

`RescaleSlope`

SOLL unterstützt werden.

---

## 72. Rescale Intercept

`RescaleIntercept`

SOLL unterstützt werden.

---

## 73. Rescale Formula

Für geeignete DICOM-Objekte SOLL gelten können:

`Output = StoredValue * RescaleSlope + RescaleIntercept`

---

## 74. CT Hounsfield Units

CT-Daten SOLLEN unter Verwendung der im Datensatz angegebenen Transformationen in physikalisch interpretierbare Werte wie Hounsfield Units überführt werden können.

Eine feste Annahme über Slope oder Intercept DARF nicht verwendet werden.

---

## 75. Modality LUT

Modality LUT MUSS architektonisch unterstützt werden.

---

## 76. VOI LUT

VOI LUT SOLL unterstützt werden können.

---

## 77. Window Center

`WindowCenter`

SOLL interpretiert werden können.

---

## 78. Window Width

`WindowWidth`

SOLL interpretiert werden können.

---

## 79. Multiple Windows

Mehrere Window-Center-/Window-Width-Werte MÜSSEN repräsentierbar sein.

---

## 80. Windowing

Windowing MUSS als Darstellungstransformation behandelt werden und DARF die ursprünglichen Pixelwerte nicht zerstören.

---

## 81. Presentation LUT

Presentation LUT SOLL unterstützt werden können.

---

## 82. Inversion

MONOCHROME1 und andere erforderliche Darstellungstransformationen SOLLEN korrekt berücksichtigt werden.

---

## 83. Study

Eine DICOM-Studie wird logisch als:

`DICOMStudy`

repräsentiert.

---

## 84. Study Instance UID

Die Study Instance UID MUSS als primärer technischer Studienbezeichner verwendet werden können.

---

## 85. Series

Eine DICOM-Serie wird als:

`DICOMSeries`

repräsentiert.

---

## 86. Series Instance UID

Die Series Instance UID MUSS zur technischen Gruppierung verwendet werden können.

---

## 87. SOP Instance

Eine einzelne DICOM-Instanz wird als:

`DICOMInstance`

repräsentiert.

---

## 88. SOP Instance UID

Die SOP Instance UID MUSS unterstützt werden.

---

## 89. SOP Class UID

Die SOP Class UID MUSS unterstützt werden.

---

## 90. Instance Number

`InstanceNumber`

DARF als Ordnungsinformation verwendet werden.

Sie DARF jedoch nicht als alleinige Grundlage für geometrische Slice-Sortierung verwendet werden.

---

## 91. Patient Metadata

Patientenbezogene Attribute MÜSSEN strukturiert repräsentierbar sein.

---

## 92. Patient Name

Person Name MUSS entsprechend der DICOM-PN-Semantik verarbeitet werden können.

---

## 93. Character Sets

DICOM-spezifische Zeichensatzinformationen MÜSSEN berücksichtigt werden.

---

## 94. Specific Character Set

`SpecificCharacterSet`

MUSS ausgewertet werden können.

---

## 95. Unicode

Interne Textrepräsentationen SOLLEN nach Dekodierung Unicode verwenden.

---

## 96. Original Encoding

Die ursprüngliche Zeichenkodierung SOLL bei verlustfreier Weiterverarbeitung nachvollziehbar bleiben können.

---

## 97. Modality

Die Modalität MUSS bestimmt werden können.

Beispiele:

- CT,
- MR,
- PT,
- US,
- CR,
- DX,
- MG,
- XA,
- RF,
- NM,
- RTIMAGE.

---

## 98. Modality Independence

Der grundlegende DICOM-Parser DARF nicht an eine einzelne Modalität gekoppelt sein.

---

## 99. Image Geometry

DICOM Compute MUSS räumliche Bildgeometrie rekonstruieren können.

---

## 100. Pixel Spacing

Pixel Spacing MUSS berücksichtigt werden.

---

## 101. Image Position

`ImagePositionPatient`

SOLL für patientenbezogene räumliche Positionierung verwendet werden.

---

## 102. Image Orientation

`ImageOrientationPatient`

SOLL für die Orientierung des Bildes verwendet werden.

---

## 103. Row Direction

Die räumliche Zeilenrichtung MUSS aus den Orientierungsinformationen bestimmbar sein.

---

## 104. Column Direction

Die räumliche Spaltenrichtung MUSS bestimmbar sein.

---

## 105. Slice Normal

Die Slice-Normale SOLL aus dem Kreuzprodukt der Richtungsvektoren bestimmt werden können:

`normal = rowDirection × columnDirection`

---

## 106. Slice Position

Die geometrische Position eines Slice SOLL durch Projektion auf die Slice-Normale bestimmt werden können.

---

## 107. Geometric Sorting

Slices SOLLEN anhand ihrer geometrischen Position sortiert werden.

---

## 108. Instance Number Sorting

Instance Number DARF nur als Fallback oder zusätzliche Konsistenzinformation verwendet werden.

---

## 109. Slice Spacing

Der tatsächliche Abstand benachbarter Slices SOLL aus deren räumlicher Position bestimmt werden können.

---

## 110. Slice Thickness

`SliceThickness`

DARF nicht automatisch mit dem tatsächlichen Abstand der Slice-Ebenen gleichgesetzt werden.

---

## 111. Non-Uniform Spacing

Nicht äquidistante Slice-Abstände MÜSSEN erkannt werden können.

---

## 112. Missing Slice Detection

Fehlende Slices SOLLEN anhand geometrischer Abstände erkannt werden können.

---

## 113. Duplicate Slice Detection

Geometrisch doppelte Slices SOLLEN erkannt werden können.

---

## 114. Orientation Consistency

Eine Serie MUSS auf konsistente Bildorientierung geprüft werden können.

---

## 115. Geometry Tolerance

Geometrische Vergleiche MÜSSEN konfigurierbare Floating-Point-Toleranzen verwenden.

---

## 116. Volume

Ein aus DICOM-Bildern rekonstruiertes Volumen wird als:

`DICOMVolume`

bezeichnet.

---

## 117. Volume Dimensions

Ein Volumen MUSS mindestens besitzen können:

- Width,
- Height,
- Depth.

---

## 118. Voxel Spacing

Ein Volumen MUSS physikalische Voxelabstände besitzen können:

- SpacingX,
- SpacingY,
- SpacingZ.

---

## 119. Volume Origin

Ein Volumen SOLL einen räumlichen Ursprung besitzen.

---

## 120. Direction Matrix

Die räumliche Orientierung SOLL als Direction Matrix repräsentiert werden können.

---

## 121. Voxel-to-Patient Transform

Eine Transformation von Voxelkoordinaten zu patientenbezogenen Koordinaten MUSS ableitbar sein.

Konzeptionell:

`PatientPosition = Origin + Direction * (VoxelIndex * Spacing)`

---

## 122. Patient-to-Voxel Transform

Die inverse Transformation SOLL verfügbar sein.

---

## 123. Volume Reconstruction

Eine Serie kompatibler Bilder SOLL zu einem Volumen rekonstruiert werden können.

---

## 124. Volume Compatibility

Vor der Volumenrekonstruktion MÜSSEN mindestens geprüft werden:

- Rows,
- Columns,
- Pixel Spacing,
- Orientation,
- Pixel Representation,
- Bits Stored,
- Samples Per Pixel,
- Photometric Interpretation,
- geometrische Slice-Anordnung.

---

## 125. Mixed Series Protection

Nicht kompatible Instanzen DÜRFEN nicht stillschweigend zu einem gemeinsamen Volumen zusammengeführt werden.

---

## 126. Localizer Protection

Scout-, Localizer- oder andere geometrisch inkompatible Bilder SOLLEN von regulären Volumenstacks getrennt werden können.

---

## 127. Enhanced DICOM

Enhanced Multi-Frame DICOM MUSS architektonisch unterstützt werden.

---

## 128. Functional Groups

Shared und Per-Frame Functional Groups SOLLEN verarbeitet werden können.

---

## 129. Shared Functional Groups

Gemeinsame Frame-Metadaten SOLLEN nur einmal logisch gespeichert werden müssen.

---

## 130. Per-Frame Functional Groups

Frame-spezifische Geometrie und Parameter MÜSSEN unterstützt werden können.

---

## 131. Frame Geometry

Jeder Frame DARF eine eigene räumliche Geometrie besitzen.

---

## 132. Dimension Organization

Enhanced-DICOM-Dimensionierungsinformationen SOLLEN interpretierbar sein.

---

## 133. Temporal Dimension

Zeitabhängige Frames SOLLEN als zusätzliche Dimension gruppiert werden können.

---

## 134. Cardiac Phases

Herzphasen SOLLEN strukturiert repräsentiert werden können, sofern entsprechende DICOM-Informationen vorhanden sind.

---

## 135. Dynamic Series

Dynamische Bildserien SOLLEN als Zeitreihe verarbeitet werden können.

---

## 136. 4D Volume

Zeitabhängige Volumendaten SOLLEN als:

`DICOMVolumeSequence`

repräsentiert werden können.

---

## 137. Segmentation Objects

DICOM Segmentation Objects SOLLEN unterstützt werden können.

---

## 138. SEG

DICOM SEG SOLL als separate semantische Datenklasse behandelt werden.

---

## 139. Segment

Ein Segment SOLL mindestens besitzen können:

- Segment Number,
- Segment Label,
- Segment Description,
- Algorithm Type,
- Coded Property Information.

---

## 140. Binary Segmentation

Binäre Segmentierungen SOLLEN unterstützt werden.

---

## 141. Fractional Segmentation

Fractional Segmentation SOLL unterstützt werden können.

---

## 142. Segment Geometry

Segmentierungsdaten MÜSSEN räumlich auf ihre Referenzbilder abbildbar sein.

---

## 143. Referenced Instances

DICOM-Objekte MÜSSEN Referenzen auf andere SOP Instances erhalten können.

---

## 144. Reference Resolution

Referenzen SOLLEN innerhalb eines geladenen DICOM-Datenbestands auflösbar sein.

---

## 145. Registration

DICOM Spatial Registration Objects SOLLEN architektonisch unterstützt werden können.

---

## 146. Transform

Registrierungen SOLLEN als Transformationen zwischen Koordinatensystemen repräsentiert werden.

---

## 147. Structured Reports

DICOM Structured Reports SOLLEN als strukturierte DICOM-Daten verarbeitet werden können.

---

## 148. SR Content Tree

SR-Inhalte SOLLEN als Baumstruktur repräsentiert werden.

---

## 149. Measurement Data

Messwerte aus SR-Objekten SOLLEN strukturiert zugänglich sein.

---

## 150. Parametric Maps

DICOM Parametric Map Objects SOLLEN unterstützt werden können.

---

## 151. Floating Pixel Data

Floating Point Pixel Data und Double Floating Point Pixel Data SOLLEN unterstützt werden können.

---

## 152. Real World Value Mapping

Real World Value Mapping SOLL unterstützt werden können.

---

## 153. Quantitative Imaging

DICOM Compute SOLL quantitative Bildwerte erhalten können, ohne sie durch Darstellungsoperationen zu verfälschen.

---

## 154. Raw Data Preservation

Originale Pixelwerte SOLLEN unverändert verfügbar bleiben.

---

## 155. Derived Data

Verarbeitete Daten MÜSSEN als abgeleitete Daten vom Originalzustand unterscheidbar sein.

---

## 156. Immutable Source

Eine Compute-Pipeline SOLL Originaldaten standardmäßig nicht in-place verändern.

---

## 157. Processing Pipeline

DICOM Compute SOLL Verarbeitungspipelines unterstützen.

Beispiel:

    DICOM Input
         |
         v
       Parse
         |
         v
       Decode
         |
         v
    Geometry
         |
         v
    Calibration
         |
         v
    Processing
         |
         v
      Output

---

## 158. Pipeline Stage

Eine Pipeline-Stufe SOLL als unabhängige Compute-Operation repräsentierbar sein.

---

## 159. Pipeline Dependencies

Abhängigkeiten zwischen Verarbeitungsschritten MÜSSEN explizit darstellbar sein.

---

## 160. Lazy Processing

Verarbeitungsschritte SOLLEN bei Bedarf verzögert ausgeführt werden können.

---

## 161. Streaming Processing

Große Datensätze SOLLEN verarbeitet werden können, ohne vollständig im Arbeitsspeicher liegen zu müssen.

---

## 162. Chunk Processing

Volumendaten SOLLEN blockweise verarbeitet werden können.

---

## 163. Tile Processing

Große 2D-Bilder SOLLEN kachelweise verarbeitet werden können.

---

## 164. Frame Processing

Multiframe-Daten SOLLEN frameweise verarbeitet werden können.

---

## 165. CPU Backend

DICOM Compute MUSS CPU-Verarbeitung unterstützen.

---

## 166. Multi-Core CPU

Unabhängige Instanzen, Frames, Slices und Tiles SOLLEN parallel verarbeitet werden können.

---

## 167. SIMD

Pixeloperationen SOLLEN SIMD-Vektorisierung verwenden können.

---

## 168. GPU Backend

DICOM Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` verwenden können.

---

## 169. GPU Operations

Geeignete Operationen SOLLEN auf der GPU ausgeführt werden können.

Beispiele:

- Resampling,
- Windowing,
- LUT,
- Farbkonvertierung,
- Filter,
- Volumenoperationen,
- Projektionen,
- Segmentierungsoperationen.

---

## 170. GPU Independence

DICOM Parsing selbst DARF keine GPU voraussetzen.

---

## 171. NPU Backend

`NPSPEC-COMPUTE-RUNTIME-0006` DARF für KI-basierte medizinische Bildverarbeitung verwendet werden.

---

## 172. NPU Separation

KI-Inferenz MUSS logisch von der DICOM-Decodierung getrennt bleiben.

---

## 173. AI Input

DICOM-Daten SOLLEN kontrolliert in Tensoren oder andere Compute-Objekte überführt werden können.

---

## 174. AI Metadata

Relevante räumliche und physikalische Metadaten DÜRFEN bei einer Tensor-Konvertierung nicht unbeabsichtigt verloren gehen.

---

## 175. Tensor Object

Eine Konvertierung SOLL mindestens erzeugen können:

`PixelTensor`

und:

`SpatialMetadata`

---

## 176. GPU Tensor Sharing

Wenn kompatibel, SOLLEN dekodierte GPU-Daten ohne unnötige Kopien an weitere GPU-Compute-Operationen übergeben werden können.

---

## 177. Zero-Copy

Zero-Copy- oder Shared-Memory-Verfahren SOLLEN verwendet werden können, sofern Lebensdauer und Synchronisation korrekt behandelt werden.

---

## 178. Memory Model

DICOM Compute MUSS mit `NPSPEC-COMPUTE-RUNTIME-0003` integrierbar sein.

---

## 179. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN möglich sein:

- RawFileBuffer,
- DataSetBuffer,
- EncodedPixelBuffer,
- DecodedPixelBuffer,
- FrameBuffer,
- VolumeBuffer,
- MetadataBuffer,
- LUTBuffer,
- TemporaryComputeBuffer.

---

## 180. Memory Mapping

Große DICOM-Dateien SOLLEN über Memory Mapping verarbeitet werden können.

---

## 181. Partial Reads

Der Parser SOLL Datenbereiche gezielt lesen können, ohne immer die vollständige Datei zu kopieren.

---

## 182. Metadata-Only Read

Eine DICOM-Datei SOLL ohne vollständiges Laden beziehungsweise Dekodieren der Pixel Data analysiert werden können.

---

## 183. Stop Before Pixel Data

Ein Parser SOLL optional nach den benötigten Metadaten vor Pixel Data stoppen können.

---

## 184. Deferred Pixel Data

Pixel Data SOLL verzögert geladen werden können.

---

## 185. Memory Budget

Eine Verarbeitungspipeline SOLL ein Speicherbudget erhalten können.

---

## 186. Memory Estimation

Der erwartete Speicherbedarf einer Dekodierung oder Volumenrekonstruktion SOLL vorab geschätzt werden können.

---

## 187. Out-of-Memory

Speicherknappheit MUSS kontrolliert behandelt werden.

---

## 188. Cache

Dekodierte Frames DÜRFEN gecacht werden.

---

## 189. Cache Policy

Cache-Größe und Eviction-Policy SOLLEN konfigurierbar sein.

---

## 190. Scheduler Integration

DICOM Compute MUSS mit `NPSPEC-COMPUTE-RUNTIME-0002` integrierbar sein.

---

## 191. Scheduler Tasks

Typische Tasks sind:

- Parse Metadata,
- Decode Frame,
- Sort Series,
- Reconstruct Volume,
- Rescale Pixels,
- Window Pixels,
- Resample,
- Validate,
- De-Identify,
- Export.

---

## 192. Task Graph

Eine DICOM-Verarbeitung SOLL als Abhängigkeitsgraph repräsentiert werden können.

Beispiel:

    Parse
      |
      +--> Metadata
      |
      v
    Decode
      |
      v
    Geometry
      |
      v
    Volume
      |
      v
    Processing

---

## 193. Parallel Decode

Mehrere unabhängige Frames SOLLEN parallel dekodiert werden können.

---

## 194. Parallel Series Scan

Große Verzeichnisse oder Datenbestände SOLLEN parallel auf DICOM-Metadaten untersucht werden können.

---

## 195. Distributed Compute

Sehr große Datenbestände SOLLEN über `NPSPEC-COMPUTE-RUNTIME-0007` verteilt verarbeitet werden können.

---

## 196. Distributed Partition

Partitionierung DARF erfolgen nach:

- Study,
- Series,
- Instance,
- Frame,
- Volume Chunk.

---

## 197. Data Locality

Verteilte Verarbeitung SOLL Datenlokalität berücksichtigen.

---

## 198. HPC

`NPSPEC-COMPUTE-HPC-0001` SOLL für große medizinische Bildverarbeitungs-Workloads verwendet werden können.

---

## 199. Resampling

DICOM-Volumen SOLLEN räumlich resampled werden können.

---

## 200. Nearest Neighbor

Nearest-Neighbor-Interpolation SOLL unterstützt werden.

---

## 201. Linear Interpolation

Lineare beziehungsweise trilineare Interpolation SOLL unterstützt werden.

---

## 202. Higher-Order Interpolation

Höherwertige Interpolationsverfahren DÜRFEN unterstützt werden.

---

## 203. Label Resampling

Segmentierungslabels SOLLEN standardmäßig mit labelgeeigneten Interpolationsmethoden verarbeitet werden.

---

## 204. Isotropic Resampling

Volumendaten SOLLEN auf isotrope Voxelauflösung resampled werden können.

---

## 205. Coordinate Preservation

Resampling MUSS räumliche Transformationen korrekt aktualisieren.

---

## 206. Cropping

Bilder und Volumen SOLLEN räumlich zugeschnitten werden können.

---

## 207. Padding

Padding SOLL unterstützt werden.

---

## 208. Flipping

Achsen dürfen transformiert werden, sofern die räumliche Geometrie entsprechend aktualisiert wird.

---

## 209. Rotation

Rotationen MÜSSEN mit einer korrekten Aktualisierung des Koordinatensystems verbunden sein.

---

## 210. Reorientation

Volumendaten SOLLEN in definierte Orientierungssysteme umorientiert werden können.

---

## 211. Orientation Preservation

Eine Umorientierung DARF nicht stillschweigend die ursprüngliche Orientierung verlieren.

---

## 212. Multiplanar Reconstruction

Volumen SOLLEN für:

- axial,
- sagittal,
- coronal,
- oblique

rekonstruiert werden können.

---

## 213. MPR

Multiplanar Reconstruction wird abgekürzt:

`MPR`

---

## 214. Maximum Intensity Projection

Maximum Intensity Projection SOLL unterstützt werden können.

Abkürzung:

`MIP`

---

## 215. Minimum Intensity Projection

Minimum Intensity Projection SOLL unterstützt werden können.

---

## 216. Average Intensity Projection

Average Intensity Projection SOLL unterstützt werden können.

---

## 217. Volume Rendering Preparation

DICOM Compute SOLL Volumendaten für Volume Rendering vorbereiten können.

Die eigentliche Darstellung DARF durch ein separates Graphics-Modul erfolgen.

---

## 218. Histogram

Pixel- und Voxelhistogramme SOLLEN berechnet werden können.

---

## 219. Statistics

Mindestens folgende Statistiken SOLLEN verfügbar sein:

- Minimum,
- Maximum,
- Mean,
- Standard Deviation,
- Median optional,
- Percentiles optional.

---

## 220. Region Statistics

Statistiken SOLLEN auf definierte Regionen beschränkt werden können.

---

## 221. Mask

Eine Maske SOLL Pixel beziehungsweise Voxel selektieren können.

---

## 222. Segmentation Mask

DICOM-SEG-Daten SOLLEN als Compute-Masken verwendet werden können.

---

## 223. Measurement Units

Messwerte SOLLEN ihre Einheit erhalten.

---

## 224. Unit Safety

Werte unterschiedlicher physikalischer Einheiten DÜRFEN nicht ohne explizite Konvertierung vermischt werden.

---

## 225. Metadata Preservation

Nicht von einer Operation betroffene Metadaten SOLLEN erhalten bleiben.

---

## 226. Derived Metadata

Abgeleitete Objekte SOLLEN ihre Verarbeitungshistorie dokumentieren können.

---

## 227. Provenance

Eine Compute-Pipeline SOLL Provenance-Daten erzeugen können.

Mindestens:

- Source SOP Instance UID,
- Processing Operation,
- Runtime Version,
- Processing Timestamp,
- Configuration Hash.

---

## 228. Deterministic Mode

DICOM Compute SOLL einen deterministischen Verarbeitungsmodus unterstützen.

---

## 229. Deterministic Decode

Bei identischem Codec und identischen Eingabedaten SOLL eine verlustfreie Dekodierung reproduzierbare Pixelwerte erzeugen.

---

## 230. Lossy Input

Bei verlustbehaftet komprimierten Quelldaten MUSS der verlustbehaftete Ursprung nachvollziehbar bleiben.

---

## 231. Lossy Compression Flag

Vorhandene Informationen über Lossy Compression MÜSSEN erhalten und ausgewertet werden können.

---

## 232. Re-Encoding

Ein erneutes Encoding DARF nicht so dargestellt werden, als wären verlustbehaftete Originaldaten verlustfrei entstanden.

---

## 233. Validation

DICOM Compute MUSS DICOM-Daten validieren können.

---

## 234. Structural Validation

Strukturelle Validierung SOLL prüfen:

- Elementgrenzen,
- Längen,
- VR,
- Sequenzen,
- Items,
- Pixel Data,
- Transfer Syntax.

---

## 235. Semantic Validation

Semantische Validierung SOLL bekannte Beziehungen zwischen Attributen prüfen können.

---

## 236. Pixel Validation

Pixel-Metadaten und tatsächliche Pixelinformationen SOLLEN gegeneinander geprüft werden.

---

## 237. Geometry Validation

Seriengeometrie SOLL validiert werden.

---

## 238. UID Validation

UIDs SOLLEN syntaktisch validiert werden können.

---

## 239. Required Attribute Validation

Für bekannte SOP Classes SOLLEN erforderliche Attribute geprüft werden können.

---

## 240. Validation Profile

Validierungsregeln SOLLEN profilabhängig sein.

---

## 241. Strict Mode

Ein strenger Validierungsmodus SOLL verfügbar sein.

---

## 242. Tolerant Mode

Ein toleranter Lesemodus DARF nicht-konforme, aber eindeutig interpretierbare Daten verarbeiten.

---

## 243. Tolerant Mode Diagnostics

Jede tolerierte Abweichung MUSS diagnostizierbar bleiben.

---

## 244. Silent Repair

Fehlerhafte DICOM-Daten DÜRFEN nicht stillschweigend verändert werden.

---

## 245. Repair Mode

Ein expliziter Repair Mode DARF bekannte korrigierbare Probleme beheben.

---

## 246. Repair Provenance

Jede automatische Reparatur MUSS protokollierbar und nachvollziehbar sein.

---

## 247. De-Identification

DICOM Compute MUSS eine Infrastruktur zur De-Identifikation bereitstellen können.

---

## 248. De-Identification Policy

De-Identifikation MUSS über explizite Policies erfolgen.

---

## 249. Remove

Attribute SOLLEN entfernt werden können.

---

## 250. Replace

Attribute SOLLEN durch definierte Ersatzwerte ersetzt werden können.

---

## 251. Hash

Geeignete Identifikatoren DÜRFEN deterministisch pseudonymisiert werden.

---

## 252. UID Remapping

UIDs SOLLEN konsistent neu erzeugt beziehungsweise abgebildet werden können.

---

## 253. Referential Integrity

Beim UID-Remapping MÜSSEN interne Referenzen konsistent bleiben.

---

## 254. Date Handling

Datumsinformationen SOLLEN entsprechend einer De-Identification-Policy entfernt oder konsistent verschoben werden können.

---

## 255. Private Attributes

Private Tags MÜSSEN bei De-Identifikation gesondert behandelt werden.

---

## 256. Burned-In Annotation

DICOM Compute MUSS berücksichtigen, dass personenbezogene Informationen direkt in Pixeln eingebrannt sein können.

---

## 257. Pixel De-Identification

Pixelbasierte De-Identifikation SOLL als separate Verarbeitungspipeline möglich sein.

---

## 258. Metadata-Only De-Identification

Eine reine Metadaten-De-Identifikation DARF nicht behaupten, dass Pixelinformationen frei von identifizierenden Informationen sind.

---

## 259. De-Identification Verification

De-identifizierte Datensätze SOLLEN erneut gegen eine definierte Policy geprüft werden können.

---

## 260. Original Data

De-Identifikation DARF Originaldaten standardmäßig nicht überschreiben.

---

## 261. Security

DICOM Compute MUSS alle Eingabedaten als potenziell nicht vertrauenswürdig behandeln.

---

## 262. Bounds Checking

Parser und Codecs MÜSSEN Längen- und Bereichsprüfungen durchführen.

---

## 263. Integer Overflow

Größenberechnungen MÜSSEN gegen Integer Overflows geschützt sein.

---

## 264. Allocation Limits

Dateiinhalte DÜRFEN keine unbeschränkten Speicherallokationen erzwingen.

---

## 265. Decompression Bomb Protection

Komprimierte Pixel Data MUSS gegen unverhältnismäßig große Dekodierausgaben geschützt verarbeitet werden.

---

## 266. Maximum Dimensions

Konfigurierbare Grenzwerte für:

- Rows,
- Columns,
- Frames,
- Sequence Depth,
- Element Length,
- Pixel Data Size

SOLLTEN existieren.

---

## 267. Codec Isolation

Unsichere oder externe Codecs SOLLEN isoliert ausgeführt werden können.

---

## 268. Parser Failure

Ein Parserfehler DARF keinen teilweise validierten Datensatz als vollständig gültig markieren.

---

## 269. Diagnostics

DICOM Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 270. Diagnostic Classes

Mindestens folgende Diagnoseklassen SOLLEN existieren:

- INVALID_DICOM,
- INVALID_PREAMBLE,
- INVALID_META_HEADER,
- INVALID_TAG,
- INVALID_VR,
- INVALID_LENGTH,
- INVALID_SEQUENCE,
- INVALID_UID,
- UNSUPPORTED_TRANSFER_SYNTAX,
- UNSUPPORTED_CODEC,
- PIXEL_DATA_CORRUPT,
- PIXEL_METADATA_MISMATCH,
- FRAME_DECODE_FAILED,
- INVALID_GEOMETRY,
- INCONSISTENT_SERIES,
- MISSING_SLICE,
- DUPLICATE_SLICE,
- NON_UNIFORM_SPACING,
- CHARACTER_SET_ERROR,
- DEIDENTIFICATION_WARNING,
- BURNED_IN_ANNOTATION_RISK,
- OUT_OF_MEMORY,
- DEVICE_FAILURE,
- IO_FAILURE.

---

## 271. Diagnostic Severity

Mindestens:

- INFO,
- WARNING,
- ERROR,
- FATAL

SOLLTEN unterstützt werden.

---

## 272. Diagnostic Location

Eine Diagnose SOLL die Position innerhalb eines Datensatzes referenzieren können.

Beispiele:

- File Offset,
- Tag,
- Sequence Path,
- Frame Number,
- SOP Instance UID.

---

## 273. Sequence Path

Fehler innerhalb verschachtelter Sequenzen SOLLEN über einen vollständigen Elementpfad referenzierbar sein.

---

## 274. Pixel Diagnostic

Pixelbezogene Fehler SOLLEN mindestens referenzieren können:

- Frame,
- Fragment,
- Transfer Syntax,
- Codec.

---

## 275. Series Diagnostic

Serienbezogene Fehler SOLLEN die betroffenen Instanzen identifizieren können.

---

## 276. Logging Privacy

Diagnostik DARF nicht unnötig patientenidentifizierende Informationen in Logs schreiben.

---

## 277. Redacted Diagnostics

Diagnosen SOLLEN eine datenschutzfreundliche Darstellung unterstützen.

---

## 278. Profiling

DICOM Compute MUSS Profiling unterstützen.

---

## 279. Profiling Categories

Mindestens:

- File Read,
- Parse,
- Metadata Decode,
- Pixel Decode,
- Geometry,
- Series Sorting,
- Volume Reconstruction,
- Rescale,
- Windowing,
- Resampling,
- De-Identification,
- Validation,
- Export.

---

## 280. Throughput

Verarbeitungsleistung SOLL messbar sein.

Beispiele:

- Instances/s,
- Frames/s,
- MB/s,
- Voxels/s.

---

## 281. Decode Throughput

Codec-Leistung SOLL getrennt von I/O gemessen werden können.

---

## 282. GPU Profiling

GPU-Verarbeitungszeit SOLL separat messbar sein.

---

## 283. Cache Statistics

Cache Hit Rate und Cache Miss Rate SOLLEN messbar sein.

---

## 284. Memory Statistics

Peak Memory und aktuelle Speicherbelegung SOLLEN messbar sein.

---

## 285. Checkpointing

Lange DICOM-Compute-Pipelines SOLLEN Checkpointing unterstützen können.

---

## 286. Pipeline Checkpoint

Ein Checkpoint SOLL mindestens speichern können:

- Pipeline ID,
- Input Set Identity,
- Completed Operations,
- Pending Operations,
- Configuration Hash.

---

## 287. Restart

Unterbrochene Batch-Verarbeitungen SOLLEN aus kompatiblen Checkpoints fortgesetzt werden können.

---

## 288. Input Validation on Restart

Beim Restart MUSS geprüft werden können, ob sich die Eingabedaten geändert haben.

---

## 289. File Fingerprint

Eingabedaten SOLLEN über einen Fingerprint identifiziert werden können.

---

## 290. Object Model Integration

DICOM Compute MUSS mit `NPSPEC-COMPUTE-RUNTIME-0004` integrierbar sein.

---

## 291. Compute Objects

Mindestens folgende logische Objekte SOLLEN verfügbar sein:

- DICOMFile,
- DICOMDataSet,
- DICOMElement,
- DICOMSequence,
- DICOMPixelData,
- DICOMFrame,
- DICOMInstance,
- DICOMSeries,
- DICOMStudy,
- DICOMVolume,
- DICOMSegmentation,
- DICOMTransform.

---

## 292. Object Lifetime

Objektlebensdauer MUSS unabhängig von temporären Parserpuffern kontrollierbar sein.

---

## 293. Immutable Metadata View

Eine unveränderliche Metadatenansicht SOLL verfügbar sein.

---

## 294. Mutable Data Set

Explizite Bearbeitung SOLL über kontrollierte Mutable-Operationen erfolgen.

---

## 295. Copy-on-Write

Große DICOM-Objekte DÜRFEN Copy-on-Write verwenden.

---

## 296. Serialization

Compute-interne DICOM-Objekte SOLLEN serialisierbar sein, sofern dies für Caching oder Distributed Compute erforderlich ist.

---

## 297. Standard DICOM Output

DICOM-Ausgabe MUSS von interner Compute-Serialisierung getrennt bleiben.

---

## 298. Writer

Eine vollständige Implementierung SOLL einen DICOM Writer bereitstellen können.

---

## 299. Writer Validation

Ausgegebene DICOM-Dateien MÜSSEN vor Veröffentlichung strukturell validierbar sein.

---

## 300. Transfer Syntax Selection

Beim Schreiben MUSS die gewünschte Transfer Syntax explizit gewählt oder durch eine definierte Policy bestimmt werden.

---

## 301. Lossless Default

Eine Implementierung SOLL bei Transformationen nicht ohne explizite Anforderung auf verlustbehaftete Kompression wechseln.

---

## 302. Metadata Editing

Attribute SOLLEN hinzugefügt, geändert und entfernt werden können.

---

## 303. UID Generation

Neue SOP Instances MÜSSEN gültige UIDs erhalten können.

---

## 304. UID Collision Protection

Neu erzeugte UIDs SOLLEN eine praktisch eindeutige Identität gewährleisten.

---

## 305. Derived Instance

Abgeleitete DICOM-Instanzen SOLLEN als solche kennzeichenbar sein.

---

## 306. Source References

Abgeleitete Instanzen SOLLEN ihre Quellen referenzieren können.

---

## 307. DICOM Directory Input

DICOM Compute SOLL Verzeichnisse mit vielen DICOM-Dateien indexieren können.

---

## 308. Recursive Scan

Rekursive Verzeichnissuche SOLL unterstützt werden können.

---

## 309. Extension Independence

Dateierkennung DARF nicht ausschließlich auf Dateiendungen basieren.

---

## 310. File Header Detection

Dateiinhalte SOLLEN zur Erkennung herangezogen werden.

---

## 311. Metadata Index

Ein temporärer Metadatenindex SOLL aufgebaut werden können.

---

## 312. Study Grouping

Instanzen SOLLEN nach Study Instance UID gruppiert werden können.

---

## 313. Series Grouping

Instanzen SOLLEN nach Series Instance UID gruppiert werden können.

---

## 314. SOP Deduplication

Doppelte SOP Instance UIDs SOLLEN erkannt werden.

---

## 315. Duplicate Policy

Der Umgang mit Duplikaten MUSS über eine Policy steuerbar sein.

---

## 316. Conflicting Duplicate

Unterschiedliche Dateien mit gleicher SOP Instance UID SOLLEN als Konflikt diagnostiziert werden.

---

## 317. DICOMDIR

DICOMDIR SOLL unterstützt werden können.

---

## 318. DICOMDIR Independence

Ein Datenbestand DARF auch ohne DICOMDIR indexiert werden können.

---

## 319. Archive Processing

Große DICOM-Datenarchive SOLLEN batchweise verarbeitet werden können.

---

## 320. Progress Reporting

Batch-Verarbeitung MUSS Fortschritt melden können.

Mindestens:

- Files Scanned,
- Instances Parsed,
- Frames Decoded,
- Bytes Processed,
- Errors,
- Warnings.

---

## 321. Cancellation

DICOM-Compute-Aufgaben MÜSSEN kontrolliert abbrechbar sein.

---

## 322. Cancellation Safety

Ein Abbruch DARF keine teilweise erzeugte Ausgabe als vollständig markieren.

---

## 323. Transactional Output

Ausgabeoperationen SOLLEN transaktional beziehungsweise atomar veröffentlicht werden können.

---

## 324. Temporary Output

Unvollständige Ausgaben SOLLEN als temporär markiert bleiben.

---

## 325. Testing — Parser

Mindestens folgende Fälle MÜSSEN getestet werden:

- Explicit VR Little Endian,
- Implicit VR Little Endian,
- Sequences,
- Nested Sequences,
- Unknown Tags,
- Private Tags,
- Undefined Length Elements,
- malformed lengths.

---

## 326. Testing — Pixel Data

Mindestens:

- 8-bit unsigned,
- 16-bit unsigned,
- 16-bit signed,
- MONOCHROME1,
- MONOCHROME2,
- RGB,
- Single Frame,
- Multi Frame.

---

## 327. Testing — Compression

Für jeden implementierten Codec MÜSSEN gültige und beschädigte Testdaten vorhanden sein.

---

## 328. Testing — Geometry

Mindestens:

- axial series,
- reversed slice order,
- oblique series,
- missing slice,
- duplicate slice,
- non-uniform spacing,
- inconsistent orientation.

---

## 329. Testing — Volume

Volumenrekonstruktion MUSS auf:

- Dimensions,
- Spacing,
- Origin,
- Orientation,
- Voxel Values

geprüft werden.

---

## 330. Testing — Rescale

Rescale Slope und Rescale Intercept MÜSSEN getestet werden.

---

## 331. Testing — Windowing

Window Center und Window Width MÜSSEN mit definierten Referenzwerten getestet werden.

---

## 332. Testing — Character Sets

Mindestens ASCII und relevante DICOM-Zeichensatzkonfigurationen SOLLEN getestet werden.

---

## 333. Testing — De-Identification

Mindestens:

- Remove Attribute,
- Replace Attribute,
- UID Remap,
- Reference Preservation,
- Date Shift,
- Private Tag Policy

MÜSSEN getestet werden.

---

## 334. Testing — Security

Mindestens:

- truncated file,
- oversized element,
- invalid sequence length,
- excessive nesting,
- integer overflow attempt,
- decompression bomb,
- corrupt fragments

MÜSSEN getestet werden.

---

## 335. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU getestet werden.

---

## 336. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen übereinstimmen.

---

## 337. Testing — Determinism

Deterministische Verarbeitung MUSS bei identischen Eingaben reproduzierbare Ergebnisse liefern, soweit die verwendeten Algorithmen deterministisch definiert sind.

---

## 338. Testing — Large Data

Große Studien und Multiframe-Objekte SOLLEN getestet werden.

---

## 339. Stress Tests

Mindestens:

- ManySmallFiles,
- VeryLargeFile,
- LargeMultiframe,
- DeepSequences,
- LargeVolume,
- ParallelDecode,
- MemoryPressure,
- RepeatedOpenClose,
- Cancellation,
- CorruptInputBatch

SOLLTEN getestet werden.

---

## 340. Fuzz Testing

DICOM Parser und Decoder-Integrationsschicht SOLLEN mit Fuzz Testing geprüft werden.

---

## 341. Reference Datasets

Tests SOLLEN standardkonforme Referenzdatensätze verwenden.

---

## 342. Regression Tests

Jeder behobene Parser- oder Decoderfehler SOLL durch einen Regressionstest abgesichert werden.

---

## 343. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

- DICOM-Dateierkennung,
- DICOM Data Sets,
- Data Elements,
- Tags,
- Explicit VR Little Endian,
- Implicit VR Little Endian,
- Sequenzen,
- private und unbekannte Tags,
- Single-Frame Pixel Data,
- Multiframe-Architektur,
- MONOCHROME1,
- MONOCHROME2,
- 8- und 16-Bit-Pixeldaten,
- Signed und Unsigned Pixel Representation,
- Rescale Slope,
- Rescale Intercept,
- Window Center,
- Window Width,
- Study Instance UID,
- Series Instance UID,
- SOP Class UID,
- SOP Instance UID,
- Pixel Spacing,
- Image Position Patient,
- Image Orientation Patient,
- geometrische Slice-Sortierung,
- Volumenrekonstruktion,
- CPU-Verarbeitung,
- Multi-Core-Architektur,
- Streaming beziehungsweise Deferred Pixel Data,
- strukturierte Diagnostik,
- Validierung,
- De-Identification-Infrastruktur,
- Speicherlimits,
- kontrollierten Abbruch.

---

## 344. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

- JPEG,
- JPEG Lossless,
- JPEG-LS,
- JPEG 2000,
- RLE,
- Enhanced Multi-Frame,
- Functional Groups,
- 4D-Daten,
- DICOM SEG,
- Structured Reports,
- Parametric Maps,
- Spatial Registration,
- Real World Value Mapping,
- RGB,
- YBR,
- Palette Color,
- GPU-Verarbeitung,
- NPU-Integration,
- Distributed Compute,
- HPC,
- MPR,
- MIP,
- Resampling,
- Isotropic Resampling,
- Volume Processing,
- Pixel De-Identification,
- DICOM Writer,
- DICOMDIR,
- In-Situ Processing,
- Zero-Copy Compute Integration.

---

## 345. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-DICOM-0001` MUSS:

1. DICOM-Datensätze erkennen können.
2. DICOM Data Elements parsen können.
3. Tags verlustfrei repräsentieren.
4. bekannte und unbekannte Tags unterscheiden können.
5. private Tags erhalten können.
6. Value Representations verarbeiten können.
7. Value Multiplicity unterstützen.
8. Sequenzen verarbeiten.
9. verschachtelte Sequenzen verarbeiten.
10. Explicit VR Little Endian unterstützen.
11. Implicit VR Little Endian unterstützen.
12. Transfer Syntax UIDs auswerten.
13. nicht unterstützte Transfer Syntaxes diagnostizieren.
14. Pixel Data repräsentieren.
15. native Pixel Data dekodieren.
16. Multiframe-Daten architektonisch unterstützen.
17. Rows und Columns berücksichtigen.
18. Samples Per Pixel berücksichtigen.
19. Bits Allocated berücksichtigen.
20. Bits Stored berücksichtigen.
21. High Bit berücksichtigen.
22. Pixel Representation berücksichtigen.
23. Photometric Interpretation berücksichtigen.
24. MONOCHROME1 unterstützen.
25. MONOCHROME2 unterstützen.
26. Stored Pixel Values erhalten.
27. Rescale Slope unterstützen.
28. Rescale Intercept unterstützen.
29. Window Center unterstützen.
30. Window Width unterstützen.
31. Darstellungstransformationen von Rohdaten trennen.
32. Study Instance UID verarbeiten.
33. Series Instance UID verarbeiten.
34. SOP Class UID verarbeiten.
35. SOP Instance UID verarbeiten.
36. DICOM-Serien gruppieren.
37. Bildgeometrie bestimmen.
38. Pixel Spacing berücksichtigen.
39. Image Position Patient berücksichtigen.
40. Image Orientation Patient berücksichtigen.
41. Slice-Normalen bestimmen.
42. Slices geometrisch sortieren.
43. Slice-Abstände bestimmen.
44. fehlende Slices diagnostizieren können.
45. doppelte Slices diagnostizieren können.
46. inkonsistente Orientierung diagnostizieren können.
47. kompatible Serien zu Volumen rekonstruieren.
48. Voxel Spacing erhalten.
49. Volume Origin erhalten.
50. Volume Orientation erhalten.
51. Voxel-to-Patient-Transformation bereitstellen.
52. inkompatible Serien nicht stillschweigend zusammenführen.
53. CPU-Verarbeitung unterstützen.
54. Multi-Core-Verarbeitung architektonisch unterstützen.
55. GPU-Verarbeitung architektonisch unterstützen.
56. das Compute Memory Model integrieren.
57. den Compute Scheduler integrieren.
58. das Compute Object Model integrieren.
59. Deferred Pixel Data ermöglichen.
60. Metadata-Only Parsing ermöglichen.
61. Speicherlimits erzwingen können.
62. Integer-Overflow-Schutz besitzen.
63. Bounds Checking durchführen.
64. beschädigte Eingaben kontrolliert ablehnen.
65. strukturierte Diagnosen bereitstellen.
66. patientenbezogene Daten in Diagnosen minimieren können.
67. DICOM-Daten validieren können.
68. De-Identification-Policies unterstützen.
69. UID-Remapping konsistent durchführen können.
70. interne Referenzen beim UID-Remapping erhalten.
71. Originaldaten standardmäßig unverändert lassen.
72. Verarbeitung kontrolliert abbrechen können.
73. unvollständige Ausgabe von vollständiger Ausgabe unterscheiden.
74. numerische Pixeltransformationen reproduzierbar durchführen können.
75. Provenance für abgeleitete Daten unterstützen.
76. unbekannte Daten nicht ohne explizite Policy verwerfen.
77. Codec-Implementierung vom Parser trennen.
78. Bilddarstellung von quantitativen Pixelwerten trennen.
79. physikalische und räumliche Metadaten bei Compute-Konvertierungen erhalten.
80. Erweiterungen ohne Änderung des grundlegenden DICOM-Objektmodells ermöglichen.

---

## 346. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

    NovaDICOMCompute
     |
     +-- DICOMRuntime
     |
     +-- DICOMParser
     |
     +-- DICOMWriter
     |
     +-- DICOMDictionary
     |
     +-- TransferSyntaxManager
     |
     +-- CodecManager
     |
     +-- PixelDataManager
     |
     +-- FrameDecoder
     |
     +-- SeriesManager
     |
     +-- GeometryManager
     |
     +-- VolumeBuilder
     |
     +-- PixelPipeline
     |
     +-- ResamplingEngine
     |
     +-- DeIdentificationEngine
     |
     +-- ValidationEngine
     |
     +-- DICOMDiagnostics
     |
     +-- DICOMProfiler

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 347. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

DICOM Compute verwendet die allgemeine NovaOS Compute Runtime für rechenintensive Operationen.

    DICOM Data
        |
        v
    DICOM Compute
        |
        v
    Compute Runtime
        |
        +--> CPU
        +--> SIMD
        +--> GPU
        +--> NPU
        +--> Distributed

---

## 348. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Compute Scheduler verwaltet parallelisierbare DICOM-Operationen.

Beispiele:

    Parse
    Decode
    Rescale
    Resample
    Validate
    De-Identify

---

## 349. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Große Pixel-, Frame- und Volumenpuffer werden über das gemeinsame Compute Memory Model verwaltet.

---

## 350. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

DICOM-Objekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 351. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime SOLL für massiv parallele Pixel- und Volumenoperationen verwendet werden können.

---

## 352. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime DARF für medizinische KI-Modelle verwendet werden.

DICOM Compute bleibt dabei für:

- Dekodierung,
- Geometrie,
- Metadaten,
- Pixelinterpretation,
- Datenvorbereitung

zuständig.

---

## 353. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute SOLL große Studienarchive oder rechenintensive Bildpipelines auf mehrere Nodes verteilen können.

---

## 354. Beziehung zu `NPSPEC-COMPUTE-HPC-0001`

HPC Compute SOLL insbesondere für:

- große 3D-Volumen,
- 4D-Daten,
- Batch-Analyse,
- Rekonstruktionen,
- quantitative Bildanalyse,
- große KI-Pipelines

verwendet werden können.

---

## 355. Grundlegender DICOM-Verarbeitungsablauf

Der normative konzeptionelle Ablauf lautet:

    Input
      |
      v
    Detect
      |
      v
    Parse
      |
      v
    Validate
      |
      v
    Metadata
      |
      v
    Pixel Data
      |
      v
    Decode
      |
      v
    Geometry
      |
      v
    Series / Volume
      |
      v
    Compute Processing
      |
      v
    Derived Output

Nicht jede Pipeline MUSS alle Schritte ausführen.

Eine reine Metadatenabfrage DARF beispielsweise vor Pixel Decode enden.

---

## 356. Pixelverarbeitungsmodell

Die grundlegende Trennung lautet:

    Encoded Pixel Data
            |
            v
          Decode
            |
            v
       Stored Values
            |
            v
    Modality Transform
            |
            v
    Quantitative Values
            |
            +----------------+
            |                |
            v                v
       Analysis          VOI / Display

Dadurch bleiben quantitative Analyse und Bildschirmdarstellung voneinander getrennt.

---

## 357. Serienmodell

Die Gruppierung lautet grundsätzlich:

    Study
      |
      +-- Series
           |
           +-- Instance
           +-- Instance
           +-- Instance
                |
                v
             Frames

---

## 358. Volumenmodell

Eine Serie darf erst nach geometrischer Prüfung als Volumen interpretiert werden:

    Series
      |
      v
    Geometry Validation
      |
      v
    Slice Sorting
      |
      v
    Spacing Validation
      |
      v
    Volume Reconstruction

---

## 359. Datenschutzmodell

Die grundlegende Trennung lautet:

    Original DICOM
         |
         +------------------+
         |                  |
         v                  v
    Clinical Use       De-Identification
                            |
                            v
                     Derived Data Set

De-identifizierte Daten MÜSSEN als abgeleitete Daten behandelt werden.

---

## 360. Performance-Grundsatz

DICOM Compute SOLL unnötige Datenkopien vermeiden.

Die bevorzugte Verarbeitung lautet:

    File
      |
      v
    Metadata Parse
      |
      v
    Deferred Pixel Access
      |
      v
    Decode Required Frames
      |
      v
    Compute Buffer
      |
      v
    Processing

anstatt grundsätzlich:

    Read Entire File
      |
      v
    Decode Everything
      |
      v
    Copy Everything

---

## 361. Erweiterbarkeit

Neue:

- SOP Classes,
- Transfer Syntaxes,
- Codecs,
- Modalitäten,
- Pixel-Pipelines,
- Validierungsprofile,
- De-Identification-Policies,
- Compute-Operationen

SOLLTEN integrierbar sein, ohne den grundlegenden DICOM-Parser neu entwerfen zu müssen.

---

## 362. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

    NPSPEC-COMPUTE-DICOM-PARSER-0001
    DICOM Parser

    NPSPEC-COMPUTE-DICOM-DATASET-0001
    DICOM Data Set Model

    NPSPEC-COMPUTE-DICOM-DICTIONARY-0001
    DICOM Data Dictionary

    NPSPEC-COMPUTE-DICOM-PIXEL-0001
    DICOM Pixel Data

    NPSPEC-COMPUTE-DICOM-CODEC-0001
    DICOM Codec Architecture

    NPSPEC-COMPUTE-DICOM-GEOMETRY-0001
    DICOM Spatial Geometry

    NPSPEC-COMPUTE-DICOM-SERIES-0001
    Study and Series Management

    NPSPEC-COMPUTE-DICOM-VOLUME-0001
    Volume Reconstruction

    NPSPEC-COMPUTE-DICOM-MULTIFRAME-0001
    Enhanced Multi-Frame DICOM

    NPSPEC-COMPUTE-DICOM-SEG-0001
    DICOM Segmentation

    NPSPEC-COMPUTE-DICOM-SR-0001
    Structured Reports

    NPSPEC-COMPUTE-DICOM-RESAMPLE-0001
    Medical Image Resampling

    NPSPEC-COMPUTE-DICOM-DEID-0001
    DICOM De-Identification

    NPSPEC-COMPUTE-DICOM-VALIDATION-0001
    DICOM Validation

    NPSPEC-COMPUTE-DICOM-SECURITY-0001
    DICOM Input Security

    NPSPEC-COMPUTE-DICOM-GPU-0001
    GPU Medical Imaging Backend

    NPSPEC-COMPUTE-DICOM-TEST-0001
    DICOM Conformance Tests

---

## 363. Zusammenfassung

`NPSPEC-COMPUTE-DICOM-0001` definiert die gemeinsame DICOM-Compute-Infrastruktur von NovaOS.

Die Architektur behandelt DICOM nicht lediglich als Bilddateiformat.

Ein DICOM-Objekt besteht vielmehr aus:

    Metadata
    +
    Pixel Data
    +
    Geometry
    +
    Semantic Relationships

Die zentrale Verarbeitung lautet:

    DICOM
      |
      v
    Parse
      |
      v
    Validate
      |
      v
    Decode
      |
      v
    Geometry
      |
      v
    Series
      |
      v
    Volume
      |
      v
    Compute

Dabei bleiben:

    File Format
    Data Set
    Metadata
    Pixel Data
    Codec
    Geometry
    Series
    Volume
    Compute Backend

logisch voneinander getrennt.

DICOM Compute kann dadurch medizinische Bilddaten von einfachen Einzelbildern bis zu großen 3D- und 4D-Datensätzen verarbeiten.

Die Compute-Ausführung kann skalieren über:

    CPU
    SIMD
    GPU
    NPU
    HPC
    Distributed Compute

Das grundlegende Architekturprinzip lautet:

> NovaOS DICOM Compute behandelt medizinische Bilddaten als strukturierte, räumlich definierte und semantisch verknüpfte Compute-Daten. Originalwerte, quantitative Transformationen, Darstellungsoperationen, Metadaten, Geometrie und abgeleitete Daten bleiben klar voneinander getrennt. Dadurch können DICOM-Daten sicher, reproduzierbar und skalierbar für Visualisierung, quantitative Bildverarbeitung, wissenschaftliche Analyse und weitere NovaOS-Compute-Fähigkeiten verwendet werden, ohne DICOM auf ein gewöhnliches Bilddateiformat zu reduzieren.
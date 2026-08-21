# NPSPEC-COMPUTE-ULTRASOUND-0001 — Ultrasound Compute

**Status:** Angenommen  
**Version:** 0.1  
**Kategorie:** Compute / Medical Imaging / Ultrasound  
**Komponente:** NovaOS Ultrasound Compute  
**Bezeichner:** `NPSPEC-COMPUTE-ULTRASOUND-0001`  
**Übergeordnete Spezifikation:** `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Abhängigkeiten:** `NPSPEC-COMPUTE-RUNTIME-0001`, `NPSPEC-COMPUTE-RUNTIME-0002`, `NPSPEC-COMPUTE-RUNTIME-0003`, `NPSPEC-COMPUTE-RUNTIME-0004`, `NPSPEC-COMPUTE-DICOM-0001`, `NPSPEC-COMPUTE-RADIOLOGY-0001`  
**Verwandte Spezifikationen:** `NPSPEC-COMPUTE-RUNTIME-0005`, `NPSPEC-COMPUTE-RUNTIME-0006`, `NPSPEC-COMPUTE-RUNTIME-0007`, `NPSPEC-COMPUTE-PACS-0001`, `NPSPEC-COMPUTE-HPC-0001`  
**Kurzname:** Ultrasound Compute  

---

## 1. Zweck

Diese Spezifikation definiert die Ultrasound-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Ultrasound Compute stellt spezialisierte numerische und signalverarbeitende Funktionen für Ultraschalldaten bereit.

Die Komponente baut auf der allgemeinen Radiology-Compute-Infrastruktur auf und erweitert diese um ultraschallspezifische Datenmodelle, Signalverarbeitung, Rekonstruktion und quantitative Analyse.

Die grundlegende Architektur lautet:

```text
Ultrasound Acquisition
        |
        v
Raw / Beamformed Data
        |
        v
Ultrasound Compute
        |
        +--> Signal Processing
        +--> Beamforming
        +--> Envelope Detection
        +--> Scan Conversion
        +--> B-Mode
        +--> M-Mode
        +--> Doppler
        +--> Color Doppler
        +--> Power Doppler
        +--> Spectral Doppler
        +--> Elastography
        +--> 3D / 4D Reconstruction
        +--> Measurement
        +--> AI Processing
```

Ultrasound Compute MUSS technische Signal- und Bildverarbeitung von medizinischer Interpretation trennen.

---

## 2. Designprinzip

Das zentrale Designprinzip lautet:

> Ultraschalldaten werden nicht ausschließlich als fertige Bilder behandelt, sondern abhängig von der verfügbaren Datenquelle als zeitabhängige akustische Messdaten mit definierter Kanal-, Abtast-, Sonden-, Strahl-, Raum- und Geschwindigkeitsgeometrie.

Die Architektur MUSS deshalb sowohl bereits rekonstruierte Ultraschallbilder als auch niedrigere Datenebenen unterstützen können.

---

## 3. Datenebenen

Ultrasound Compute SOLL mindestens folgende Datenebenen unterscheiden können:

```text
Raw Channel Data
RF Data
IQ Data
Beamformed Data
Envelope Data
Scan Converted Data
Display Image
Derived Quantitative Data
```

---

## 4. Raw Channel Data

Unverarbeitete Empfangsdaten einzelner Transducer-Elemente werden als:

`UltrasoundChannelData`

repräsentiert.

---

## 5. RF Data

Hochfrequente Radio-Frequency-Ultraschalldaten werden als:

`UltrasoundRFData`

repräsentiert.

---

## 6. IQ Data

Demodulierte komplexe In-Phase/Quadrature-Daten werden als:

`UltrasoundIQData`

repräsentiert.

---

## 7. Beamformed Data

Nach Beamforming erzeugte Daten werden als:

`UltrasoundBeamformedData`

repräsentiert.

---

## 8. Envelope Data

Nach Hüllkurvendetektion erzeugte Daten werden als:

`UltrasoundEnvelopeData`

repräsentiert.

---

## 9. Scan Converted Data

In kartesische Bildgeometrie transformierte Daten werden als:

`UltrasoundImage`

repräsentiert.

---

## 10. Original Data Preservation

Originale Eingangsdaten MÜSSEN von daraus erzeugten Zwischen- und Ergebnisdaten unterscheidbar bleiben.

---

## 11. Ziele

Ultrasound Compute MUSS architektonisch mindestens folgende Bereiche unterstützen:

1. 2D-Ultraschall,
2. B-Mode,
3. M-Mode,
4. Doppler,
5. Color Doppler,
6. Power Doppler,
7. Spectral Doppler,
8. Pulsed-Wave Doppler,
9. Continuous-Wave Doppler,
10. Duplex,
11. Triplex,
12. RF-Daten,
13. IQ-Daten,
14. Beamforming,
15. Envelope Detection,
16. Demodulation,
17. Scan Conversion,
18. Dynamic Range Compression,
19. Time Gain Compensation,
20. Filterung,
21. Speckle Processing,
22. 3D-Ultraschall,
23. 4D-Ultraschall,
24. Volumenrekonstruktion,
25. Elastography,
26. Motion Tracking,
27. Messungen,
28. Geschwindigkeitsanalyse,
29. Flow Analysis,
30. Cine Loops,
31. DICOM-Integration,
32. CPU-Ausführung,
33. GPU-Ausführung,
34. NPU-Ausführung,
35. Streaming,
36. Echtzeitverarbeitung,
37. reproduzierbare Offline-Verarbeitung,
38. Provenance,
39. Diagnostics,
40. Erweiterbarkeit.

---

## 12. Nicht-Ziele

Diese Spezifikation definiert nicht vollständig:

- Hardwaresteuerung konkreter Ultraschallgeräte,
- elektrische Ansteuerung von Hochspannungspulsern,
- konkrete Transducer-Elektronik,
- medizinische Diagnose,
- automatische Therapieentscheidungen,
- Befunderstellung,
- konkrete Viewer-Oberflächen,
- regulatorische Zulassung,
- konkrete KI-Modelle.

---

## 13. Sicherheitsgrenze

Ultrasound Compute ist primär eine Datenverarbeitungsinfrastruktur.

Direkte Kontrolle physischer Ultraschall-Sendehardware MUSS durch separate Hardware-, Device-, Safety- und Acquisition-Spezifikationen definiert werden.

---

## 14. Acoustic Output Separation

Parameter zur akustischen Ausgangsleistung DÜRFEN nicht implizit durch reine Bildverarbeitungsalgorithmen verändert werden.

---

## 15. Clinical Interpretation Separation

Ein Compute-Ergebnis wie:

```text
Velocity
Flow Estimate
Strain Map
Segmentation
Measurement
Probability
```

DARF nicht automatisch als bestätigte medizinische Diagnose behandelt werden.

---

## 16. Ultrasound Study

Eine Ultraschalluntersuchung SOLL als:

`UltrasoundStudy`

repräsentiert werden.

---

## 17. Ultrasound Series

Eine zusammengehörige Datenserie SOLL als:

`UltrasoundSeries`

repräsentiert werden.

---

## 18. Ultrasound Frame

Ein zeitlicher Frame wird als:

`UltrasoundFrame`

repräsentiert.

---

## 19. Ultrasound Cine

Eine Folge von Frames wird als:

`UltrasoundCine`

repräsentiert.

---

## 20. Ultrasound Volume

Ein dreidimensionaler Ultraschalldatensatz wird als:

`UltrasoundVolume`

repräsentiert.

---

## 21. Ultrasound Volume Sequence

Zeitabhängige 3D-Volumen werden als:

`UltrasoundVolumeSequence`

repräsentiert.

---

## 22. Acquisition Metadata

Ultraschalldaten SOLLEN relevante Aufnahmeparameter speichern können.

---

## 23. Sampling Frequency

Die Abtastfrequenz SOLL explizit repräsentiert werden.

---

## 24. Center Frequency

Die nominelle beziehungsweise verwendete Mittenfrequenz SOLL repräsentierbar sein.

---

## 25. Bandwidth

Die Signalbandbreite SOLL repräsentierbar sein.

---

## 26. Sample Count

Die Anzahl der Samples MUSS bekannt sein.

---

## 27. Channel Count

Bei Mehrkanaldaten MUSS die Anzahl der Kanäle bekannt sein.

---

## 28. Frame Timestamp

Frames SOLLEN präzise Zeitinformationen besitzen.

---

## 29. Acquisition Timestamp

Aufnahmezeitpunkte SOLLEN von Verarbeitungszeitpunkten getrennt sein.

---

## 30. Temporal Order

Die Reihenfolge von Frames MUSS eindeutig rekonstruierbar sein.

---

## 31. Probe Model

Eine Ultraschallsonde SOLL als:

`UltrasoundProbe`

repräsentiert werden.

---

## 32. Probe Type

Mindestens folgende Sondentypen SOLLEN repräsentierbar sein:

```text
Linear
Curvilinear
PhasedArray
MatrixArray
Mechanical3D
Endocavitary
TEE
Other
```

---

## 33. Probe Geometry

Die geometrische Anordnung der Transducer-Elemente SOLL beschreibbar sein.

---

## 34. Element Count

Die Anzahl der aktiven beziehungsweise verfügbaren Elemente SOLL repräsentierbar sein.

---

## 35. Element Position

Die Position jedes Elements SOLL in einem Sondenkoordinatensystem darstellbar sein.

---

## 36. Element Pitch

Der Elementabstand SOLL repräsentierbar sein.

---

## 37. Element Width

Die aktive Elementbreite SOLL repräsentierbar sein.

---

## 38. Probe Coordinate System

Die Sonde MUSS ein eindeutig definiertes lokales Koordinatensystem besitzen können.

---

## 39. Patient Coordinate Mapping

Wenn verfügbar, SOLL die Beziehung zwischen Probe Space und Patient Space repräsentiert werden.

---

## 40. Tracked Ultrasound

Extern getrackte Ultraschallsonden SOLLEN unterstützt werden können.

---

## 41. Tracking Transform

Eine Tracking-Transformation SOLL mindestens enthalten:

```text
Timestamp
ProbeTransform
ReferenceTransform
CoordinateSystem
```

---

## 42. Synchronization

Tracking- und Ultraschalldaten MÜSSEN zeitlich synchronisierbar sein.

---

## 43. Speed of Sound

Die angenommene Schallgeschwindigkeit MUSS als expliziter Verarbeitungsparameter repräsentierbar sein.

Symbol:

```text
c
```

---

## 44. Default Speed of Sound

Eine Implementierung DARF einen Standardwert verwenden, MUSS diesen jedoch als Annahme kennzeichnen.

---

## 45. Time of Flight

Die grundlegende Laufzeitbeziehung lautet:

```text
d = c * t / 2
```

für ein einfaches Pulse-Echo-Modell mit Hin- und Rückweg.

---

## 46. Speed-of-Sound Assumption

Räumliche Distanzberechnungen aus Laufzeitdaten MÜSSEN die verwendete Schallgeschwindigkeit berücksichtigen.

---

## 47. Beam Model

Ein ausgesendeter beziehungsweise rekonstruierter Ultraschallstrahl wird als:

`UltrasoundBeam`

repräsentiert.

---

## 48. Beam Origin

Der Ursprung eines Beams MUSS definierbar sein.

---

## 49. Beam Direction

Die Strahlrichtung MUSS definierbar sein.

---

## 50. Beam Angle

Der Strahlwinkel SOLL repräsentierbar sein.

---

## 51. Beam Sequence

Die zeitliche Reihenfolge ausgesendeter beziehungsweise verarbeiteter Beams SOLL erhalten bleiben.

---

## 52. Receive Aperture

Die Empfangsapertur SOLL beschreibbar sein.

---

## 53. Transmit Aperture

Die Sendeapertur SOLL beschreibbar sein, sofern entsprechende Daten verfügbar sind.

---

## 54. Focus Depth

Fokustiefen SOLLEN repräsentierbar sein.

---

## 55. Dynamic Receive Focusing

Dynamische Empfangsfokussierung SOLL unterstützt werden können.

---

## 56. Beamforming

Ultrasound Compute SOLL Beamforming unterstützen können.

---

## 57. Delay-and-Sum

Delay-and-Sum-Beamforming SOLL als grundlegendes Verfahren unterstützt werden können.

---

## 58. Delay Calculation

Beamforming-Verzögerungen MÜSSEN aus definierter Geometrie und Schallgeschwindigkeit berechnet werden können.

---

## 59. Fractional Delay

Nichtganzzahlige Sample-Verzögerungen SOLLEN über geeignete Interpolation unterstützt werden.

---

## 60. Apodization

Apodization SOLL unterstützt werden können.

---

## 61. Apodization Window

Mindestens folgende Fenster SOLLEN integrierbar sein:

```text
Rectangular
Hann
Hamming
Blackman
Tukey
Custom
```

---

## 62. Dynamic Aperture

Dynamische Aperturwahl SOLL unterstützt werden können.

---

## 63. F-Number

F-Number-basierte Apertursteuerung SOLL unterstützt werden können.

---

## 64. Receive Beamforming

Receive Beamforming SOLL unabhängig von der Hardware-Sendeansteuerung implementierbar sein.

---

## 65. Plane-Wave Imaging

Plane-Wave-Imaging SOLL architektonisch unterstützt werden können.

---

## 66. Plane-Wave Compounding

Mehrere Plane-Wave-Winkel SOLLEN kohärent oder inkohärent kombiniert werden können.

---

## 67. Synthetic Aperture

Synthetic-Aperture-Verfahren SOLLEN integrierbar sein.

---

## 68. Adaptive Beamforming

Adaptive Beamforming-Verfahren SOLLEN als Erweiterung integrierbar sein.

---

## 69. Beamforming Provenance

Beamforming-Ergebnisse MÜSSEN die relevanten Parameter speichern können.

---

## 70. RF Preprocessing

RF-Daten SOLLEN vor weiterer Verarbeitung gefiltert werden können.

---

## 71. DC Removal

DC-Anteile SOLLEN entfernt werden können.

---

## 72. Bandpass Filtering

Bandpassfilterung SOLL unterstützt werden können.

---

## 73. FIR Filtering

FIR-Filter SOLLEN unterstützt werden können.

---

## 74. IIR Filtering

IIR-Filter SOLLEN unterstützt werden können.

---

## 75. Filter State

Streamingfähige Filter MÜSSEN ihren Zustand zwischen Datenblöcken erhalten können.

---

## 76. Demodulation

RF-Daten SOLLEN in komplexe Basisbanddaten demoduliert werden können.

---

## 77. IQ Representation

IQ-Daten SOLLEN als komplexe Werte repräsentiert werden.

---

## 78. Complex Precision

Mindestens folgende komplexe Präzision SOLL unterstützt werden können:

```text
ComplexFloat32
ComplexFloat64
```

---

## 79. Envelope Detection

Ultrasound Compute MUSS Hüllkurvendetektion unterstützen können.

---

## 80. Hilbert Transform

Hilbert-Transform-basierte Envelope Detection SOLL unterstützt werden können.

---

## 81. IQ Envelope

Für IQ-Daten kann die Hüllkurve berechnet werden als:

```text
A = sqrt(I^2 + Q^2)
```

---

## 82. Envelope Precision

Die Berechnungspräzision MUSS definierbar sein.

---

## 83. Log Compression

Ultrasound Compute SOLL logarithmische Kompression unterstützen.

---

## 84. Dynamic Range

Der dargestellte Dynamikbereich SOLL konfigurierbar sein.

---

## 85. Display Separation

Log Compression DARF quantitative RF- oder IQ-Quelldaten nicht überschreiben.

---

## 86. Time Gain Compensation

Time Gain Compensation wird abgekürzt:

`TGC`

---

## 87. TGC Curve

Eine TGC-Kurve SOLL als tiefenabhängige Verstärkungsfunktion repräsentiert werden.

---

## 88. TGC Application

TGC SOLL reproduzierbar auf Daten angewendet werden können.

---

## 89. TGC Provenance

Die verwendete TGC-Kurve SOLL gespeichert werden können.

---

## 90. Gain

Globale Verstärkung SOLL als expliziter Parameter repräsentierbar sein.

---

## 91. B-Mode

Ultrasound Compute MUSS B-Mode-Daten verarbeiten können.

---

## 92. B-Mode Pipeline

Eine typische B-Mode-Pipeline lautet:

```text
RF / IQ
   |
   v
Beamforming
   |
   v
Envelope Detection
   |
   v
Gain / TGC
   |
   v
Log Compression
   |
   v
Scan Conversion
   |
   v
B-Mode Image
```

---

## 93. B-Mode Image

Das Ergebnis wird als:

`UltrasoundBModeImage`

repräsentiert.

---

## 94. B-Mode Quantitative Separation

Display-Pixel eines B-Mode-Bildes DÜRFEN nicht automatisch als quantitative Gewebewerte interpretiert werden.

---

## 95. Scan Conversion

Ultrasound Compute MUSS Scan Conversion unterstützen können.

---

## 96. Scan Geometry

Mindestens folgende Eingabegeometrien SOLLEN unterstützt werden können:

```text
Linear
Sector
Curvilinear
Custom
```

---

## 97. Cartesian Output

Scan Conversion SOLL Daten in ein kartesisches Raster transformieren können.

---

## 98. Output Spacing

Die physikalische Pixelauflösung des Ausgaberasters MUSS definierbar sein.

---

## 99. Scan Conversion Interpolation

Mindestens:

```text
NearestNeighbor
Linear
```

SOLLTEN unterstützt werden.

---

## 100. Higher-Order Scan Conversion

Höherwertige Interpolationsverfahren DÜRFEN unterstützt werden.

---

## 101. Invalid Region

Bereiche außerhalb der akquirierten Ultraschallgeometrie MÜSSEN eindeutig behandelbar sein.

---

## 102. Scan Conversion Mask

Eine Validitätsmaske SOLL erzeugt werden können.

---

## 103. M-Mode

Ultrasound Compute SOLL M-Mode unterstützen.

---

## 104. M-Mode Data

M-Mode wird als:

`UltrasoundMMode`

repräsentiert.

---

## 105. M-Mode Axes

Ein M-Mode-Datensatz SOLL mindestens folgende Achsen besitzen:

```text
Depth
Time
```

---

## 106. M-Mode Sampling

Die zeitliche Samplingrate MUSS bekannt sein.

---

## 107. M-Mode Measurement

Zeit-, Distanz- und Bewegungsmessungen SOLLEN unterstützt werden können.

---

## 108. Doppler

Ultrasound Compute SOLL Doppler-Verarbeitung unterstützen.

---

## 109. Doppler Shift

Die Dopplerfrequenz wird als:

```text
f_d
```

repräsentiert.

---

## 110. Doppler Velocity

Für geeignete Annahmen kann Geschwindigkeit aus:

```text
v = (f_d * c) / (2 * f_0 * cos(theta))
```

abgeleitet werden.

Dabei bezeichnet:

```text
v      Geschwindigkeit
f_d    Dopplerfrequenzverschiebung
c      angenommene Schallgeschwindigkeit
f_0    Sendefrequenz
theta  Winkel zwischen Strahl und Bewegungsrichtung
```

---

## 111. Doppler Angle

Der verwendete Dopplerwinkel MUSS explizit gespeichert werden.

---

## 112. Angle Validity

Eine Geschwindigkeitsberechnung DARF nicht ohne Behandlung eines mathematisch ungeeigneten Winkels durchgeführt werden.

---

## 113. Angle Correction

Angle Correction SOLL unterstützt werden.

---

## 114. Uncorrected Doppler

Dopplerwerte ohne Winkelkorrektur MÜSSEN als solche identifizierbar sein.

---

## 115. Aliasing

Doppler-Verarbeitung MUSS Aliasing erkennen beziehungsweise repräsentieren können.

---

## 116. Nyquist Limit

Das Nyquist-Limit SOLL aus der effektiven Sampling- beziehungsweise PRF-Konfiguration ableitbar sein.

---

## 117. Pulse Repetition Frequency

Pulse Repetition Frequency wird abgekürzt:

`PRF`

---

## 118. PRF Metadata

PRF MUSS bei entsprechenden Dopplerdaten repräsentierbar sein.

---

## 119. Pulsed-Wave Doppler

Pulsed-Wave Doppler wird abgekürzt:

`PW Doppler`

und SOLL unterstützt werden.

---

## 120. Sample Volume

PW Doppler SOLL ein definiertes Sample Volume besitzen.

---

## 121. Sample Volume Position

Die Position des Sample Volume MUSS räumlich repräsentierbar sein.

---

## 122. Sample Volume Size

Die Größe des Sample Volume SOLL repräsentierbar sein.

---

## 123. Continuous-Wave Doppler

Continuous-Wave Doppler wird abgekürzt:

`CW Doppler`

und SOLL architektonisch unterstützt werden können.

---

## 124. Spectral Doppler

Ultrasound Compute SOLL Spectral Doppler unterstützen.

---

## 125. Spectral Doppler Object

Ein Spektraldatensatz wird als:

`UltrasoundSpectralDoppler`

repräsentiert.

---

## 126. Spectral Analysis

Spektralanalyse SOLL über FFT-basierte Verfahren durchgeführt werden können.

---

## 127. Windowed FFT

Zeitfensterbasierte FFT-Verarbeitung SOLL unterstützt werden.

---

## 128. FFT Window

Mindestens folgende Fenster SOLLEN verfügbar sein können:

```text
Hann
Hamming
Blackman
Rectangular
Custom
```

---

## 129. FFT Size

FFT-Größe MUSS konfigurierbar sein.

---

## 130. FFT Overlap

Überlappung zwischen Spektralfenstern SOLL konfigurierbar sein.

---

## 131. Spectrogram

Das Ergebnis SOLL als Zeit-Frequenz-Darstellung repräsentiert werden können.

---

## 132. Baseline

Die Doppler-Baseline SOLL als Darstellungsparameter verwaltet werden können.

---

## 133. Wall Filter

Ein Wall Filter SOLL unterstützt werden.

---

## 134. Wall Filter Purpose

Der Wall Filter SOLL niederfrequente Signalanteile kontrolliert unterdrücken können.

---

## 135. Wall Filter Provenance

Filterparameter MÜSSEN für quantitative Auswertungen nachvollziehbar sein.

---

## 136. Color Doppler

Ultrasound Compute SOLL Color Doppler unterstützen.

---

## 137. Color Doppler Object

Color-Doppler-Daten werden als:

`UltrasoundColorDoppler`

repräsentiert.

---

## 138. Velocity Estimate

Für jedes gültige Doppler-Sample SOLL eine Geschwindigkeits- beziehungsweise Frequenzschätzung repräsentierbar sein.

---

## 139. Variance Estimate

Varianz- oder Turbulenzschätzungen SOLLEN repräsentierbar sein.

---

## 140. Doppler Confidence

Konfidenzwerte SOLLEN unterstützt werden können.

---

## 141. Color Doppler ROI

Color Doppler SOLL auf eine definierte räumliche Region beschränkt werden können.

---

## 142. Color Map Separation

Farbcodierung MUSS von den zugrunde liegenden numerischen Dopplerwerten getrennt sein.

---

## 143. Power Doppler

Ultrasound Compute SOLL Power Doppler unterstützen.

---

## 144. Power Doppler Object

Power-Doppler-Daten werden als:

`UltrasoundPowerDoppler`

repräsentiert.

---

## 145. Power Value

Die Doppler-Signalleistung SOLL unabhängig von einer Display-Farbskala gespeichert werden können.

---

## 146. Duplex Mode

Gleichzeitige B-Mode- und Doppler-Verarbeitung SOLL unterstützt werden können.

---

## 147. Triplex Mode

B-Mode, Color Doppler und Spectral Doppler SOLLEN gemeinsam repräsentierbar sein.

---

## 148. Synchronization Across Modes

Daten verschiedener Ultraschallmodi MÜSSEN zeitlich miteinander synchronisierbar sein.

---

## 149. Speckle

Ultrasound Compute SOLL ultraschallspezifische Speckle-Verarbeitung unterstützen können.

---

## 150. Speckle Reduction

Speckle-Reduktionsverfahren SOLLEN integrierbar sein.

---

## 151. Speckle Preservation

Filter DÜRFEN diagnostisch beziehungsweise quantitativ relevante Strukturen nicht unkontrolliert verändern.

---

## 152. Speckle Tracking

Speckle Tracking SOLL architektonisch unterstützt werden.

---

## 153. Motion Tracking

Ultrasound Compute SOLL Bewegungen zwischen Frames analysieren können.

---

## 154. Motion Vector

Bewegung kann als:

`UltrasoundMotionVector`

repräsentiert werden.

---

## 155. Motion Field

Dichte Bewegungsfelder SOLLEN unterstützt werden können.

---

## 156. Block Matching

Block-Matching-Verfahren SOLLEN unterstützt werden können.

---

## 157. Optical Flow

Optical-Flow-artige Verfahren SOLLEN integrierbar sein.

---

## 158. Correlation Tracking

Korrelationsbasierte Trackingverfahren SOLLEN unterstützt werden können.

---

## 159. Tracking Confidence

Bewegungsschätzungen SOLLEN Konfidenzinformationen besitzen können.

---

## 160. Strain

Lokale Deformation SOLL aus geeigneten Bewegungsdaten ableitbar sein.

---

## 161. Strain Rate

Strain Rate SOLL für geeignete zeitaufgelöste Daten berechnet werden können.

---

## 162. Elastography

Ultrasound Compute SOLL Elastographie architektonisch unterstützen.

---

## 163. Elastography Object

Elastographiedaten werden als:

`UltrasoundElastography`

repräsentiert.

---

## 164. Strain Elastography

Strain Elastography SOLL unterstützt werden können.

---

## 165. Shear Wave Elastography

Shear-Wave-Elastography SOLL architektonisch unterstützt werden können.

---

## 166. Elasticity Map

Elastographieergebnisse SOLLEN als quantitative beziehungsweise relative Karten repräsentiert werden können.

---

## 167. Elastography Units

Quantitative Elastographieparameter MÜSSEN ihre Einheit deklarieren.

---

## 168. Relative Elastography

Relative und absolut quantitative Elastographieergebnisse MÜSSEN unterscheidbar sein.

---

## 169. 3D Ultrasound

Ultrasound Compute SOLL 3D-Ultraschall unterstützen.

---

## 170. 3D Acquisition

3D-Daten DÜRFEN aus:

```text
MatrixArray
MechanicalSweep
Tracked2D
Freehand3D
```

erzeugt werden.

---

## 171. Volume Reconstruction

Einzelne Ultraschallframes SOLLEN zu einem Volumen rekonstruiert werden können.

---

## 172. Frame Pose

Für freie 3D-Rekonstruktion MUSS die räumliche Pose jedes Frames bekannt oder schätzbar sein.

---

## 173. Voxel Compounding

Mehrere Samples SOLLEN in gemeinsame Volumenvoxel kombiniert werden können.

---

## 174. Compounding Policy

Mindestens folgende Strategien SOLLEN integrierbar sein:

```text
Nearest
Average
WeightedAverage
Maximum
ConfidenceWeighted
```

---

## 175. Reconstruction Mask

Die Gültigkeit rekonstruierter Volumenbereiche SOLL als Maske repräsentiert werden.

---

## 176. Reconstruction Confidence

Rekonstruierte Voxel SOLLEN Konfidenzwerte besitzen können.

---

## 177. 4D Ultrasound

Zeitaufgelöste 3D-Ultraschalldaten SOLLEN unterstützt werden können.

---

## 178. Volume Timestamp

Jedes 3D-Volumen einer 4D-Sequenz MUSS zeitlich identifizierbar sein.

---

## 179. Temporal Interpolation

Zeitliche Interpolation SOLL optional unterstützt werden.

---

## 180. Cine Processing

Ultrasound Compute MUSS Cine-Sequenzen effizient verarbeiten können.

---

## 181. Random Frame Access

Einzelne Frames SOLLEN direkt adressierbar sein.

---

## 182. Sequential Processing

Streaming-Algorithmen SOLLEN Frames sequenziell verarbeiten können.

---

## 183. Ring Buffer

Echtzeit-Cine-Verarbeitung SOLL Ringbuffer verwenden können.

---

## 184. Frame Drop Detection

Verlorene Frames MÜSSEN erkennbar sein.

---

## 185. Frame Number

Frames SOLLEN eine monotone Sequenznummer besitzen können.

---

## 186. Timestamp Discontinuity

Zeitliche Diskontinuitäten SOLLEN diagnostiziert werden.

---

## 187. Real-Time Processing

Ultrasound Compute SOLL Echtzeitverarbeitung unterstützen.

---

## 188. Real-Time Deadline

Echtzeitpipelines SOLLEN eine Ziel-Latenz beziehungsweise Deadline deklarieren können.

---

## 189. Latency Budget

Eine Pipeline SOLL ihr Latenzbudget auf Verarbeitungsschritte verteilen können.

---

## 190. Backpressure

Bei Überlastung MUSS eine definierte Backpressure-Policy existieren.

---

## 191. Frame Drop Policy

Bei Echtzeitüberlastung MUSS definiert sein, ob Frames:

```text
BLOCK
DROP_OLDEST
DROP_NEWEST
DEGRADE_QUALITY
```

behandelt werden.

---

## 192. Offline Mode

Offline-Verarbeitung DARF vollständige Daten erhalten und auf maximale Reproduzierbarkeit optimieren.

---

## 193. Real-Time vs Offline

Echtzeit- und Offline-Modus MÜSSEN getrennte Qualitäts- und Scheduling-Policies besitzen können.

---

## 194. Quality Degradation

Eine Echtzeitpipeline DARF bei Überlastung kontrolliert auf weniger aufwendige Algorithmen wechseln.

---

## 195. Degradation Visibility

Eine reduzierte Verarbeitungsqualität MUSS als solche erkennbar sein.

---

## 196. Measurement

Ultrasound Compute SOLL ultraschallspezifische Messungen unterstützen.

---

## 197. Distance Measurement

Räumliche Distanzmessungen SOLLEN unterstützt werden.

---

## 198. Area Measurement

Flächenmessungen SOLLEN unterstützt werden.

---

## 199. Volume Measurement

Volumenmessungen SOLLEN unterstützt werden.

---

## 200. Time Measurement

Zeitintervalle SOLLEN messbar sein.

---

## 201. Velocity Measurement

Geschwindigkeiten SOLLEN aus geeigneten Dopplerdaten bestimmt werden können.

---

## 202. Measurement Object

Eine Ultraschallmessung wird als:

`UltrasoundMeasurement`

repräsentiert.

---

## 203. Measurement Fields

Eine Messung SOLL mindestens enthalten können:

```text
MeasurementId
Type
Value
Unit
Geometry
Source
Algorithm
Timestamp
Confidence
```

---

## 204. Measurement Units

Messungen MÜSSEN Einheiten deklarieren.

---

## 205. Measurement Geometry

Geometrische Messungen MÜSSEN auf validierter räumlicher Geometrie basieren.

---

## 206. Doppler Measurement Provenance

Dopplerbasierte Geschwindigkeitswerte SOLLEN mindestens referenzieren können:

```text
TransmitFrequency
PRF
DopplerAngle
SpeedOfSound
Estimator
```

---

## 207. Quantitative Separation

Quantitative Messungen MÜSSEN von Displaykoordinaten und Displaywerten getrennt sein.

---

## 208. Segmentation

Ultrasound Compute SOLL die Segmentierungsarchitektur von Radiology Compute verwenden können.

---

## 209. Ultrasound-Specific Segmentation

Ultraschallspezifische Segmentierungsalgorithmen SOLLEN integrierbar sein.

---

## 210. Temporal Segmentation

Segmentierungen SOLLEN über Cine-Sequenzen verfolgt werden können.

---

## 211. Contour Tracking

Konturen SOLLEN zwischen Frames verfolgt werden können.

---

## 212. AI Integration

Ultrasound Compute SOLL KI-Modelle integrieren können.

---

## 213. AI Tasks

Mögliche Aufgaben umfassen:

```text
Segmentation
Detection
Classification
ViewRecognition
LandmarkDetection
Denoising
Beamforming
Reconstruction
MotionTracking
MeasurementAssistance
```

---

## 214. AI Input

KI-Modelle DÜRFEN abhängig vom Modell folgende Eingaben verwenden:

```text
RF
IQ
Beamformed Data
B-Mode
Doppler
Volume
Cine
```

---

## 215. AI Model Identity

KI-Ergebnisse SOLLEN referenzieren können:

```text
ModelId
ModelVersion
Configuration
InputReference
```

---

## 216. AI Provenance

Vor- und Nachverarbeitungsschritte MÜSSEN dokumentierbar sein.

---

## 217. AI Result Separation

KI-Ergebnisse MÜSSEN als abgeleitete Ergebnisse gekennzeichnet werden.

---

## 218. AI Confidence

Konfidenzwerte SOLLEN erhalten bleiben.

---

## 219. CPU Backend

Ultrasound Compute MUSS CPU-Ausführung unterstützen.

---

## 220. SIMD

Signalverarbeitung SOLL SIMD-Vektorisierung unterstützen können.

---

## 221. Multi-Core

Unabhängige Kanäle, Beams, Scanlines, Frames oder Verarbeitungsschritte SOLLEN parallelisiert werden können.

---

## 222. GPU Backend

Ultrasound Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0005` integrieren.

---

## 223. GPU Suitable Operations

Geeignete GPU-Operationen umfassen:

```text
Beamforming
Filtering
Demodulation
Envelope Detection
Scan Conversion
Doppler Estimation
FFT
Speckle Processing
Motion Tracking
3D Reconstruction
AI Processing
```

---

## 224. GPU Streaming

Daten SOLLEN asynchron zwischen Acquisition-, Host- und GPU-Pipelines übertragen werden können.

---

## 225. GPU Double Buffering

Double- oder Multi-Buffering SOLL unterstützt werden können.

---

## 226. GPU Memory Budget

Echtzeitverarbeitung MUSS verfügbare GPU-Speicherbudgets berücksichtigen.

---

## 227. NPU Backend

Ultrasound Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0006` für geeignete KI-Verarbeitung integrieren.

---

## 228. NPU Optionality

Grundlegende Ultraschall-Signalverarbeitung DARF keine NPU voraussetzen.

---

## 229. Distributed Compute

Ultrasound Compute SOLL `NPSPEC-COMPUTE-RUNTIME-0007` für geeignete Offline-Workloads integrieren können.

---

## 230. Distributed Real-Time Restriction

Latenzkritische Echtzeitpipelines SOLLEN nicht unnötig über nicht deterministische Netzwerkpfade verteilt werden.

---

## 231. HPC

Große Offline-Datensätze SOLLEN über `NPSPEC-COMPUTE-HPC-0001` verarbeitet werden können.

---

## 232. Scheduler Integration

Ultrasound Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0002` integrieren.

---

## 233. Task Model

Eine Pipeline SOLL in Compute Tasks zerlegt werden.

Beispiel:

```text
Acquire
   |
   v
Beamform
   |
   v
Demodulate
   |
   v
Envelope
   |
   v
Scan Convert
   |
   v
Analyze
   |
   v
Display / Store
```

---

## 234. Task Dependencies

Abhängigkeiten zwischen Tasks MÜSSEN explizit darstellbar sein.

---

## 235. Real-Time Priority

Echtzeitkritische Tasks SOLLEN höhere Scheduler-Priorität erhalten können.

---

## 236. Deadline-Aware Scheduling

Der Scheduler SOLL Ultraschalltasks mit Deadlines unterstützen können.

---

## 237. Background Processing

Nicht zeitkritische Analyse SOLL gegenüber der Echtzeitpipeline niedriger priorisiert werden.

---

## 238. Memory Model

Ultrasound Compute MUSS `NPSPEC-COMPUTE-RUNTIME-0003` integrieren.

---

## 239. Memory Objects

Mindestens folgende Speicherobjekte SOLLEN unterstützt werden:

```text
ChannelBuffer
RFBuffer
IQBuffer
BeamformedBuffer
EnvelopeBuffer
FrameBuffer
DopplerBuffer
SpectrumBuffer
VolumeBuffer
MotionFieldBuffer
TemporaryBuffer
```

---

## 240. Buffer Reuse

Echtzeitpipelines SOLLEN Speicherpuffer wiederverwenden.

---

## 241. Allocation Control

Häufige dynamische Speicherallokationen im zeitkritischen Pfad SOLLEN vermieden werden.

---

## 242. Pinned Memory

Pinned beziehungsweise transferoptimierter Host-Speicher SOLL bei geeigneten GPU-Pipelines unterstützt werden können.

---

## 243. Ring Buffer

Zeitabhängige Eingangsdaten SOLLEN in Ringbuffern verwaltet werden können.

---

## 244. Buffer Overflow

Buffer Overflow MUSS erkannt werden.

---

## 245. Buffer Underflow

Buffer Underflow SOLL erkannt werden.

---

## 246. Data Types

Mindestens folgende Datentypen SOLLEN unterstützt werden können:

```text
Int16
Int32
Float16
Float32
Float64
ComplexFloat32
ComplexFloat64
```

soweit für die jeweilige Operation geeignet.

---

## 247. ADC Representation

Rohe ADC-Daten SOLLEN in ihrer ursprünglichen Integerpräzision erhalten werden können.

---

## 248. Processing Precision

Die Verarbeitungspräzision MUSS unabhängig vom Eingangsdatenformat definierbar sein.

---

## 249. Numerical Stability

Signalverarbeitungsalgorithmen MÜSSEN auf numerische Stabilität ausgelegt sein.

---

## 250. NaN Handling

NaN-Werte MÜSSEN kontrolliert behandelt werden.

---

## 251. Infinity Handling

Infinity-Werte MÜSSEN kontrolliert behandelt werden.

---

## 252. Saturation

Signal-Sättigung SOLL erkannt werden können.

---

## 253. Clipping

Clipping in Eingangsdaten SOLL diagnostizierbar sein.

---

## 254. Normalization

Normalisierung MUSS explizit und reproduzierbar erfolgen.

---

## 255. DICOM Integration

Ultrasound Compute MUSS `NPSPEC-COMPUTE-DICOM-0001` integrieren können.

---

## 256. DICOM Ultrasound Input

DICOM-Ultraschallbilder und Multiframe-Objekte SOLLEN als Eingabe verwendet werden können.

---

## 257. DICOM Metadata

Ultraschallspezifische DICOM-Metadaten SOLLEN erhalten bleiben.

---

## 258. DICOM Output

Geeignete abgeleitete Ergebnisse SOLLEN in standardisierte DICOM-Repräsentationen überführt werden können, sofern eine passende Repräsentation existiert.

---

## 259. PACS Integration

Ultrasound Compute SOLL `NPSPEC-COMPUTE-PACS-0001` integrieren können.

---

## 260. PACS Input

Ultraschallstudien SOLLEN aus PACS abgerufen werden können.

---

## 261. PACS Derived Output

Abgeleitete DICOM-Ergebnisse SOLLEN kontrolliert an PACS zurückgegeben werden können.

---

## 262. Radiology Integration

Gemeinsame Operationen wie:

```text
Resampling
Registration
Segmentation
Measurement
Volume Processing
```

SOLLTEN über `NPSPEC-COMPUTE-RADIOLOGY-0001` wiederverwendet werden.

---

## 263. Duplication Avoidance

Ultrasound Compute SOLL generische Radiology-Compute-Funktionen nicht unnötig duplizieren.

---

## 264. Registration

Ultraschallframes und Volumen SOLLEN registriert werden können.

---

## 265. Temporal Registration

Aufeinanderfolgende Frames SOLLEN zeitlich registriert werden können.

---

## 266. Multi-Modality Registration

Ultraschalldaten SOLLEN mit anderen Modalitäten registrierbar sein.

Beispiele:

```text
Ultrasound <-> CT
Ultrasound <-> MR
Ultrasound <-> PET
```

---

## 267. Registration Confidence

Registrierungsergebnisse SOLLEN Qualitäts- beziehungsweise Konfidenzmetriken besitzen.

---

## 268. Provenance

Ultrasound Compute MUSS Provenance unterstützen.

---

## 269. Provenance Record

Ein Provenance-Eintrag SOLL mindestens enthalten können:

```text
SourceId
OperationId
AlgorithmId
AlgorithmVersion
Parameters
RuntimeVersion
Backend
Precision
Timestamp
```

---

## 270. Ultrasound-Specific Provenance

Je nach Operation SOLLEN zusätzlich gespeichert werden:

```text
ProbeGeometry
SamplingFrequency
CenterFrequency
SpeedOfSound
BeamformingParameters
Gain
TGC
PRF
DopplerAngle
FilterConfiguration
```

---

## 271. Processing Graph

Die Herkunft eines Ergebnisses SOLL als Graph darstellbar sein.

Beispiel:

```text
RF Data
   |
   v
Beamforming
   |
   v
IQ Demodulation
   |
   +----------------+
   |                |
   v                v
B-Mode          Doppler
   |                |
   v                v
Image          Velocity Map
```

---

## 272. Reproducibility

Offline-Pipelines SOLLEN bei identischen Eingaben und Parametern reproduzierbar sein.

---

## 273. Deterministic Mode

Ultrasound Compute SOLL einen deterministischen Modus unterstützen.

---

## 274. Backend Differences

Numerische Unterschiede zwischen CPU-, GPU- und anderen Backends MÜSSEN über definierte Toleranzen berücksichtigt werden.

---

## 275. Diagnostics

Ultrasound Compute MUSS strukturierte Diagnosen bereitstellen.

---

## 276. Diagnostic Classes

Mindestens folgende Klassen SOLLEN existieren:

```text
INVALID_ULTRASOUND_DATA
INVALID_CHANNEL_DATA
INVALID_RF_DATA
INVALID_IQ_DATA
INVALID_PROBE_GEOMETRY
INVALID_BEAM_GEOMETRY
INVALID_SAMPLE_RATE
INVALID_FREQUENCY
INVALID_SPEED_OF_SOUND
INVALID_PRF
INVALID_DOPPLER_ANGLE
INVALID_SCAN_GEOMETRY
BEAMFORMING_FAILED
DEMODULATION_FAILED
ENVELOPE_FAILED
SCAN_CONVERSION_FAILED
DOPPLER_PROCESSING_FAILED
FFT_FAILED
TRACKING_FAILED
RECONSTRUCTION_FAILED
FRAME_DROPPED
TIMESTAMP_DISCONTINUITY
BUFFER_OVERFLOW
BUFFER_UNDERFLOW
DEADLINE_MISSED
SIGNAL_CLIPPED
SIGNAL_SATURATED
NAN_DETECTED
INFINITY_DETECTED
OUT_OF_MEMORY
DEVICE_FAILURE
CANCELLED
```

---

## 277. Diagnostic Severity

Mindestens:

```text
INFO
WARNING
ERROR
FATAL
```

SOLLTEN unterstützt werden.

---

## 278. Deadline Diagnostic

Verpasste Echtzeit-Deadlines SOLLEN explizit diagnostiziert werden.

---

## 279. Data Quality Diagnostic

Probleme der Eingangssignalqualität SOLLEN getrennt von Compute-Fehlern behandelt werden.

---

## 280. Privacy-Aware Diagnostics

Diagnosen SOLLEN keine unnötigen patientenbezogenen Informationen enthalten.

---

## 281. Profiling

Ultrasound Compute MUSS Profiling unterstützen.

---

## 282. Profiling Categories

Mindestens:

```text
Input
Beamforming
Filtering
Demodulation
Envelope
Compression
ScanConversion
Doppler
FFT
Tracking
Reconstruction
AI
Output
```

SOLLTEN messbar sein.

---

## 283. Frame Rate

Verarbeitete Frames pro Sekunde SOLLEN messbar sein.

---

## 284. Volume Rate

Verarbeitete Volumen pro Sekunde SOLLEN messbar sein.

---

## 285. End-to-End Latency

Die Zeit zwischen Eingang eines Datenframes und Verfügbarkeit des Ergebnisses SOLL messbar sein.

---

## 286. Stage Latency

Latenz einzelner Pipeline-Stufen SOLL messbar sein.

---

## 287. Deadline Miss Rate

Die Rate verpasster Deadlines SOLL messbar sein.

---

## 288. Buffer Occupancy

Belegung zeitkritischer Buffer SOLL beobachtbar sein.

---

## 289. GPU Profiling

GPU-Kernelzeit und Transferzeit SOLLEN getrennt messbar sein.

---

## 290. Memory Profiling

Peak Memory und Bufferbedarf SOLLEN messbar sein.

---

## 291. Testing — Signal

Mindestens folgende Fälle MÜSSEN getestet werden:

```text
KnownSineSignal
KnownRFSignal
KnownIQSignal
ZeroSignal
ImpulseSignal
NoiseSignal
ClippedSignal
```

---

## 292. Testing — Beamforming

Mindestens:

```text
SinglePointTarget
MultiplePointTargets
KnownGeometry
KnownDelay
FocusDepth
Apodization
```

SOLLTEN getestet werden.

---

## 293. Testing — Envelope

Envelope Detection SOLL gegen analytisch bekannte Signale getestet werden.

---

## 294. Testing — Scan Conversion

Mindestens:

```text
LinearGeometry
SectorGeometry
CurvilinearGeometry
BoundaryPixels
InvalidRegion
```

MÜSSEN getestet werden.

---

## 295. Testing — B-Mode

B-Mode-Pipelines SOLLEN mit definierten Phantomen getestet werden.

---

## 296. Testing — Doppler

Mindestens:

```text
ZeroVelocity
KnownPositiveVelocity
KnownNegativeVelocity
KnownAngle
AngleCorrection
Aliasing
NyquistBoundary
```

MÜSSEN getestet werden.

---

## 297. Testing — Spectral Doppler

Mindestens:

```text
SingleFrequency
MultipleFrequencies
KnownFrequencyShift
WindowFunction
FFTSize
Overlap
```

SOLLTEN getestet werden.

---

## 298. Testing — Color Doppler

Geschwindigkeits- und Varianzschätzungen SOLLEN gegen definierte Referenzdaten getestet werden.

---

## 299. Testing — Motion

Mindestens:

```text
ZeroMotion
KnownTranslation
KnownDeformation
FrameDiscontinuity
```

SOLLTEN getestet werden.

---

## 300. Testing — 3D Reconstruction

Mindestens:

```text
KnownFramePositions
KnownVolumeGeometry
SparseFrames
OverlappingFrames
InvalidTrackingPose
```

SOLLTEN getestet werden.

---

## 301. Testing — Real-Time

Mindestens:

```text
StableFrameRate
TemporaryOverload
SustainedOverload
FrameDrop
DeadlineMiss
BufferOverflow
```

MÜSSEN getestet werden.

---

## 302. Testing — CPU

Alle Mindestfunktionen MÜSSEN auf CPU testbar sein.

---

## 303. Testing — GPU

GPU-Ergebnisse SOLLEN innerhalb definierter Toleranzen mit CPU-Referenzen verglichen werden.

---

## 304. Testing — Determinism

Deterministische Offline-Pipelines MÜSSEN reproduzierbar sein.

---

## 305. Testing — Cancellation

Abgebrochene Verarbeitung DARF unvollständige Ergebnisse nicht als vollständig veröffentlichen.

---

## 306. Phantom Testing

Ultraschall-Compute SOLL mit geeigneten digitalen und physikalischen Phantomen validierbar sein.

---

## 307. Point Target Phantom

Punktziele SOLLEN für Beamforming- und Auflösungstests verwendet werden können.

---

## 308. Flow Phantom

Definierte Flussdaten SOLLEN für Dopplertests verwendet werden können.

---

## 309. Motion Phantom

Definierte Bewegungsmuster SOLLEN für Tracking-Tests verwendet werden können.

---

## 310. Stress Tests

Mindestens folgende Szenarien SOLLTEN getestet werden:

```text
HighChannelCount
HighSampleRate
HighFrameRate
LongCine
Large3DVolume
ContinuousStreaming
GPUFallback
MemoryPressure
RepeatedModeSwitching
ConcurrentBModeAndDoppler
CancellationUnderLoad
```

---

## 311. Minimalimplementierung

Eine minimale konforme Implementierung MUSS mindestens unterstützen:

```text
UltrasoundFrame
UltrasoundCine
UltrasoundImage

B-Mode

RF Input Architecture
IQ Input Architecture

Sampling Metadata
Probe Metadata
Spatial Geometry
Temporal Metadata

RF Filtering
Envelope Detection
Gain
TGC
Log Compression

Scan Conversion

Linear Geometry
Sector Geometry

M-Mode Architecture

Doppler Architecture
Doppler Angle
PRF
Velocity Representation

Spectral Doppler Architecture
FFT Integration

CPU Execution
Multi-Core Architecture
SIMD Architecture

Real-Time Streaming
Ring Buffer
Frame Drop Detection
Cancellation

DICOM Integration
Radiology Compute Integration

Memory Model
Scheduler
Object Model

Diagnostics
Profiling
Provenance
```

---

## 312. Erweiterte Implementierung

Eine vollständige Implementierung SOLL zusätzlich unterstützen:

```text
Raw Channel Data
Delay-and-Sum Beamforming
Dynamic Receive Focusing
Apodization
Dynamic Aperture

Plane-Wave Imaging
Plane-Wave Compounding
Synthetic Aperture
Adaptive Beamforming

PW Doppler
CW Doppler
Color Doppler
Power Doppler
Duplex
Triplex

Speckle Reduction
Speckle Tracking
Motion Fields

Strain
Strain Rate
Elastography
Shear-Wave Elastography

3D Ultrasound
4D Ultrasound
Freehand Reconstruction
Tracked Ultrasound
Voxel Compounding

GPU Acceleration
NPU AI Processing
Distributed Offline Processing
HPC

AI Segmentation
AI View Recognition
AI Landmark Detection
AI Reconstruction
AI Beamforming

PACS Integration
```

---

## 313. Normative Mindestanforderungen

Eine konforme Implementierung von `NPSPEC-COMPUTE-ULTRASOUND-0001` MUSS:

1. Ultraschallbilder repräsentieren können.
2. Ultraschallframes repräsentieren können.
3. Cine-Sequenzen repräsentieren können.
4. Aufnahme- und Verarbeitungszeitpunkte unterscheiden.
5. zeitliche Frame-Reihenfolge erhalten.
6. räumliche Ultraschallgeometrie repräsentieren.
7. Sondeninformationen repräsentieren können.
8. Sondentypen unterscheiden können.
9. Samplingfrequenzen repräsentieren können.
10. Mittenfrequenzen repräsentieren können.
11. Schallgeschwindigkeit als Verarbeitungsparameter behandeln.
12. die verwendete Schallgeschwindigkeit nachvollziehbar machen.
13. RF-Daten architektonisch unterstützen.
14. IQ-Daten architektonisch unterstützen.
15. komplexe IQ-Werte repräsentieren können.
16. Signalfilterung unterstützen.
17. Hüllkurvendetektion unterstützen.
18. Gain-Verarbeitung unterstützen.
19. TGC unterstützen.
20. TGC-Parameter speichern können.
21. logarithmische Kompression unterstützen.
22. quantitative Quelldaten von Displaydaten trennen.
23. B-Mode unterstützen.
24. Scan Conversion unterstützen.
25. lineare Scan-Geometrie unterstützen.
26. sektorielle Scan-Geometrie unterstützen.
27. Scan-Conversion-Ausgabe mit physikalischer Geometrie versehen.
28. ungültige Scanbereiche kennzeichnen können.
29. M-Mode architektonisch unterstützen.
30. Dopplerverarbeitung architektonisch unterstützen.
31. Dopplerwinkel repräsentieren.
32. PRF repräsentieren.
33. Dopplergeschwindigkeit mit Einheit repräsentieren.
34. nicht winkelkorrigierte Werte kennzeichnen können.
35. Aliasing architektonisch erkennen können.
36. Spectral Doppler architektonisch unterstützen.
37. FFT-basierte Spektralanalyse integrieren können.
38. Window Functions unterstützen können.
39. FFT-Größe konfigurieren können.
40. CPU-Ausführung unterstützen.
41. Multi-Core-Ausführung architektonisch unterstützen.
42. SIMD-Ausführung architektonisch unterstützen.
43. GPU-Ausführung architektonisch unterstützen.
44. NPU-Ausführung architektonisch unterstützen.
45. Echtzeit-Streaming unterstützen.
46. Ringbuffer unterstützen können.
47. Frame Drops erkennen.
48. Zeitdiskontinuitäten erkennen.
49. Backpressure definieren können.
50. Echtzeit- und Offline-Verarbeitung unterscheiden.
51. Deadline-basierte Verarbeitung architektonisch unterstützen.
52. Verarbeitungsqualität bei Degradation kennzeichnen.
53. geometrische Messungen unterstützen können.
54. Messwerte mit Einheiten versehen.
55. Dopplermessungen mit relevanten Parametern verknüpfen.
56. DICOM Compute integrieren.
57. Radiology Compute integrieren.
58. PACS Compute integrieren können.
59. das Compute Memory Model integrieren.
60. den Compute Scheduler integrieren.
61. das Compute Object Model integrieren.
62. kontrollierten Abbruch unterstützen.
63. unvollständige Ergebnisse eindeutig kennzeichnen.
64. strukturierte Diagnosen bereitstellen.
65. Buffer Overflow erkennen.
66. Signal Clipping diagnostizieren können.
67. NaN-Werte erkennen.
68. Infinity-Werte erkennen.
69. Profiling unterstützen.
70. End-to-End-Latenz messbar machen.
71. Pipeline-Latenzen messbar machen.
72. Provenance unterstützen.
73. relevante Ultraschallparameter in Provenance speichern können.
74. Originaldaten und abgeleitete Daten unterscheiden.
75. Erweiterungen ohne Änderung des grundlegenden Ultrasound-Objektmodells ermöglichen.

---

## 314. Empfohlene interne Komponenten

Eine vollständige Implementierung SOLL logisch folgende Komponenten besitzen:

```text
NovaUltrasoundCompute
 |
 +-- UltrasoundRuntime
 |
 +-- UltrasoundDataModel
 |
 +-- ProbeGeometryEngine
 |
 +-- RFProcessingEngine
 |
 +-- IQProcessingEngine
 |
 +-- BeamformingEngine
 |
 +-- EnvelopeEngine
 |
 +-- ScanConversionEngine
 |
 +-- BModeEngine
 |
 +-- MModeEngine
 |
 +-- DopplerEngine
 |
 +-- SpectralDopplerEngine
 |
 +-- ColorDopplerEngine
 |
 +-- PowerDopplerEngine
 |
 +-- MotionTrackingEngine
 |
 +-- ElastographyEngine
 |
 +-- Ultrasound3DEngine
 |
 +-- UltrasoundMeasurementEngine
 |
 +-- UltrasoundAIIntegration
 |
 +-- UltrasoundPipelineManager
 |
 +-- UltrasoundDiagnostics
 |
 +-- UltrasoundProfiler
```

Diese Struktur ist logisch und schreibt keine konkrete Datei-, Klassen- oder Modulstruktur vor.

---

## 315. Beziehung zu `NPSPEC-COMPUTE-RADIOLOGY-0001`

Radiology Compute stellt allgemeine medizinische Bildverarbeitung bereit.

Ultrasound Compute erweitert diese um ultraschallspezifische Signalverarbeitung.

```text
Radiology Compute
       |
       +--> Geometry
       +--> Resampling
       +--> Registration
       +--> Segmentation
       +--> Measurement
               |
               v
        Ultrasound Compute
               |
               +--> RF
               +--> IQ
               +--> Beamforming
               +--> B-Mode
               +--> M-Mode
               +--> Doppler
               +--> Elastography
```

---

## 316. Beziehung zu `NPSPEC-COMPUTE-DICOM-0001`

DICOM Compute stellt standardisierte Ultraschallobjekte und deren Metadaten bereit.

Ultrasound Compute führt die eigentliche ultraschallspezifische Verarbeitung aus.

---

## 317. Beziehung zu `NPSPEC-COMPUTE-PACS-0001`

PACS Compute verwaltet Ultraschallstudien und daraus erzeugte medizinische Objekte.

```text
PACS
 |
 v
DICOM Ultrasound
 |
 v
Ultrasound Compute
 |
 v
Derived Result
 |
 v
PACS
```

---

## 318. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0001`

Die Compute Runtime stellt gemeinsame CPU-, GPU-, NPU- und andere Ausführungsressourcen bereit.

---

## 319. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0002`

Der Scheduler koordiniert zeitkritische und nicht zeitkritische Ultraschalltasks.

---

## 320. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0003`

Das Memory Model stellt effiziente Buffer für RF-, IQ-, Frame-, Doppler- und Volumendaten bereit.

---

## 321. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0004`

Ultraschallobjekte SOLLEN auf dem gemeinsamen Compute Object Model aufbauen.

---

## 322. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0005`

GPU Runtime SOLL massiv parallele Ultraschall-Signal- und Bildverarbeitung beschleunigen.

---

## 323. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0006`

NPU Runtime SOLL für geeignete KI-Verarbeitung verwendet werden können.

---

## 324. Beziehung zu `NPSPEC-COMPUTE-RUNTIME-0007`

Distributed Compute SOLL große Offline-Ultraschall-Workloads verteilen können.

---

## 325. Grundlegende Datenpipeline

```text
Raw Channel Data
       |
       v
RF Processing
       |
       v
Beamforming
       |
       v
IQ / Envelope
       |
       +-------------------+
       |                   |
       v                   v
     B-Mode             Doppler
       |                   |
       v                   v
Scan Conversion       Flow Analysis
       |                   |
       +---------+---------+
                 |
                 v
          Radiology Object
```

---

## 326. Echtzeitpipeline

```text
Acquisition
    |
    v
Ring Buffer
    |
    v
Signal Processing
    |
    v
Beamforming
    |
    v
Mode Processing
    |
    v
Scan Conversion
    |
    v
Analysis
    |
    v
Output
```

Jede Stufe SOLL eine definierte Latenz besitzen können.

---

## 327. Doppler-Pipeline

```text
IQ / RF Data
     |
     v
Wall Filtering
     |
     v
Doppler Estimation
     |
     +-------------------+
     |                   |
     v                   v
Velocity / Power     Spectral FFT
     |                   |
     v                   v
Color / Power Map    Spectrogram
```

---

## 328. 3D-Rekonstruktionspipeline

```text
2D Frames
    +
Frame Poses
    |
    v
Spatial Mapping
    |
    v
Voxel Compounding
    |
    v
Volume
    |
    v
Validity / Confidence
```

---

## 329. KI-Integrationsmodell

```text
RF / IQ / Image / Cine
          |
          v
     Preprocessing
          |
          v
        Tensor
          |
     +----+----+
     |    |    |
     v    v    v
    CPU  GPU  NPU
          |
          v
       AI Model
          |
          v
      AI Result
          |
          v
Radiology / Ultrasound Object
```

---

## 330. Echtzeitgrundsatz

Die Architektur MUSS unterscheiden zwischen:

```text
Must Process
May Process
May Drop
Background
```

Echtzeitkritische Eingangsdaten DÜRFEN nicht durch optionale Hintergrundanalyse unkontrolliert blockiert werden.

---

## 331. Datenintegritätsgrundsatz

Die Verarbeitungskette MUSS jederzeit unterscheiden können zwischen:

```text
Raw Measurement Data
Processed Signal Data
Reconstructed Image Data
Display Data
Derived Quantitative Data
AI-Derived Data
```

---

## 332. Numerischer Grundsatz

Optimierungen DÜRFEN die definierte numerische Bedeutung quantitativer Ultraschallwerte nicht stillschweigend verändern.

---

## 333. Geometriegrundsatz

Räumliche Messungen DÜRFEN nur auf Daten mit definierter und validierter Ultraschallgeometrie ausgeführt werden.

---

## 334. Dopplergrundsatz

Eine Dopplergeschwindigkeit ist nicht allein durch eine Farbskala definiert.

Quantitative Dopplerdaten MÜSSEN unabhängig von ihrer Darstellung gespeichert werden können.

---

## 335. Provenance-Grundsatz

Für relevante abgeleitete Ergebnisse SOLL nachvollziehbar sein:

```text
Welche Quelldaten?
Welche Sonde?
Welche Samplingrate?
Welche Frequenz?
Welche Schallgeschwindigkeit?
Welche Beamforming-Parameter?
Welche Filter?
Welche TGC?
Welche PRF?
Welcher Dopplerwinkel?
Welcher Algorithmus?
Welche Version?
Welches Backend?
```

---

## 336. Erweiterbarkeit

Neue:

```text
Probe Geometries
Beamforming Algorithms
Signal Filters
Doppler Estimators
Scan Conversion Methods
Speckle Algorithms
Elastography Methods
3D Reconstruction Methods
AI Models
Hardware Backends
```

SOLLTEN ohne Änderung des grundlegenden Ultrasound-Compute-Modells integrierbar sein.

---

## 337. Empfohlene Folge-Spezifikationen

Auf dieser Basisspezifikation SOLLEN weitere Detail-Spezifikationen aufbauen.

Empfohlen:

```text
NPSPEC-COMPUTE-ULTRASOUND-SIGNAL-0001
Ultrasound Signal Processing

NPSPEC-COMPUTE-ULTRASOUND-PROBE-0001
Ultrasound Probe Geometry

NPSPEC-COMPUTE-ULTRASOUND-BEAMFORM-0001
Ultrasound Beamforming

NPSPEC-COMPUTE-ULTRASOUND-IQ-0001
Ultrasound IQ Processing

NPSPEC-COMPUTE-ULTRASOUND-BMODE-0001
B-Mode Processing

NPSPEC-COMPUTE-ULTRASOUND-MMODE-0001
M-Mode Processing

NPSPEC-COMPUTE-ULTRASOUND-SCAN-0001
Ultrasound Scan Conversion

NPSPEC-COMPUTE-ULTRASOUND-DOPPLER-0001
Ultrasound Doppler Compute

NPSPEC-COMPUTE-ULTRASOUND-SPECTRAL-0001
Spectral Doppler

NPSPEC-COMPUTE-ULTRASOUND-COLOR-0001
Color Doppler

NPSPEC-COMPUTE-ULTRASOUND-POWER-0001
Power Doppler

NPSPEC-COMPUTE-ULTRASOUND-SPECKLE-0001
Ultrasound Speckle Processing

NPSPEC-COMPUTE-ULTRASOUND-MOTION-0001
Ultrasound Motion Tracking

NPSPEC-COMPUTE-ULTRASOUND-ELASTO-0001
Ultrasound Elastography

NPSPEC-COMPUTE-ULTRASOUND-3D-0001
3D Ultrasound Reconstruction

NPSPEC-COMPUTE-ULTRASOUND-4D-0001
4D Ultrasound

NPSPEC-COMPUTE-ULTRASOUND-REALTIME-0001
Ultrasound Real-Time Pipeline

NPSPEC-COMPUTE-ULTRASOUND-GPU-0001
Ultrasound GPU Backend

NPSPEC-COMPUTE-ULTRASOUND-AI-0001
Ultrasound AI Integration

NPSPEC-COMPUTE-ULTRASOUND-DIAG-0001
Ultrasound Diagnostics

NPSPEC-COMPUTE-ULTRASOUND-TEST-0001
Ultrasound Conformance Tests
```

---

## 338. Zusammenfassung

`NPSPEC-COMPUTE-ULTRASOUND-0001` definiert die spezialisierte Ultraschall-Compute-Infrastruktur der NovaOS-Compute-Architektur.

Die Architektur verarbeitet Ultraschalldaten auf mehreren Ebenen:

```text
Raw Channel Data
        |
        v
      RF Data
        |
        v
    Beamforming
        |
        v
      IQ Data
        |
        v
Envelope / Doppler
        |
        v
 Scan Conversion
        |
        v
 Ultrasound Image
        |
        v
Radiology Compute
```

Unterstützte Verarbeitungsbereiche umfassen:

```text
B-Mode
M-Mode
PW Doppler
CW Doppler
Spectral Doppler
Color Doppler
Power Doppler
Duplex
Triplex
Speckle Processing
Motion Tracking
Elastography
3D Ultrasound
4D Ultrasound
```

Die Compute-Ausführung kann skalieren über:

```text
CPU
SIMD
Multi-Core
GPU
NPU
HPC
Distributed Compute
```

wobei latenzkritische Echtzeitverarbeitung lokal und deadlinebewusst ausgeführt werden SOLL.

Die Architektur trennt konsequent:

```text
Raw Measurement
RF
IQ
Beamformed Data
Quantitative Data
Display Data
Derived Data
AI Results
```

voneinander.

Das grundlegende Architekturprinzip lautet:

> NovaOS Ultrasound Compute behandelt Ultraschall nicht lediglich als Folge fertiger medizinischer Bilder. Die Architektur kann die gesamte digitale Verarbeitungskette von zeit- und kanalaufgelösten akustischen Messdaten über Beamforming, IQ- und Hüllkurvenverarbeitung bis zu B-Mode, Doppler, 3D/4D-Rekonstruktion und KI-gestützter Analyse abbilden. Geometrie, Samplingparameter, Schallgeschwindigkeit, Dopplerparameter, Algorithmen und Verarbeitungsschritte bleiben dabei explizit und nachvollziehbar, sodass Echtzeitverarbeitung, quantitative Analyse und zukünftige Ultraschallverfahren auf einer gemeinsamen Compute-Infrastruktur aufbauen können.
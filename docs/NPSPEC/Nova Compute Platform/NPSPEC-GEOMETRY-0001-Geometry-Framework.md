# NPSPEC-GEOMETRY-0001
# Geometry Framework

| Feld | Wert |
|---|---|
| Dokument | NPSPEC-GEOMETRY-0001 |
| Titel | Geometry Framework |
| Version | 1.0.0 |
| Status | Angenommen |
| Kategorie | System Specification |
| Bereich | Nova Compute Platform |
| Abhängigkeiten | NPSPEC-MATH-0001, NPSPEC-VECTOR-0001, NPSPEC-UNITS-0001 bis 0008 |
| Letzte Änderung | 2026-07-13 |

---

# 1. Zweck

Das Geometry Framework definiert die geometrischen Grundobjekte, Algorithmen und Koordinatensysteme der Nova Compute Platform.

Es dient als gemeinsame Basis für:

- CAD
- Robotik
- Computergrafik
- Simulation
- GIS
- Physik
- Computer Vision
- KI

---

# 2. Ziele

- Einheitliches Geometriemodell
- Dimensions- und Einheitensicherheit
- 2D-, 3D- und nD-Geometrie
- Hohe numerische Stabilität
- GPU-/NPU-Unterstützung
- Reflection und Serialisierung

---

# 3. Architektur

```text
Geometry Framework
├── Point
├── Line
├── Ray
├── Segment
├── Plane
├── Circle
├── Ellipse
├── Polygon
├── Polyline
├── Triangle
├── Mesh
├── Bounding Volumes
├── Coordinate Systems
└── Spatial Algorithms
```

---

# 4. Objektmodell

```c
typedef struct nova_geometry_object
{
    nova_object_header_t object;

    nova_uuid_t uuid;

    nova_coordinate_system_t coordinate_system;

    nova_dimension_uuid_t dimension;
    nova_unit_uuid_t unit;

    nova_geometry_flags_t flags;
} nova_geometry_object_t;
```

Alle Geometrieobjekte leiten sich hiervon ab.

---

# 5. Primitive Objekte

Unterstützt werden mindestens:

- Point2
- Point3
- Line
- Ray
- Segment
- Plane
- Triangle
- Rectangle
- Circle
- Sphere
- AABB
- OBB

---

# 6. Koordinatensysteme

Unterstützung für:

- kartesisch
- polar
- zylindrisch
- sphärisch
- geodätisch
- benutzerdefiniert

Transformationen müssen reproduzierbar sein.

---

# 7. Dimensionsregeln

Alle Koordinaten besitzen eine Quantity.

Beispiel:

```text
Point3<Length>
```

Ungültig:

```text
Point(x=1 m, y=5 s)
```

---

# 8. Geometrische Operationen

- Abstand
- Winkel
- Projektion
- Schnittpunkte
- Translation
- Rotation
- Skalierung
- Spiegelung
- Affine Transformation

Dimensionen bleiben erhalten.

---

# 9. Spatial Algorithms

Das Framework definiert u.a.:

- Point in Polygon
- Ray Casting
- Line Intersection
- Convex Hull
- Delaunay
- Voronoi
- BVH
- KD-Tree

---

# 10. Measurement-Integration

Punkte, Linien und Flächen können Measurement-Objekte enthalten.

Unsicherheiten werden gemäß NPSPEC-UNITS-0004 propagiert.

---

# 11. API

```c
nova_result_t nova_point_create(
    const nova_quantity_t* x,
    const nova_quantity_t* y,
    const nova_quantity_t* z,
    nova_point3_t* out_point);

nova_result_t nova_geometry_distance(
    const nova_point3_t* a,
    const nova_point3_t* b,
    nova_quantity_t* out_distance);

nova_result_t nova_geometry_transform(
    nova_geometry_object_t* object,
    const nova_matrix_t* transform);
```

---

# 12. Performance

Optimierungen:

- SIMD
- GPU
- Spatial Indexing
- Zero-Copy
- Cachefreundliche Layouts

---

# 13. Reflection

Abfragbar:

- Typ
- Koordinatensystem
- Einheit
- Dimension
- Bounding Volume
- Metadaten

---

# 14. Tests

Pflichttests:

- Distanzberechnung
- Schnittpunkte
- Transformationen
- Dimensionsprüfung
- Serialisierung
- Reflection
- GPU-Ausführung

---

# 15. Roadmap

Folgespezifikationen:

- NPSPEC-MESH-0001
- NPSPEC-CAD-0001
- NPSPEC-COLLISION-0001
- NPSPEC-ROBOTICS-0001
- NPSPEC-GIS-0001

---

# 16. Zusammenfassung

Das Geometry Framework stellt die standardisierte geometrische Infrastruktur der Nova Compute Platform bereit. Es verbindet Geometrie mit Units, Quantities und Measurements und schafft eine gemeinsame Grundlage für Robotik, CAD, Simulation, Physik und Computergrafik.

# The .infinity File Format (version 1)

One object per file. UTF-8 JSON. All lengths are stored in **millimeters**
regardless of display-unit settings; all angles in degrees. Display units
are a per-file *setting*, not a property of the stored values - so files
are portable between users with different unit preferences.

## Top level

```json
{
  "format": "infinity-creator-part",
  "version": 1,
  "settings": { ... },
  "features": [ ... ]
}
```

`format` identifies the file; `version` is the schema version. Loaders
refuse files with a newer version than they understand.

## settings

Per-document settings (File > Document Properties). Absent keys mean
"inherit from application settings".

```json
{
  "description": "Mounting bracket for the shelf project",
  "material": "PETG",
  "unitOverride": "inch",              // optional: "mm" | "inch"
  "meshDeflectionOverride": 0.05        // optional, mm
}
```

## features

The ordered feature history. Recompute = replay in order. Every feature
carries:

```json
{
  "type": "Box",              // FeatureFactory type name
  "name": "Base plate",
  "suppressed": false,
  "booleanOp": 1,             // 0 NewBody, 1 Add, 2 Subtract, 3 Intersect
  "parameters": { "dx": 50.0, "dy": 50.0, "dz": 50.0,
                  "x": 0.0, "y": 0.0, "z": 0.0,
                  "rx": 0.0, "ry": 0.0, "rz": 0.0 }
}
```

Parameter keys are defined by each feature type; unknown keys are ignored
on load (forward compatibility for parameter additions within version 1).

Future versions will add sketches, reference geometry, and drawing sheets
as sibling arrays; the version field gates the changes.

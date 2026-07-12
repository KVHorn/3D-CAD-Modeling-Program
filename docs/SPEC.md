# Infinity Creator — Specification

Single-part parametric 3D CAD modeler recreating CATIA V5's sketch → feature
workflow. One object per `.infinity` file. No assemblies, no surfacing, no DMU.
Target: simple to moderately complex printable parts.

Status key: [P#] = phase where it lands. See ARCHITECTURE.md for the phase plan.

## Workbenches

- **Part Design** [P1+] — solid features on the active part
- **Sketcher** [P5–P7] — 2D profiles on a plane, constraint-driven
- **Drafting** [P10–P11] — snapshot blueprint drawings of a part

## Part Design capabilities

**Primitives** [P1 ✓]: Box, Cylinder, Sphere, Cone, Torus — placement
(position + rotation) and boolean operation (new body / add / subtract /
intersect) via definition dialogs.

**Sketch-based features**: Pad, Pocket, Shaft (revolve), Hole with proper
Hole Definition dialog [P8]; Slot, Groove (profile swept along guide) [P9].
Hole subtypes (counterbore/countersink): open item, decide by P8.

**Dress-up features**: Fillet, Chamfer — all-edges [P2], edge-selection [P4];
Draft, Shell, Thickness [P4] (require face picking).

**Reference elements** [P4]: origin planes (xy/yz/zx, shown in tree), offset
plane, plane-on-face, reference points/lines. Face/edge references survive
recompute via geometric re-matching (pragmatic topological naming — known
limitation: industrial-grade naming is out of scope; re-matching degrades
gracefully on simple-to-moderate parts).

**Transformation features** [P9]: translate / rotate / mirror / scale body;
rectangular pattern (rows × cols × spacing) and circular pattern
(count × angle) — exact parameters are an open item.

**Specification tree** [P2]: ordered feature history; double-click to edit
definition, suppress/unsuppress, delete with recompute. Recompute = replay;
failures reported per feature.

## Sketcher capabilities

**Entities** [P5]: point, line, infinite line, axis, arc, circle (all
construction variants: three-point, radius/angle, tri-tangent, etc.),
construction geometry, center point. Conics (ellipse/parabola/hyperbola):
**deferred** — revisit after core is complete; drawable-ellipse can be added
without solver rework.

**Predefined profiles** [P7]: rectangle, oriented rectangle, parallelogram,
centered rectangle/parallelogram, elongated hole, cylindrical elongated hole,
keyhole, hexagon — implemented as entity+constraint macros.

**Constraints & dimensions** [P6]: coincident, parallel, perpendicular,
tangent, horizontal/vertical, fix, fix together; dimensions (distance,
length, offset, radius, diameter, angle); constraints by dialog;
multi-constraint edit. Solver: Newton-Raphson numeric.

**Relimitation** [P7]: trim, quick trim, break, close, complement.

**Transformations** [P7]: mirror, symmetry, translate, rotate, scale, offset.

**Editing** [P5–P7]: snapping, normal-to-sketch view, copy/paste of sketch
entities, sketch dimensions displayed in-canvas.

## Drafting capabilities

**Model**: "Generate Draft" button snapshots the active part **as it exists
at that moment** and opens a drawing document (its own MDI window with the
Drafting workbench). Drawings are static — model edits do not update them;
regenerate for a fresh sheet. No associativity, by design.

**Views** [P10]: third-angle three-view (front/top/right) + isometric, via
OCCT HLR — visible edges solid, hidden edges dashed, exact model coordinates.

**Dimensions** [P10]: measured from the frozen solid (never approximated,
never derived from sketch constraints — constraint-derived dimensioning is
explicitly cut). Snap system: endpoints, midpoints, circle/arc centers,
quadrant points; new snap types are catalog additions. Tool cases:
point↔point (any snap combination), point↔edge (perpendicular distance),
edge↔edge (parallel → distance, else angle; tight epsilon, manual override
later), radius, diameter; arc/line combinations as the tool matures.
Distances are measured in the projection plane (standard drafting semantics).
Tolerance text (±x.x / fit class) rendered after values.

**Sheet** [P10]: title block (editable name/date/scale/material), ISO/ASME
style presets (arrowheads, text placement, first- vs third-angle layout),
PDF export.

**v2** [P11]: section views (plane cut + hatch), detail views (circled region
at scale), GD&T feature-control frames — **render-only** (no datum
validation/metrology).

## Menu system (7 tabs)

- **File**: New, Open, Close | Save, Save As, Save As Copy, Save All
  (unsaved → Obj1.infinity, Obj2...) | Print (viewport screenshot via
  QPrinter), Printer Setup | Document Properties (per-file settings),
  Document History (recent 4–5) | Exit (save prompts for dirty documents).
  Cut: Send To. Deferred: New From (templates).
- **Edit**: Undo, Redo | Update (manual recompute escape hatch) | Cut/Copy/
  Paste (sketcher scope) | Delete | Properties (selected feature's dialog) |
  **Options** (application settings — deliberate scoping: Edit = program-wide,
  File = this document). Cut: Paste Special, Search.
- **View**: Toolbars ▶ customization dialog [P12] | Specification Tree
  (checkable), View Cube (checkable) | Fit All In, Zoom Area, Zoom In/Out,
  Pan, Rotate | Named Views (front/top/right/iso) | Render Style ▶ (Shaded /
  Shaded+Edges / Wireframe) | Full Screen. Cut: fly mode, compass, lighting,
  depth effect, ground, magnifier, overviews, commands list.
- **Insert**: Sketch | Sketch-Based Features ▶ | Dress-Up ▶ |
  Transformation ▶ | Reference Elements ▶ | Primitives ▶. Cut: bodies/
  geometrical sets, annotations, surface-based, analysis, knowledge.
- **Workspace**: Part Design, Sketcher (Drafting activates via drawing docs).
- **Window**: New Window (deferred), Tile Horizontally, Tile Vertically,
  Cascade | open-documents list inline in the menu.
- **Help**: Infinity Creator Help | User Companion — separate window,
  left-side navigation map, numbered sections (1, 1.1, 1.2.1 ...), content
  written per-phase, English-only | About.

Tools tab: removed (its sole item, Options, lives under Edit).

## Settings model

- **Application settings** (Edit > Options, QSettings/registry): units
  default, export mesh quality default, colors, undo depth, toolbar
  visibility.
- **Per-file settings** (File > Document Properties, stored inside
  .infinity): every value defaults to "inherit from application settings"
  with optional override — units, export mesh quality, part name/
  description/material (feeds drafting title block). Overrides travel with
  the file; clearing an override resumes tracking the app default.
  Discipline: a setting earns a per-file override only if it genuinely
  varies per object.

## File formats

- **.infinity** — native; JSON (feature history + sketches + references +
  document settings + drawing sheets), versioned schema (docs/FILE_FORMAT.md
  arrives in P3).
- **Export**: STL (meshed, deflection from settings) [P1 basic ✓ / P3 final],
  3MF (OPC zip via minizip) [P3].
- **Import**: none planned for v1.

## Open items (decide later; none block P1–P2)

Units policy (decide by P3 — touches file format); hole subtypes (by P8);
pattern parameters (by P9); mouse bindings (current: LMB rotate / MMB pan /
wheel zoom — CATIA scheme or configurable later); sketch-entry flow (pick
plane then Sketch vs. Sketch then pick — a primary must be chosen by P5);
autosave/crash recovery; .infinity Explorer file association; keyboard
shortcut map; undo depth default (50 suggested).

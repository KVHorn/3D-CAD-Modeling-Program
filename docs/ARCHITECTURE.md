# Infinity Creator — Architecture & Phase Plan

## Delivery model

All source is generated in phases. Every phase ships as a **complete
snapshot** of the whole project (new files + revised existing files +
untouched files) — snapshots replace the working copy entirely; nothing is
ever merged by hand. Each phase must build and run before the next begins
(the build gate). Commit each snapshot to git when it builds; local edits
between phases must be reported before the next snapshot ships or they will
be overwritten.

## Final directory layout

Files marked ✓ exist as of Phase 1. Everything else arrives in the phase
noted in brackets.

```
3D-CAD-Modeling-Program/
├── CMakeLists.txt ✓            CMakePresets.json ✓
├── run.bat ✓                   .gitignore ✓         README.md ✓
├── docs/
│   ├── SPEC.md ✓               ARCHITECTURE.md ✓ (this file)
│   └── FILE_FORMAT.md [P3]     # .infinity JSON schema, versioned
├── resources/ [P12]
│   ├── resources.qrc
│   └── icons/
└── src/
    ├── main.cpp ✓              # file logging, QApplication
    │
    ├── app/
    │   ├── MainWindow.h/.cpp ✓ # MDI shell, workbench switching
    │   ├── MenuBuilder.h/.cpp [P2]      # 7-tab menu system; items enable
    │   │                                #   as their phases land
    │   ├── DocumentBase.h/.cpp [P10]    # shared doc base (part & drawing)
    │   ├── Document.h/.cpp ✓            # part doc: viewport + PartModel
    │   │                                #   (renamed PartDocument in P10)
    │   ├── DrawingDocument.h/.cpp [P10] # drawing sheet window
    │   ├── FeatureTreeDock.h/.cpp [P2]  # specification tree
    │   ├── UndoStack.h/.cpp [P2]
    │   ├── AppSettings.h/.cpp [P3]      # QSettings wrapper (Edit > Options)
    │   └── RecentFiles.h/.cpp [P3]      # File > Document History
    │
    ├── workspaces/
    │   ├── WorkspaceType.h ✓            # + Drafting [P10]
    │   ├── Workspace.h/.cpp ✓
    │   ├── PartDesignWorkspace.h/.cpp ✓ # grows through P2/P4/P8/P9/P10
    │   ├── SketcherWorkspace.h/.cpp ✓   # real tools P5–P7
    │   └── DraftingWorkspace.h/.cpp [P10]
    │
    ├── viewport/
    │   ├── OcctViewport.h/.cpp ✓        # + face/edge picking [P4]
    │   └── ViewCube.h/.cpp [P11]
    │
    ├── model/                           # parametric engine (UI-free)
    │   ├── Feature.h/.cpp ✓  FeatureParameter.h ✓  BooleanOp.h ✓
    │   ├── PrimitiveFeatures.h/.cpp ✓
    │   ├── PartModel.h/.cpp ✓  FeatureFactory.h/.cpp ✓
    │   ├── DressUpFeatures.h/.cpp [P2]  # fillet/chamfer; +draft/shell/
    │   │                                #   thickness [P4]
    │   ├── ReferenceGeometry.h/.cpp [P4]
    │   ├── ShapeReference.h/.cpp [P4]   # geometric re-matching (pragmatic
    │   │                                #   topological naming)
    │   ├── SketchFeatures.h/.cpp [P8]   # Pad/Pocket/Shaft/Hole; +Slot/
    │   │                                #   Groove [P9]
    │   ├── TransformFeatures.h/.cpp [P9]
    │   └── DocumentSettings.h/.cpp [P3] # per-file inherit-or-override store
    │
    ├── sketcher/ [P5–P7]
    │   ├── Sketch.h/.cpp                SketchEntities.h/.cpp
    │   ├── PredefinedProfiles.h/.cpp    Constraint.h/.cpp
    │   ├── ConstraintSolver.h/.cpp      Relimitation.h/.cpp
    │   ├── SketchTransforms.h/.cpp      SketchEditor.h/.cpp
    │
    ├── drafting/ [P10–P11]
    │   ├── HlrProjector.h/.cpp          DrawingSheet.h/.cpp
    │   ├── SnapEngine.h/.cpp            DimensionItem.h/.cpp
    │   ├── DraftingStyle.h/.cpp         TitleBlock.h/.cpp
    │   ├── SheetExporter.h/.cpp
    │   └── (P11: SectionView, DetailView, GdtSymbols — render-only)
    │
    ├── ui/
    │   ├── ParameterDialog.h/.cpp ✓
    │   ├── OptionsDialog.h/.cpp [P3]
    │   ├── DocumentPropertiesDialog.h/.cpp [P3]
    │   └── ToolbarCustomizeDialog.h/.cpp [P12]
    │
    ├── help/ [P12]
    │   ├── UserCompanionWindow.h/.cpp
    │   └── manual/                      # per-phase content, via .qrc
    │
    └── io/ [P3]
        ├── InfinityFormat.h/.cpp        # history + sketches + references +
        │                                #   settings + sheets
        ├── MeshUtils.h/.cpp             StlExporter.h/.cpp
        └── ThreeMfExporter.h/.cpp       # OPC zip (requires
                                         #   vcpkg install minizip:x64-windows)
```

## Phases

1. ✓ **Feature engine** — primitives, booleans, definition dialogs,
   recompute, MDI + workspaces, viewport, STL (basic), file logging.
2. **Specification tree** — FeatureTreeDock (edit/suppress/delete),
   Fillet/Chamfer (all-edges), MenuBuilder skeleton (7 tabs, items graying
   in), Undo/Redo/Delete/Properties.
3. **Files** — .infinity save/open/save-as/save-all, dirty tracking + exit
   prompts, recent files, Document Properties (inherit-or-override),
   Options dialog, io/ module: 3MF export, STL refactor. Decide units here.
4. **Reference geometry & picking** — origin/offset/on-face planes,
   face/edge selection, ShapeReference, edge-select fillet/chamfer,
   Draft/Shell/Thickness.
5. **Sketcher I** — entities, drawing, snapping, normal-to view.
6. **Sketcher II** — constraint solver, constraints, dimensions.
7. **Sketcher III** — relimitation, transforms, predefined profiles,
   copy/paste.
8. **Sketch-based features** — Pad, Pocket, Shaft, Hole. (The program
   becomes the product here.)
9. **Advanced features** — Slot, Groove, patterns, body transforms.
10. **Drafting I** — DocumentBase split, drawing documents, HLR views
    (3-view + iso), snap engine, dimension tool, title block, styles,
    PDF export.
11. **Drafting II & view polish** — sections, details, render-only GD&T;
    named views, render styles, zoom area, view cube.
12. **Polish & help** — toolbar customization, shortcuts, print, User
    Companion, icons, remaining menu items.

Estimated final size: 25,000–40,000 lines.

## Core design decisions (recorded)

- Recompute = replay of the ordered feature history; every feature
  serializes itself (toJson/fromJson) → .infinity is "the history, written
  down."
- Documents are MDI sub-windows; workbenches are shared toolbar sets
  activated per focused document.
- One shared OpenGl_GraphicDriver across all viewports.
- GUI-subsystem exe; diagnostics to debug.log (qInstallMessageHandler).
- Drafting is snapshot-based and non-associative; dimensions are measured
  from exact projected geometry, never derived from constraints.
- Topological naming via geometric re-matching — accepted limitation.
- Settings: Edit > Options = app-wide (QSettings); File > Document
  Properties = per-file overrides stored in .infinity.
- Conics deferred; fly mode, DMU, drafting-associativity, GD&T validation,
  assemblies: out of scope.

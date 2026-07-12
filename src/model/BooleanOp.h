#ifndef BOOLEANOP_H
#define BOOLEANOP_H

// How a feature's geometry combines with the part built so far.
// Mirrors CATIA's insert modes: a new body, or fuse/cut/intersect
// against the running result.
enum class BooleanOp
{
    NewBody = 0,   // replace the running result
    Add = 1,       // BRepAlgoAPI_Fuse
    Subtract = 2,  // BRepAlgoAPI_Cut
    Intersect = 3  // BRepAlgoAPI_Common
};

#endif // BOOLEANOP_H

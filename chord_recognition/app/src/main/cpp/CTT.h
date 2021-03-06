//
// Created by sew on 4/13/19.
//

#ifndef CHORD_RECOGNITION_CTT_H
#define CHORD_RECOGNITION_CTT_H

#include <string>

#define CTT_LENGTH 48

const float CTT[CTT_LENGTH][12] {
        // MAJOR Chords
        {1,0,0.05,0,1,0,0,1,0,0,0,0.05},    // Cmaj
        {0.05,1,0,0.05,0,1,0,0,1,0,0,0},    // C#maj
        {0,0.05,1,0,0.05,0,1,0,0,1,0,0},    // Dmaj
        {0,0,0.05,1,0,0.05,0,1,0,0,1,0},    // D#maj
        {0,0,0,0.05,1,0,0.05,0,1,0,0,1},    // Emaj
        {1,0,0,0,0.05,1,0,0.05,0,1,0,0},    // Fmaj
        {0,1,0,0,0,0.05,1,0,0.05,0,1,0},    // F#maj
        {0,0,1,0,0,0,0.05,1,0,0.05,0,1},    // Gmaj
        {1,0,0,1,0,0,0,0.05,1,0,0.05,0},    // G#maj
        {0,1,0,0,1,0,0,0,0.05,1,0,0.05},    // Amaj
        {0.05,0,1,0,0,1,0,0,0,0.05,1,0},    // A#maj
        {0,0.05,0,1,0,0,1,0,0,0,0.05,1},    // Bmaj

        // MINOR Chords
        {1,0,0,1,0,0,0,1,0,0,0,0},          // Cmin
        {0,1,0,0,1,0,0,0,1,0,0,0},          // C#min
        {0,0,1,0,0,1,0,0,0,1,0,0},          // Dmin
        {0,0,0,1,0,0,1,0,0,0,1,0},          // D#min
        {0,0,0,0,1,0,0,1,0,0,0,1},          // Emin
        {1,0,0,0,0,1,0,0,1,0,0,0},          // Fmin
        {0,1,0,0,0,0,1,0,0,1,0,0},          // F#min
        {0,0,1,0,0,0,0,1,0,0,1,0},          // Gmin
        {0,0,0,1,0,0,0,0,1,0,0,1},          // G#min
        {1,0,0,0,1,0,0,0,0,1,0,0},          // Amin
        {0,1,0,0,0,1,0,0,0,0,1,0},          // A#min
        {0,0,1,0,0,0,1,0,0,0,0,1},          // Bmin



        //DIM Chords
        {1,0,0,1,0,0,1,0,0,0,0,0},          //Cdim
        {0,1,0,0,1,0,0,1,0,0,0,0},          // C#dim
        {0,0,1,0,0,1,0,0,1,0,0,0},          // Ddim
        {0,0,0,1,0,0,1,0,0,1,0,0},          // D#dim
        {0,0,0,0,1,0,0,1,0,0,1,0},          // Edim
        {0,0,0,0,0,1,0,0,1,0,0,1},          // Fdim
        {1,0,0,0,0,0,1,0,0,1,0,0},          // F#dim
        {0,1,0,0,0,0,0,1,0,0,1,0},          // Gdim
        {0,0,1,0,0,0,0,0,1,0,0,1},          // G#dim
        {1,0,0,1,0,0,0,0,0,1,0,0},          // Adim
        {0,1,0,0,1,0,0,0,0,0,1,0},          // A#dim
        {0,0,1,0,0,1,0,0,0,0,0,1},          // Bdim

        // SINGLE Notes
        {1,-0.3,-0.3,-0.3,-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3},          // C
        {-0.3,1,-0.3,-0.3,-0.3,-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3},          // C#
        {-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.5,-0.3,-0.3,0.0,-0.3,-0.3},          // D
        {-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.3,-0.3,-0.3,0.0,-0.3},          // D#
        {-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.3,-0.3,-0.3,0.0},          // E
        {0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.3,-0.3,-0.3},          // F
        {-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.3,-0.3},          // F#
        {-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3,-0.5},          // G
        {-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3,-0.3},          // G#
        {-0.3,-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3,-0.3},          // A
        {-0.3,-0.3,-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1,-0.3},          // A#
        {-0.3,-0.3,-0.3,-0.3,-0.3,-0.3,0.0,-0.3,-0.3,-0.3,-0.3,1},          // B


};


const std::string Chord_Names[CTT_LENGTH] = {

        //Major Chords
        {"C Major"},
        {"C# Major"},
        {"D Major"},
        {"D# Major"},
        {"E Major"},
        {"F Major"},
        {"F# Major"},
        {"G Major"},
        {"G# Major"},
        {"A Major"},
        {"A# Major"},
        {"B Major"},

        //Minor Chords
        {"C Minor"},
        {"C# Minor"},
        {"D Minor"},
        {"D# Minor"},
        {"E Minor"},
        {"F Minor"},
        {"F# Minor"},
        {"G Minor"},
        {"G# Minor"},
        {"A Minor"},
        {"A# Minor"},
        {"B Minor"},

        //Dim Chords
        {"C Diminished"},
        {"C# Diminished"},
        {"D Diminished"},
        {"D# Diminished"},
        {"E Diminished"},
        {"F Diminished"},
        {"F# Diminished"},
        {"G Diminished"},
        {"G# Diminished"},
        {"A Diminished"},
        {"A# Diminished"},
        {"B Diminished"},

        //Single Notes
        {"C"},
        {"C#"},
        {"D"},
        {"D#"},
        {"E"},
        {"F"},
        {"F#"},
        {"G"},
        {"G#"},
        {"A"},
        {"A#"},
        {"B"}



};

#endif //CHORD_RECOGNITION_CTT_H

package ece420.chord_recognition;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.Rect;
import android.util.TypedValue;
import android.view.View;
import android.widget.ImageView;

public class drawPCPGraph {
    private static int paddingx = 0, paddingy = 0;
    private static int fontSize = 0;
    private static int distbetlines = 0;
    private static int width = 0, height = 0;
    private static int PCP_SIZE = 12;
    private static boolean initLines = false;
    private static int previousChord = 100000;
    private static int previousRChord = 1000000;
    private static int previousLChord = 1000000;
    private static int magnitudewidth = 50;

    private static Rect rect = new Rect();

    private static String[] pitchClasses = {"C", "C#", "D","D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

    private static void initLines(Canvas canvas, ImageView pitchView){
        paddingx = 25;
        paddingy = 50;
        distbetlines = 40;
        width = pitchView.getWidth();
        height = pitchView.getHeight();
        fontSize = (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP,13,pitchView.getResources().getDisplayMetrics());


        initLines = true;


    }

    private static void drawLines(Canvas canvas, Paint paint, ImageView pitchView) {
        paint.setColor(pitchView.getResources().getColor(android.R.color.black));
        paint.setStrokeWidth(2);
        for(int i = 0; i < 12; i++){
            int ystart = paddingy + distbetlines*i;
            canvas.drawLine(paddingx, ystart,  width - paddingx, ystart, paint);
            drawPitchClass(i, ystart, canvas, paint);
        }


    }

    private static void drawPitchClass(int i, int ystart, Canvas canvas, Paint paint) {
        paint.setTextSize(fontSize);
        String tmp = pitchClasses[(pitchClasses.length-1)-i];
        paint.getTextBounds(tmp, 0, tmp.length(), rect);
        int x = paddingx;
        int y = ystart - 10;
        canvas.drawText(tmp, x, y, paint);
    }

    protected static void drawIt(Canvas canvas, Paint paint, ImageView pitchView, Canvas animationCanvas, Bitmap animationBitmap, ImageView animationView, int Chord_index){
        if(!initLines){
            initLines(canvas, pitchView);
        }
        canvas.drawColor(Color.WHITE);
        drawLines(canvas, paint, pitchView);
        animateCurrentChord(paint,animationCanvas, animationBitmap, animationView, Chord_index);
        pitchView.invalidate();
        animationView.invalidate();
    }

    private static void animateCurrentChord(Paint paint, Canvas animationCanvas, Bitmap animationBitmap, ImageView animationView, int Chord_index) {
        int ColRightr = 255;
        int ColRightg = 0;
        int ColRightb = 0;
        int index = 0;
        if(Chord_index != 0){
            index = Chord_index - 1;
        }
        else{
            animationCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        }

        if (previousChord != Chord_index) {
            //animationCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            Rect srcRect = new Rect(250, -(-1), animationBitmap.getWidth(), animationBitmap.getHeight());
            Rect destRect = new Rect(srcRect);
            destRect.offset(-magnitudewidth - 10, 0);
            animationCanvas.drawBitmap(animationBitmap, srcRect, destRect, null);
           // animationCanvas.drawBitmap(animationBitmap, 0,0,null);

            paint.setStyle(Paint.Style.FILL);

            for (int i = 0; i < PCP_SIZE; i++) {
                if (CTT.getCTTdata(index, i) == 1) {
                    int top = 30 + distbetlines*(11-i);
                    int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                    paint.setColor(Color.argb(255, ColRightr, ColRightg, ColRightb));
                    animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                }
                if (CTT.getCTTdata(index, i) == 0) {
                    int top = 30 + distbetlines*(11-i);
                    int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                    paint.setColor(Color.argb(255, 255, 255, 255));
                    animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                }
            }

            previousChord = Chord_index;
        }


    }

    private static void animatedualCurrentChord(Paint paint, Canvas animationCanvas, Bitmap animationBitmap, ImageView animationView, int RChord_index, int LChord_index) {
        int ColRightr = 255;
        int ColRightg = 0;
        int ColRightb = 0;
        int Rindex = 0, Lindex = 0;
        if(RChord_index != 0){
            Rindex = RChord_index - 1;
        }
        if(LChord_index != 0) {
            Lindex = LChord_index - 1;
        }

        if(RChord_index != 0 && LChord_index != 0){
            animationCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
        }

        if (previousRChord != RChord_index || previousLChord != LChord_index) {
            //animationCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            Rect srcRect = new Rect(250, -(-1), animationBitmap.getWidth(), animationBitmap.getHeight());
            Rect destRect = new Rect(srcRect);
            destRect.offset(-magnitudewidth - 10, 0);
            animationCanvas.drawBitmap(animationBitmap, srcRect, destRect, null);
            // animationCanvas.drawBitmap(animationBitmap, 0,0,null);

            paint.setStyle(Paint.Style.FILL);

            for (int i = 0; i < PCP_SIZE; i++) {

                //Right Hand
                     if (CTT.getCTTdata(Rindex, i) == 1) {
                         int top = 30 + distbetlines*(11-i);
                         int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                         paint.setColor(Color.argb(255, ColRightr, ColRightg, ColRightb));
                         animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                     }
                     if (CTT.getCTTdata(Rindex, i) == 0) {
                         int top = 30 + distbetlines*(11-i);
                         int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                         paint.setColor(Color.argb(255, 255, 255, 255));
                         animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                     }

                //Left Hand
                     if (CTT.getCTTdata(Lindex, i) == 1) {
                         int top = 15 + distbetlines*(11-i);
                         int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                         paint.setColor(Color.argb(255, 0, 0, 255));
                         animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                     }
                     if (CTT.getCTTdata(Rindex, i) == 0) {
                         int top = 15 + distbetlines*(11-i);
                         int startx = animationBitmap.getWidth() - paddingx - magnitudewidth;
                         paint.setColor(Color.argb(255, 255, 255, 255));
                         animationCanvas.drawRect(startx, top, startx + magnitudewidth , top + 15, paint);
                     }


            }


        }



        previousRChord = RChord_index;
        previousLChord = LChord_index;

    }


}

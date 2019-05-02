package ece420.chord_recognition;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.text.DecimalFormat;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.Timer;
import java.util.TimerTask;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import android.os.AsyncTask;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;


import android.support.v7.app.AppCompatActivity;

public class FILE extends AppCompatActivity implements ActivityCompat.OnRequestPermissionsResultCallback
{
    EditText editSourceFile = null;
    TextView idk_view;
    private static final int READ_REQUEST = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file);

        editSourceFile = (EditText)findViewById(R.id.editTextSrcFileName);
        idk_view = (TextView)findViewById(R.id.idk);
        idk_view.setMovementMethod(new ScrollingMovementMethod());
    }

    private void startProcess() {
        Dictionary CTT_name = new Hashtable();
        CTT_name.put("0","No Chord");
        CTT_name.put("1","Cmaj");
        CTT_name.put("2","C#maj");
        CTT_name.put("3","Dmaj");
        CTT_name.put("4","D#maj");
        CTT_name.put("5","Emaj");
        CTT_name.put("6","Fmaj");
        CTT_name.put("7","F#maj");
        CTT_name.put("8","Gmaj");
        CTT_name.put("9","G#maj");
        CTT_name.put("10","Amaj");
        CTT_name.put("11","A#maj");
        CTT_name.put("12","Bmaj");
        CTT_name.put("13","Cmin");
        CTT_name.put("14","C#min");
        CTT_name.put("15","Dmin");
        CTT_name.put("16","D#min");
        CTT_name.put("17","Emin");
        CTT_name.put("18","Fmin");
        CTT_name.put("19","F#min");
        CTT_name.put("20","Gmin");
        CTT_name.put("21","G#min");
        CTT_name.put("22","Amin");
        CTT_name.put("23","A#min");
        CTT_name.put("24","Bmin");
        CTT_name.put("25","Cdim");
        CTT_name.put("26","C#dim");
        CTT_name.put("27","Ddim");
        CTT_name.put("28","D#dim");
        CTT_name.put("29","Edim");
        CTT_name.put("30","Fdim");
        CTT_name.put("31","F#dim");
        CTT_name.put("32","Gdim");
        CTT_name.put("33","G#dim");
        CTT_name.put("34","Adim");
        CTT_name.put("35","A#dim");
        CTT_name.put("36","Bdim");
        CTT_name.put("37","C");
        CTT_name.put("38","C#");
        CTT_name.put("39","D");
        CTT_name.put("40","D#");
        CTT_name.put("41","E");
        CTT_name.put("42","F");
        CTT_name.put("43","F#");
        CTT_name.put("44","G");
        CTT_name.put("45","G#");
        CTT_name.put("46","A");
        CTT_name.put("47","A#");
        CTT_name.put("48","B");
        try
        {
            // Open the wav file specified as the first argument
            WavFile wavFile = WavFile.openWavFile(new File(editSourceFile.getText().toString()));

            // Get the number of audio channels in the wav file
            //int numChannels = wavFile.getNumChannels();
            int newRChord = 0, newLChord = 0;
            int oldRChord = -1, oldLChord = -1;
            double i = 0;
            double time = 0;

            // Create a buffer of 8192 frames
            IntBuffer currentChords = ByteBuffer.allocateDirect(2*4)
                    .order(ByteOrder.LITTLE_ENDIAN)
                    .asIntBuffer();
            double[] buffer = new double[8192];

            int framesRead;
            String result = "";
            do
            {
                // Read frames into buffer
                framesRead = wavFile.readFrames(buffer, 8192);

                getChordFile(buffer, currentChords);
                int righthand = currentChords.get();
                int lefthand = currentChords.get();
                currentChords.rewind();
                if ((lefthand != oldLChord && lefthand != 0) && i != 0) {
                    time = i * 8192 / 44100;
                    DecimalFormat df = new DecimalFormat("#0.0000");
                    result = result + "At time " + df.format(time) + "s: " + "Left Hand: " + CTT_name.get(Integer.toString(lefthand)).toString() + "\n";
                }
                if (righthand != oldRChord && righthand != 0 && i != 0) {
                    time = i * 8192 / 44100;
                    DecimalFormat df = new DecimalFormat("#0.0000");
                    result = result + "At time " + df.format(time) + "s: " +" Right Hand: " + CTT_name.get(Integer.toString(righthand)).toString() +"\n";
                }
                oldRChord = righthand;
                oldLChord = lefthand;
                i = i + 1;
            }
            while (framesRead != 0);

            idk_view.setText(result);

            // Close the wavFile
            wavFile.close();

        }
        catch (Exception e)
        {
            idk_view.setText(e.toString());
        }
    }

    public void onProcessClick(View view)
    {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) !=
                PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.READ_EXTERNAL_STORAGE }, READ_REQUEST);
            return;
        }
        startProcess();
    }

    /*
     * Loading our Libs
     */
    static {
        System.loadLibrary("echo");
    }


    public static native int getChordFile(double[] buffer, IntBuffer currentChords);
}


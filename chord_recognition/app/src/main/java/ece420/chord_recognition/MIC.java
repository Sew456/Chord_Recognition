package ece420.chord_recognition;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.Manifest;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.support.v7.app.AppCompatActivity;

import java.nio.charset.Charset;
import java.util.*;

public class MIC extends Activity
        implements ActivityCompat.OnRequestPermissionsResultCallback {

    // UI Variables
    Button startButton;
    TextView chord_view;
    Boolean isPlaying = false;
    String  nativeSampleRate;
    String  nativeSampleBufSize;
    boolean supportRecording;

    // Drawing UI Variables
    Canvas canvas;
    Canvas animationCanvas;
    Bitmap bitmap;
    Bitmap animationBitmap;
    Paint paint;
    ImageView pitchView;
    ImageView animationView;

    // Static Values
    private static final int AUDIO_ECHO_REQUEST = 0;
    private static final int FRAME_SIZE = 8192;
    private static final int PCP_SIZE = 12;

    // Static UI Drawing Values
    private static final int BITMAP_HEIGHT = 500;
    private static final int BITMAP_WIDTH = 750;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_mic);
        super.setRequestedOrientation (ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        // Google NDK Stuff
        startButton = (Button)findViewById((R.id.buttonStart));
        queryNativeAudioParameters();

        // initialize native audio system
        if (supportRecording) {
            createSLEngine(Integer.parseInt(nativeSampleRate), FRAME_SIZE);
        }

        // Setup UI
        chord_view = (TextView)findViewById(R.id.textChord);

        // Setup Drawing
        pitchView = this.findViewById(R.id.pitchView);
        animationView = this.findViewById(R.id.animationView);
        animationBitmap = Bitmap.createBitmap((BITMAP_WIDTH), BITMAP_HEIGHT, Bitmap.Config.ARGB_8888);
        bitmap = Bitmap.createBitmap((BITMAP_WIDTH), BITMAP_HEIGHT, Bitmap.Config.ARGB_8888);
        canvas = new Canvas(bitmap);
        animationCanvas = new Canvas(animationBitmap);
        canvas.drawColor(Color.WHITE);
        paint = new Paint();
        pitchView.setImageBitmap(bitmap);
        animationView.setImageBitmap(animationBitmap);

        initializeFreqTextBackgroundTask(100);
    }
    @Override
    protected void onDestroy() {
        if (supportRecording) {
            if (isPlaying) {
                stopPlay();
            }
            deleteSLEngine();
            isPlaying = false;
        }
        super.onDestroy();
    }

    private void startEcho() {
        if(!supportRecording){
            return;
        }
        if (!isPlaying) {
            if(!createSLBufferQueueAudioPlayer()) {
                return;
            }
            if(!createAudioRecorder()) {
                deleteSLBufferQueueAudioPlayer();
                return;
            }
            startPlay();   // this must include startRecording()
        } else {
            stopPlay();  //this must include stopRecording()
            deleteAudioRecorder();
            deleteSLBufferQueueAudioPlayer();
        }
        isPlaying = !isPlaying;
        if (isPlaying == true) {
            startButton.setText("Stop");
        }
        else {
            startButton.setText("Start");
        }
    }

    public void onEchoClick(View view) {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) !=
                PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                    this,
                    new String[] { Manifest.permission.RECORD_AUDIO },
                    AUDIO_ECHO_REQUEST);
            return;
        }
        startEcho();
    }

    private void queryNativeAudioParameters() {
        AudioManager myAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        nativeSampleRate  =  myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        nativeSampleBufSize =myAudioMgr.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int recBufSize = AudioRecord.getMinBufferSize(
                Integer.parseInt(nativeSampleRate),
                AudioFormat.CHANNEL_IN_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
        supportRecording = true;
        if (recBufSize == AudioRecord.ERROR ||
                recBufSize == AudioRecord.ERROR_BAD_VALUE) {
            supportRecording = false;
        }
    }


    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        /*
         * if any permission failed, the sample could not play
         */
        if (AUDIO_ECHO_REQUEST != requestCode) {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
            return;
        }

        if (grantResults.length != 1  ||
                grantResults[0] != PackageManager.PERMISSION_GRANTED) {
            /*
             * When user denied permission, throw a Toast to prompt that RECORD_AUDIO
             * is necessary; also display the status on UI
             * Then application goes back to the original state: it behaves as if the button
             * was not clicked. The assumption is that user will re-click the "start" button
             * (to retry), or shutdown the app in normal way.
             */
            return;
        }

        /*
         * When permissions are granted, we prompt the user the status. User would
         * re-try the "start" button to perform the normal operation. This saves us the extra
         * logic in code for async processing of the button listener.
         */

        // The callback runs on app's thread, so we are safe to resume the action
        startEcho();
    }

    // All this does is calls the UpdateStftTask at a fixed interval
    // http://stackoverflow.com/questions/6531950/how-to-execute-async-task-repeatedly-after-fixed-time-intervals
    public void initializeFreqTextBackgroundTask(int timeInMs) {
        final Handler handler = new Handler();
        Timer timer = new Timer();
        TimerTask doAsynchronousTask = new TimerTask() {
            @Override
            public void run() {
                handler.post(new Runnable() {
                    public void run() {
                        try {
                            UpdateFreqTextTask performFreqTextUpdate = new UpdateFreqTextTask();
                            performFreqTextUpdate.execute();
                        } catch (Exception e) {
                            // TODO Auto-generated catch block
                        }
                    }
                });
            }
        };
        timer.schedule(doAsynchronousTask, 0, timeInMs); // execute every 100 ms
    }

    // UI update
    private class UpdateFreqTextTask extends AsyncTask<Void, Integer, Void> {
        @Override
        protected Void doInBackground(Void... params) {

            // Update screen, needs to be done on UI thread
            publishProgress(getChord());

            return null;
        }

        protected void onProgressUpdate(Integer... newChord) {
            Dictionary CTT_name = new Hashtable();
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
            if (newChord[0] > 0) {
                chord_view.setText(CTT_name.get(Integer.toString(newChord[0])).toString());
            }
            else {chord_view.setText("Unvoiced");};


            //draw the imageView
            canvas.drawBitmap(bitmap, 0, 0, null);

            drawPCPGraph.drawIt(canvas ,paint, pitchView, animationCanvas, animationBitmap, animationView, newChord[0]);

        }
    }


    /*
     * Loading our Libs
     */
    static {
        System.loadLibrary("echo");
    }

    /*
     * jni function implementations...
     */
    public static native void createSLEngine(int rate, int framesPerBuf);
    public static native void deleteSLEngine();

    public static native boolean createSLBufferQueueAudioPlayer();
    public static native void deleteSLBufferQueueAudioPlayer();

    public static native boolean createAudioRecorder();
    public static native void deleteAudioRecorder();
    public static native void startPlay();
    public static native void stopPlay();

    public static native int getChord();
}

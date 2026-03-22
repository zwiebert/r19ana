package bertw.xr25x53;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.view.Window;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.bmxgates.logger.BluetoothSerial;
import com.felhr.usbserial.UsbService;

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Set;

public class MainActivity extends AppCompatActivity {


    private TextView display;
    private TextView textViewHex;
    private EditText editText;

    private bertw.xr25x53.XR25Frame xr25;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Remove title bar
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        //Remove notification bar
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //set content view AFTER ABOVE sequence (to avoid crash)

        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        display = (TextView) findViewById(R.id.textView1);
        textViewHex = (TextView) findViewById(R.id.textViewHex);


        xr25 = new bertw.xr25x53.XR25Frame();
        try {
            xr25.test();
        } catch (Exception e) {
            System.err.append(e.toString());
        }

        usb_create();
        blu_onCreate();

    }


    @Override
    public void onResume() {
        super.onResume();

        try {
            textViewHex.setText(xr25.toString());
            display.setText(new X53b740Frame(xr25).getDataAsText());

        } catch (Exception e) {
            textViewHex.setText(e.toString());
        }
        usb_onResume();
        bluetoothSerial.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();

        usb_onPause();
        bluetoothSerial.onPause();
    }


    /////////////////////////////////// UsbService
    private UsbService usbService;
    private MyHandler mHandler;

    private void usb_onResume() {
        setFilters();  // Start listening notifications from UsbService
        startService(UsbService.class, usbConnection, null); // Start UsbService(if it was not started before) and Bind it
    }

    private void usb_onPause() {
        unregisterReceiver(mUsbReceiver);
        unbindService(usbConnection);
    }

    private void usb_create() {
        mHandler = new MyHandler(this);
    }

    private void startService(Class<?> service, ServiceConnection serviceConnection, Bundle extras) {
        if (!UsbService.SERVICE_CONNECTED) {
            Intent startService = new Intent(this, service);
            if (extras != null && !extras.isEmpty()) {
                Set<String> keys = extras.keySet();
                for (String key : keys) {
                    String extra = extras.getString(key);
                    startService.putExtra(key, extra);
                }
            }
            startService(startService);
        }
        Intent bindingIntent = new Intent(this, service);
        bindService(bindingIntent, serviceConnection, Context.BIND_AUTO_CREATE);
    }

    private void setFilters() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbService.ACTION_USB_PERMISSION_GRANTED);
        filter.addAction(UsbService.ACTION_NO_USB);
        filter.addAction(UsbService.ACTION_USB_DISCONNECTED);
        filter.addAction(UsbService.ACTION_USB_NOT_SUPPORTED);
        filter.addAction(UsbService.ACTION_USB_PERMISSION_NOT_GRANTED);
        registerReceiver(mUsbReceiver, filter);
    }


    private final ServiceConnection usbConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName arg0, IBinder arg1) {
            usbService = ((UsbService.UsbBinder) arg1).getService();
            usbService.setHandler(mHandler);
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            usbService = null;
        }
    };

    /*
     * Notifications from UsbService will be received here.
     */
    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            switch (intent.getAction()) {
                case UsbService.ACTION_USB_PERMISSION_GRANTED: // USB PERMISSION GRANTED
                    Toast.makeText(context, "USB Ready", Toast.LENGTH_SHORT).show();
                    break;
                case UsbService.ACTION_USB_PERMISSION_NOT_GRANTED: // USB PERMISSION NOT GRANTED
                    Toast.makeText(context, "USB Permission not granted", Toast.LENGTH_SHORT).show();
                    break;
                case UsbService.ACTION_NO_USB: // NO USB CONNECTED
                    Toast.makeText(context, "No USB connected", Toast.LENGTH_SHORT).show();
                    break;
                case UsbService.ACTION_USB_DISCONNECTED: // USB DISCONNECTED
                    Toast.makeText(context, "USB disconnected", Toast.LENGTH_SHORT).show();
                    break;
                case UsbService.ACTION_USB_NOT_SUPPORTED: // USB NOT SUPPORTED
                    Toast.makeText(context, "USB device not supported", Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    /*
       * This handler will be passed to UsbService. Data received from serial port is displayed through this handler
       */
    private static class MyHandler extends Handler {
        private final WeakReference<MainActivity> mActivity;

        public MyHandler(MainActivity activity) {
            mActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            MainActivity ma = mActivity.get();
            switch (msg.what) {
                case UsbService.MESSAGE_FROM_SERIAL_PORT:
                    byte[] data = (byte[]) msg.obj;
                    if (mActivity.get().xr25.append(data)) {

                        mActivity.get().display.setText(new X53b740Frame(ma.xr25).getDataAsText());
                    }

                    mActivity.get().textViewHex.setText(mActivity.get().xr25.toString());


                    break;
                case UsbService.CTS_CHANGE:
                    Toast.makeText(mActivity.get(), "CTS_CHANGE", Toast.LENGTH_LONG).show();
                    break;
                case UsbService.DSR_CHANGE:
                    Toast.makeText(mActivity.get(), "DSR_CHANGE", Toast.LENGTH_LONG).show();
                    break;
            }
        }
    }

    /////////////////////////////////// BluetoothSerial

    String deviceNamePrefix = "R19";
    BtHandler btHandler;
    BluetoothSerial bluetoothSerial;
    byte[] btBuf;
    int btBufSize;

    private void blu_onCreate() {
        btHandler = new BtHandler(this);
        //MessageHandler is call when bytes are read from the serial input
        bluetoothSerial = new BluetoothSerial(this, new BluetoothSerial.MessageHandler() {
            @Override
            public int read(int bufferSize, byte[] buffer) {
                byte[] buf = Arrays.copyOf(buffer, bufferSize);
                btHandler.obtainMessage(0, buf).sendToTarget();

                return bufferSize;
            }
        }, deviceNamePrefix);

        blu_registerReceivers();
    }

    BroadcastReceiver bluetoothConnectReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
        }
    };
    BroadcastReceiver bluetoothDisconnectReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

        }
    };

    private void blu_registerReceivers() {

        //Fired when connection is established and also fired when onResume is called if a connection is already established.
        LocalBroadcastManager.getInstance(this).registerReceiver(bluetoothConnectReceiver, new IntentFilter(BluetoothSerial.BLUETOOTH_CONNECTED));
        //Fired when the connection is lost
        LocalBroadcastManager.getInstance(this).registerReceiver(bluetoothDisconnectReceiver, new IntentFilter(BluetoothSerial.BLUETOOTH_DISCONNECTED));
        //Fired when connection can not be established, after 30 attempts.
        LocalBroadcastManager.getInstance(this).registerReceiver(bluetoothDisconnectReceiver, new IntentFilter(BluetoothSerial.BLUETOOTH_FAILED));

    }


    private static class BtHandler extends Handler {
        private final WeakReference<MainActivity> mActivity;

        public BtHandler(MainActivity activity) {
            mActivity = new WeakReference<>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            MainActivity ma = mActivity.get();

            switch (msg.what) {
                case 0:
                    try {
                        if (ma.xr25.append((byte[]) msg.obj)) {
                            ma.display.setText(new X53b740Frame(ma.xr25).getDataAsText());
                            ma.textViewHex.setText(ma.xr25.toString());
                        }
                    } catch (Exception e) {
                        ma.textViewHex.setText(e.toString());
                    }

                    break;
            }
        }
    }

}
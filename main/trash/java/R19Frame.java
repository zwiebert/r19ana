package bertw.xr25x53;

/**
 * Created by bertw on 31.08.2017.
 */

public class R19Frame {
    public int FrameNumber, EngineSpeed_RPM, MAP_mBar, IAT_Celsius, ECT_Celsius, O2_Sensor_mV, AP_mBar;
    public float BatteryVoltage_V, ID_msec;
    public boolean isThrottleOpen, isThrottleClosed, isAGR_AKF;
    public int IdleSpeedCorr, EngineKnocking;

    public R19Frame(XR25Frame data) {
        FrameNumber = data.getFrameCounter();
        {
            int idx_add = (data.getByteByIndex(6) == 0xff ? 0 : -2) + 1 ;  // index
            {
                long num = (data.getIntByIndex(11 + idx_add) | (data.getIntByIndex(12 + idx_add) << 8)) & 0xffff;
                EngineSpeed_RPM = (num == 0) ? 0 : (int) (30000000L / num);
            }
            MAP_mBar = (int) (data.getIntByIndex(6 + idx_add) * 3.697f + 103.0f); // old: 3.7, new: 3.697
            IAT_Celsius = (int) (data.getIntByIndex(8 + idx_add) * 0.625f - 40.0f);
            ECT_Celsius = (int) (data.getIntByIndex(7 + idx_add) * 0.625f - 40.0f);
            O2_Sensor_mV = (int) (data.getIntByIndex(10 + idx_add) * 4);
            // AP_mBar = (int) (data.getIntByIndex(21 + idx_add) * -3.7f + 1043.0f); //old
            AP_mBar = 1090 - data.getIntByIndex(21 + idx_add); //new
            BatteryVoltage_V = data.getIntByIndex(9 + idx_add) * 0.0312f + 8.0f;
            {
                long num = (data.getIntByIndex(13 + idx_add) | (data.getIntByIndex(14 + idx_add) << 8)) & 0xffff;
                ID_msec = (num == 0) ? 0 : (num * 0.002f) - 0.5f;
            }
            isThrottleOpen = (data.getByteByIndex(5 + -1) & 0x10) == 0;
            isThrottleClosed = (data.getByteByIndex(5 + -1) & 0x08) == 0;

            EngineKnocking = (int) data.getIntByIndex(13 + idx_add); // new
            IdleSpeedCorr = (int) data.getIntByIndex(14 + idx_add); // new
        }
        {
            int idx_add = (EngineSpeed_RPM == 0 ? -1 : -3);

            isAGR_AKF = ((data.getByteByIndex(20 + idx_add)) & 0x20) != 0;
        }
    }

    public String getDataAsText() {
        String result = "";

        if (EngineSpeed_RPM == 0) {
            result += String.format("Motor aus. Frames: %d\n\n\n", FrameNumber);
        }



        result += String.format("DK-Leerlauf: %s\nDK-Vollgas: %s\nAKF/AGR: %s\n",
                (isThrottleClosed ? "Y" : "N"),
                (isThrottleOpen ? "Y" : "N"),
                (isAGR_AKF ? "Y" : "N")

        );

        result += String.format("MAP: %4d mBar\nKühlwasser: %3d °C\nAnsauggemisch: %3d °C\n",
                MAP_mBar, ECT_Celsius, IAT_Celsius
        );
        int o2 = O2_Sensor_mV;
        result += String.format("Batterie: %2.1f V\nLambda: %3d mV (%s)\nDrehzahl: %4d rpm\nAußendruck: %4d mBar\n",
                BatteryVoltage_V, o2, (o2 < 100 ? "mager" : (o2 > 600 ? "fett" : "1")), EngineSpeed_RPM, AP_mBar
        );
        result += String.format("Spritzdauer: %.2f ms\n",
                ID_msec
        );


        result += String.format("Motorklopfen: %3d\nLeerlaufsteller: %3d\n",
                EngineKnocking, IdleSpeedCorr
        );

        return result;
    }
}

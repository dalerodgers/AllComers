//
// Vibrate.java
//
package org.qtproject.example.Chronometer;

import android.content.Context;
import android.os.VibrationEffect;
import android.os.Vibrator;

public class Vibrate extends org.qtproject.qt.android.bindings.QtActivity
{
    public static Vibrator m_vibrator;
    public static Vibrate m_istance;

    public Vibrate()
    {
        m_istance = this;
    }

    public static void start(int x)
    {
        if (m_vibrator == null)
        {
            if (m_istance != null)
            {
                m_vibrator = (Vibrator) m_istance.getSystemService(Context.VIBRATOR_SERVICE);
                m_vibrator.vibrate(VibrationEffect.createOneShot(x, VibrationEffect.DEFAULT_AMPLITUDE));
            }
        }
        else
        {
            m_vibrator.vibrate(VibrationEffect.createOneShot(x, VibrationEffect.DEFAULT_AMPLITUDE));
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console_Demo
{
    class Calculator
    {
        const byte WHEEL_TICKS_INDEX = 2;
        const byte WHEEL_PERIOD_LSB_INDEX = 4;
        const byte WHEEL_PERIOD_MSB_INDEX = 5;
        const byte INSTANT_POWER_LSB_INDEX = 6;
        const byte INSTANT_POWER_MSB_INDEX = 7;

        static readonly float m_wheel_size_m = 2.07f;

        byte m_last_wheel_ticks = 0;
        ushort m_last_wheel_period = 0;

        public ushort GetInstantPower(byte[] payload)
        {
            // Combine two bytes to make the watts.
            ushort watts = (ushort)(payload[INSTANT_POWER_LSB_INDEX] |
                payload[INSTANT_POWER_MSB_INDEX] << 8);

            return watts;
        }

        // This works on the Power @ Wheel Torque page.
        public float GetSpeed(byte[] payload)
        {
            byte wheel_ticks = payload[WHEEL_TICKS_INDEX];
            ushort wheel_period = (ushort)(payload[WHEEL_PERIOD_LSB_INDEX] |
                payload[WHEEL_PERIOD_MSB_INDEX] << 8);

            if (wheel_period == 0 || wheel_ticks == 0)
                return 0.0f;

            byte wheel_ticks_delta = 0;
            if (wheel_ticks < m_last_wheel_ticks)
                // If we had a rollover.
                wheel_ticks_delta = (byte)((255 - m_last_wheel_ticks) + wheel_ticks);
            else
                wheel_ticks_delta = (byte)(wheel_ticks - m_last_wheel_ticks);

            ushort wheel_period_delta = 0;
            if (wheel_period < m_last_wheel_period)
                // If we had a rollover.
                wheel_period_delta = (ushort)((65535 - m_last_wheel_period) + wheel_period);
            else
                wheel_period_delta = (ushort)(wheel_period - m_last_wheel_period);

            if (wheel_ticks_delta == 0 || wheel_period_delta == 0)
                return 0.0f;

            // Calculate speed
            float speed = (wheel_ticks_delta * m_wheel_size_m) / (wheel_period_delta / 2048f);

            m_last_wheel_ticks = wheel_ticks;
            m_last_wheel_period = wheel_period;

            return speed;
        }

        // This works on Page0 of the BikeSpeed/BikeSpeed & Cadence message.
        public float GetBikeSpeed(BikeSpeedEvent[] events)
        {
            if (events == null)
                return 0.0f;

            // First array element[0] contains the oldest event.
            // Last array element[count-1] contains the most recent event.
            int count = events.Length;
            if (count == 0)
                return 0.0f;

            BikeSpeedEvent previousEvent = events[0];
            BikeSpeedEvent latestEvent = events[count - 1];

            // Revs
            ushort cumulative_revs_delta = 0;
            if (latestEvent.CumulativeWheelRevs < previousEvent.CumulativeWheelRevs)
                // If we had a rollover.
                cumulative_revs_delta = (ushort)((previousEvent.CumulativeWheelRevs ^ 0xFFFF) +
                    latestEvent.CumulativeWheelRevs);
            else
                cumulative_revs_delta = (ushort)(latestEvent.CumulativeWheelRevs - 
                    previousEvent.CumulativeWheelRevs);

            // Event time
            ushort event_time_delta = 0;
            if (latestEvent.EventTime < previousEvent.EventTime)
                // If we had a rollover.
                event_time_delta = (ushort)((previousEvent.EventTime ^ 0xFFFF) + latestEvent.EventTime);
            else
                event_time_delta = (ushort)(latestEvent.EventTime - previousEvent.EventTime);

            if (event_time_delta == 0 || cumulative_revs_delta == 0)
                return 0.0f;

            // Calculate speed
            float speed = (cumulative_revs_delta * m_wheel_size_m) / (event_time_delta / 1024f);

            return speed;
        }

    }
}

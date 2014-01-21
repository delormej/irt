using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console_Demo
{
    class Calculator
    {
        const byte INSTANT_POWER_LSB_INDEX = 6;
        const byte INSTANT_POWER_MSB_INDEX = 7;

        static readonly float m_wheel_size_m = 2.07f;
        
        public ushort GetInstantPower(byte[] payload)
        {
            // Combine two bytes to make the watts.
            ushort watts = (ushort)(payload[INSTANT_POWER_LSB_INDEX] |
                payload[INSTANT_POWER_MSB_INDEX] << 8);

            return watts;
        }

        // This works on the Power @ Wheel Torque page.
        public float GetSpeed(SpeedEvent[] events)
        {
            if (events == null)
                return 0.0f;

            // First array element[0] contains the oldest event.
            // Last array element[count-1] contains the most recent event.
            int count = events.Length;
            if (count == 0)
                return 0.0f;

            SpeedEvent previousEvent = events[0];
            SpeedEvent latestEvent = events[count - 1];
            
            // Wheel ticks (revolutions)
            byte wheel_ticks_delta = 0;
            if (latestEvent.CumulativeWheelRevs < previousEvent.CumulativeWheelRevs)
                // If we had a rollover.
                wheel_ticks_delta = (byte)((previousEvent.CumulativeWheelRevs ^ 0xFF) +
                    latestEvent.CumulativeWheelRevs);
            else
                wheel_ticks_delta = (byte)(latestEvent.CumulativeWheelRevs -
                    previousEvent.CumulativeWheelRevs);

            // Wheel period / event time.
            ushort wheel_period_delta = 0;
            if (latestEvent.EventTime < previousEvent.EventTime)
                // If we had a rollover.
                wheel_period_delta = (ushort)((previousEvent.EventTime ^ 0xFFFF) +
                    latestEvent.EventTime);
            else
                wheel_period_delta = (ushort)(latestEvent.EventTime - previousEvent.EventTime);

            if (wheel_ticks_delta == 0 || wheel_period_delta == 0)
                return 0.0f;

            // Calculate speed
            float speed = (wheel_ticks_delta * m_wheel_size_m) / (wheel_period_delta / 2048f);  

            return speed;
        }

        // This works on Page0 of the BikeSpeed/BikeSpeed & Cadence message.
        public float GetBikeSpeed(SpeedEvent[] events)
        {
            if (events == null)
                return 0.0f;

            // First array element[0] contains the oldest event.
            // Last array element[count-1] contains the most recent event.
            int count = events.Length;
            if (count == 0)
                return 0.0f;

            SpeedEvent previousEvent = events[0];
            SpeedEvent latestEvent = events[count - 1];

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

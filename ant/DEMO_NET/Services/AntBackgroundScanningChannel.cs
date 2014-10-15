using ANT_Managed_Library;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ANT_Console.Services
{
    public class AntBackgroundScanningChannel : AntService
    {
        protected virtual void Configure()
        {
            lock (this)
            {
                if (m_device == null)
                    m_device = GetDevice();
            }

            m_channel = m_device.getChannel(0);
            m_channel.channelResponse += ProcessResponse;

            m_channel.setChannelID(0, false, 0, 0);

            m_channel.assignChannelExt(
                ANT_ReferenceLibrary.ChannelType.ADV_TxRx_Only_or_RxAlwaysWildCard_0x40,
                0,
                ANT_ReferenceLibrary.ChannelTypeExtended.ADV_AlwaysSearch_0x01);

            // Infinite low priority search.
            m_channel.setLowPrioritySearchTimeout(0xFF);

            // Disable high priority search.
            m_channel.setChannelSearchTimeout(0);

            m_channel.openChannel();
        }

        protected override void ProcessResponse(ANT_Response response)
        {
            // New master available that we can connect to?
            if (response.isExtended())
            {
                ANT_ChannelID newChannelId = response.getDeviceIDfromExt();
                // Open a new channel with newChannelId.deviceNumber
                // This should tell us if it's a power meter, speed sensor, etc...
                //      newChannelId.transmissionTypeID
            }
            // otherwise, ignore
        }
    }
}

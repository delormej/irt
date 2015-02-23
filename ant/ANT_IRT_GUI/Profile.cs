using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace IRT_GUI
{
    public class FirmwareRev
    {
        public byte Major;
        public byte Minor;
        public byte Build;

        public FirmwareRev(byte SoftwareRevision, byte SupplementalSoftwareRevision)
        {
            //arg1.SoftwareRevision, arg1.SupplementalSoftwareRevision
            Major = (byte)(SoftwareRevision >> 4);
            Minor = (byte)(SoftwareRevision & 0x0F);
            Build = SupplementalSoftwareRevision;
        }

        public override string ToString()
        {
            // ant_product_page[2] = SW_REVISION_BLD;
            // ant_product_page[3] = SW_REVISION_MAJ | SW_REVISION_MIN;

            // Major reviewing is the upper nibble, Minor is the lower nibble.

            return string.Format("{0}.{1}.{2}", Major, Minor, Build);
        }
    }

    class Profile
    {

    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace IRT.Calibration
{
    public enum Motion
    {
        Undetermined,
        Accelerating,
        Stable,
        Decelerating
    }

    public class Model : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public double Watts
        {
            get { return 0; }
            set {  }
        }

        public double SpeedMps
        {
            get { return 0; }
            set {  }
        }

        public double Seconds
        {
            get { return 0; }
            set { }
        }

        public Stage Stage;

        public Motion Motion { get; set; }
    }
}

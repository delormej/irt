using System;
using System.Collections.Generic;
using ANT_Console.Services;
using ANT_Console.Messages;
using IntervalParser;
using System.Runtime.InteropServices;

namespace ANT_Console
{
    class InteractiveConsole : IReporter
    {
        bool m_scriptInfinite = false;
        bool m_inCommand = false;
        SubPages m_lastSubPage;                     // Stores last subpage requested - used for setting parameters.
        ScriptHandler m_script;
        DateTime m_lastReport = DateTime.Now;
        AntBikePower m_eMotion;
        AntBikeSpeed m_refSpeed;
        AntControl m_control;
        SpeedSimulator m_speedSim;

        public static void Main()
        {
            Console.Title = "IRT Debug Console";
            InteractiveConsole console = new InteractiveConsole();
            console.Run();
        }

        public void Run()
        {
            Controller controller = null;

            try
            {
                controller = new Controller();

                // Check to see if we should connect to a specific E-Motion Device.
                Console.Write("E-Motion Rollers Device ID or <ENTER>:");
                
                controller.ConfigureServices(ushort.Parse(Console.ReadLine()));

                m_eMotion = controller.EMotionBikePower;
                m_control = controller.AntRemoteControl;
                m_refSpeed = controller.RefBikeSpeed;

                // configure ourselves as a reporter and start.
                controller.Reporters.Add(this);
                controller.ConfigureReporters();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                return;
            }

            PreventShutdown();

            const string header = "Time         |  mph  | Watts | Watts2| Servo  | Target | Flywheel";
            ConsoleKeyInfo cki;

            Console.CursorVisible = false;
            Console.WriteLine("Starting....");
            Console.WriteLine(header);

            do
            {
                cki = Console.ReadKey(true);

                switch (cki.Key)
                {
                    case ConsoleKey.U:
                        m_control.RemoteControl(0x00);
                        WriteCommand("Sent UP command.");
                        break;

                    case ConsoleKey.D:
                        m_control.RemoteControl(0x01);
                        WriteCommand("Sent DOWN command.");
                        break;

                    case ConsoleKey.S:
                        m_control.RemoteControl(0x02);
                        WriteCommand("Sent SELECT command.");
                        break;

                    case ConsoleKey.W:
                        SetWeightCommand();
                        break;

                    case ConsoleKey.M:
                        MoveServoCommand();
                        break;

                    case ConsoleKey.H:
                        ShowHelp();
                        break;

                    case ConsoleKey.F:
                        m_eMotion.SetFirmwareUpdateMode();
                        WriteCommand("Enabling DEVICE FIRMWARE UPDATE MODE...");
                        break;

                    case ConsoleKey.X:
                        WriteCommand("Exiting...");
                        break;

                    case ConsoleKey.V:
                        WriteCommand(string.Format("Firmware version: {0}", 
                            m_eMotion.FirmwareVersion));
                        break;

                    case ConsoleKey.P:
                        ParseIntervalFileCommand();
                        break;

                    case ConsoleKey.T:
                        ExecuteScriptFileCommand();
                        break;

                    case ConsoleKey.L:
                        m_scriptInfinite = !m_scriptInfinite;
                        WriteCommand("Toggled script infinite loop setting to: " + m_scriptInfinite);
                        break;

                    case ConsoleKey.Z:
                        SetWheelSizeCommand();
                        break;

                    case ConsoleKey.G:
                        SetParameterCommand();
                        break;

                    case ConsoleKey.R:
                        GetParameterCommand();
                        break;

                    case ConsoleKey.D1:
                        SimulateSpeedCommand();
                        break;

                    default:
                        WriteCommand("Unrecognized command.");
                        ShowHelp();
                        break;
                }
            } while (cki.Key != ConsoleKey.X);

            // Signal to wrap up and report out summary.
            controller.Shutdown();
        }

        public void Report(string message)
        {
            Console.WriteLine(message);
        }

        public void Report(DataPoint data)
        {
            if (m_inCommand)
                return;

            if (data.Timestamp <= m_lastReport)
                return;

            const string format = "{0:H:mm:ss.fff} | {1,5:N1} | {2,5:N0} | {3,5:N0} | {4,6:N0} | {5,4}:{6} | {7}";

            // Leave 2 rows at the bottom for command.
            int lastLine = Console.CursorTop;

            // Do we need to scroll?
            if (lastLine > Console.WindowHeight - 2)
            {
                // Buffer space to scroll? 
                if ((Console.WindowTop + Console.WindowHeight) >= Console.BufferHeight - 1)
                {
                    // We're out of buffer space, so flush.
                    Console.MoveBufferArea(Console.WindowLeft,
                        Console.WindowTop,
                        Console.WindowWidth,
                        Console.WindowHeight,
                        Console.WindowLeft,
                        0);

                    Console.SetWindowPosition(Console.WindowLeft, 0);
                    lastLine = Console.WindowHeight - 1;
                }
                else
                {
                    // Scroll
                    Console.SetWindowPosition(Console.WindowLeft, Console.WindowTop + 1);
                }
            }

            // Record last reporting.
            m_lastReport = data.Timestamp;

            // Position the cursor at the bottom of the screen to write the command line.
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            Console.Write("<enter cmd>");
            Console.SetCursorPosition(Console.WindowLeft, lastLine);
            Console.WriteLine(format,
                data.Timestamp,
                //data.SpeedReference,
                data.SpeedEMotion,
                data.PowerEMotion,
                data.PowerReference,
                data.ServoPosition,
                data.ResistanceMode == 0x41 ? "S" : data.ResistanceMode == 0x42 ? "E" : "",
                data.TargetLevel,
                data.FlywheelRevs);
        }

        void ShowHelp()
        {
            m_inCommand = true;

            ConsoleColor color = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;

            Console.WriteLine("{KEY} [Command]\n" +
                "H [Help]\n" +
                "W [Set Weight]\n" +
                "U [Send Up Command]\n" +
                "D [Send Down Command]\n" +
                "S [Send Select Command]\n" +
                "M [Move Servo to position X]\n" +
                "Z [Set wheel size MM]\n" +
                "F [Enable Device Firmware Update Mode]\n" +
                "V [Display Firmware Version]\n" +
                "P [Parse Interval file in format {mins},{watts},{text}]\n" +
                "T [Execute script file in format {seconds},{servo_position}]\n" +
                "G [Set Parameter]\n" +
                "R [Request Parameter]\n" +
                "X [Exit]");

            Console.ForegroundColor = color;

            m_inCommand = false;
        }

        bool InteractiveCommand(string prompt, Func<bool> Parser)
        {
            m_inCommand = true;

            // Propmt user for weight in lbs
            Console.SetCursorPosition(Console.WindowLeft, Console.WindowTop + Console.WindowHeight - 1);
            ConsoleColor lastColor = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Write(prompt);
            Console.ForegroundColor = lastColor;
            Console.SetCursorPosition(prompt.Length + 2, Console.CursorTop);
            Console.CursorVisible = true;

            bool result = Parser();

            Console.CursorVisible = false;
            m_inCommand = false;

            return result;
        }

        void MoveServoCommand()
        {
            string prompt = "<enter servo position:>";
            int position = 0;
            bool success = InteractiveCommand(prompt, () =>
            {
                if (int.TryParse(Console.ReadLine(), out position))
                {
                    if (position < 699 || position > 2107)
                    {
                        WriteCommand("Servo position should be > 699 and < 2107.");
                        return false;
                    }
                }
                else
                {
                    WriteCommand("Invalid servo position.");
                    return false;
                }

                return true;
            });

            if (success)
            {
                m_eMotion.MoveServo(position);
                WriteCommand(string.Format("Moving servo to {0}.", position));
            }
        }

        void SetWeightCommand()
        {
            string prompt = "<enter weight in lbs:>";
            float weight = 0.0f;
            bool success = InteractiveCommand(prompt, () =>
            {
                //float weight = float.NaN;

                if (float.TryParse(Console.ReadLine(), out weight))
                {
                    if (weight < 100 || weight > 300)
                    {
                        WriteCommand("Weight in lbs should be > 100 and < 300.");
                        return false;
                    }
                }
                else
                {
                    WriteCommand("Invalid weight value.");
                    return false;
                }

                return true;
            });

            if (success)
            {
                // Convert lb to kg.
                float weightKg = weight / 2.2f;

                m_eMotion.SetWeight(weightKg);
                WriteCommand(string.Format("Set Weight to {0:N1}kg.", weightKg));
            }
        }

        void SetWheelSizeCommand()
        {
            string prompt = "<enter wheel size in MM:>";
            int wheelSizeMM = 0;
            bool success = InteractiveCommand(prompt, () =>
            {
                //float weight = float.NaN;

                if (int.TryParse(Console.ReadLine(), out wheelSizeMM))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });

            if (success)
            {
                
                m_eMotion.SetWheelSize(wheelSizeMM);
                m_refSpeed.WheelSizeMM = (short)wheelSizeMM;
                WriteCommand(string.Format("Set wheel size to {0} mm.", wheelSizeMM));
            }
        }

        void SetParameterCommand()
        {
            string prompt = "<enter parameter value to set {0}>";
            uint value = 0;
            bool success = InteractiveCommand(string.Format(prompt, m_lastSubPage), () =>
            {
                //float weight = float.NaN;
                string input = Console.ReadLine();

                if (input.StartsWith("-"))
                {
                    // Special handling for signed int16 values.
                    short val = 0;
                    
                    if (short.TryParse(input, out val))
                    {
                        value = (uint)val;
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (uint.TryParse(input, out value))
                {
                    return true;
                }
                else
                {
                    return false;
                }
            });

            if (success)
            {
                try
                {
                    m_eMotion.SetParameter(m_lastSubPage, value);
                    WriteCommand("Set parameter.");
                }
                catch (Exception e)
                {
                    WriteCommand(e.Message);
                }
            }
        }

        void GetParameterCommand()
        {
            string prompt = "<enter subpage (parameter) number>";
            bool success = InteractiveCommand(prompt, () =>
            {
                try
                {
                    m_lastSubPage = (SubPages)Enum.Parse(typeof(SubPages), Console.ReadLine(), true);
                    return true;
                }
                catch 
                {
                    WriteCommand("Unrecognized setting.");
                    return false;
                }
            });

            if (success)
            {
                try
                {
                    m_eMotion.RequestDeviceParameter(m_lastSubPage);
                    WriteCommand("Requested " + m_lastSubPage.ToString());
                }
                catch (Exception e)
                {
                    WriteCommand(e.Message);
                }
            }
        }

        void SimulateSpeedCommand()
        {
            string prompt = "<enter speed in miles per hour:>";
            float speed_mph = 0.0f;
            bool success = InteractiveCommand(prompt, () =>
            {
                try
                {
                    if (float.TryParse(Console.ReadLine(), out speed_mph))
                    {
                        if (speed_mph < 5.0f || speed_mph > 40.0f)
                        {
                            WriteCommand("Speed must be between 5.0 and 40.0 mph");
                            return false;
                        }
                    }

                    return true;
                }
                catch 
                {
                    WriteCommand("Unrecognized setting.");
                    return false;
                }
            });

            if (success)
            {
                try
                {
                    if (m_speedSim == null)
                    {
                        m_speedSim = new SpeedSimulator();
                    }

                    float speed_mps = speed_mph * 0.44704f; 

                    m_speedSim.Simulate(speed_mps);
                    /*WriteCommand(string.Format("Simulating {0:d} mps / {1:d} mph",
                        speed_mps, speed_mph));*/
                }
                catch (Exception e)
                {
                    WriteCommand(e.Message);
                }
            }
            else
            {
                WriteCommand("Invalid speed.");
            }
        }

        void ParseIntervalFileCommand()
        {
            string prompt = "<enter interval source filename {defaults to Source.txt}>";
            string filename = string.Empty;

            InteractiveCommand(prompt, () =>
            {
                filename = Console.ReadLine();
                return true;
            });

            IntervalParser.Parser.Parse(filename);
            WriteCommand("Parsed interval file.");
        }

        void ExecuteScriptFileCommand()
        {
            if (m_script !=null)
            {
                m_script.TogglePause();
                WriteCommand("Toggled script pause.");
                return;
            }

            string prompt = "<enter script source filename>";
            string filename = string.Empty;

            InteractiveCommand(prompt, () =>
            {
                filename = Console.ReadLine();
                return true;
            });

            m_script = new ScriptHandler();
            m_script.SetServo += OnScriptSetServo;
            m_script.ScriptComplete += (o, e) =>
            {
                //
                WriteCommand("Script complete.");

                if (m_scriptInfinite)
                {
                    // Restart.
                    m_script.Start();
                }
                else
                {
                    m_script = null;
                }
            };

            ParseScriptInput(filename);
            m_script.Start();         
        }

        void OnScriptSetServo(int position)
        {
            m_eMotion.MoveServo(position);
            if (!m_inCommand)
                WriteCommand(string.Format("<script> Moving servo to {0}.", position));
        }

        void ParseScriptInput(string filename)
        {
            try
            {
                using (System.IO.StreamReader reader = new System.IO.StreamReader(filename))
                {
                    m_script.ParseInput(reader);
                    WriteCommand("Script now executing.");
                }
            }
            catch (Exception e)
            {
                WriteCommand("ERROR parsing script: " + e.Message);
            }
        }

        void WriteCommand(string message)
        {
            m_inCommand = true;

            ConsoleColor last = Console.ForegroundColor;
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine(message);
            Console.ForegroundColor = last;

            m_inCommand = false;
        }

        public void Report2(DataPoint data)
        {
            if (data.Timestamp > m_lastReport)
            {
                Console.WriteLine(data);
                m_lastReport = data.Timestamp;
            }
        }

        private void PreventShutdown()
        {
            Utility.SetThreadExecutionState(
                Utility.ThreadExecutionState.CONTINUOUS | 
                Utility.ThreadExecutionState.DISPLAY_REQUIRED |
                Utility.ThreadExecutionState.SYSTEM_REQUIRED);
        }
    }

    public static class Utility
    {
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern ThreadExecutionState SetThreadExecutionState(ThreadExecutionState esFlags);
        [FlagsAttribute]
        public enum ThreadExecutionState : uint
        {
            CONTINUOUS = 0x80000000,
            DISPLAY_REQUIRED = 0x00000002,
            SYSTEM_REQUIRED = 0x00000001
        }
    }
}

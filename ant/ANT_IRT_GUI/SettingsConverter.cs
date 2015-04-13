using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace IRT_GUI
{
    public class SettingsConverter 
    {

        void UpdateSettings(ushort settings)
        {
            ExecuteOnUI(() =>
            {
                for (int i = 0; i < chkLstSettings.Items.Count; i++)
                {
                    Settings setting = GetSetting(i);

                    bool check = ((settings & (ushort)setting) == (ushort)setting);
                    chkLstSettings.SetItemChecked(i, check);
                }
            });
        }

        private Settings GetSetting(int index)
        {
            // Get the setting
            Settings setting;
            Enum.TryParse<Settings>(chkLstSettings.Items[index].ToString(), out setting);
            return setting;
        }

    }
}

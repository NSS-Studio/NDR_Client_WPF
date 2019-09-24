using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace wpf_ndr
{
    class RasDial: IDisposable
    {
        public RasDial(String adapterName)
        {
            CreateRasDialInstance(adapterName);
        }

        [DllImport("RasDial.dll", EntryPoint = "create_ras_dial_instance", CharSet = CharSet.Unicode)]
        private static extern bool CreateRasDialInstance(String adapterName);

        [DllImport("RasDial.dll", EntryPoint = "destroy_ras_dial_instance", CharSet = CharSet.Unicode)]
        private static extern bool DestroyRasDialInstance();
        
        [DllImport("RasDial.dll", EntryPoint = "ras_dial_connection_state", CharSet = CharSet.Unicode)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool RasDialConnectionState();
        [DllImport("RasDial.dll", EntryPoint = "ras_dial", CharSet = CharSet.Unicode)]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool Dial(String username, String password);

        [DllImport("RasDial.dll", EntryPoint = "ras_hang_up", CharSet = CharSet.Unicode)]
        private static extern void RasHangUp();
        [DllImport("RasDial.dll", EntryPoint = "ras_get_error_msg", CharSet = CharSet.Unicode)]
        private static extern IntPtr GetErrorMessage();

        public bool IsDisconnected()
        {
            return RasDialConnectionState();
        }

        public bool RDial(String username, String password)
        {
            return RasDial.Dial(username, password);
        }
        public void Dispose()
        {
            DestroyRasDialInstance();
        }
        public String GetErrorMsg()
        {
            return Marshal.PtrToStringUni(GetErrorMessage());
        }
    }
}

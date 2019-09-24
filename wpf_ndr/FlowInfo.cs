using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace wpf_ndr
{
    class FlowInfo
    {
        public static long GetFlowInfo()
        {
            IPGlobalProperties computerProperties = IPGlobalProperties.GetIPGlobalProperties();
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface adapter in nics)
            {
                IPv4InterfaceStatistics ipv4Statistics = adapter.GetIPv4Statistics();
                if (adapter.Name == Utils.adapterName)
                {
                    return ipv4Statistics.BytesSent / 1024 + ipv4Statistics.BytesReceived / 1024; // return kb
                }
            }
            return 0;
        }
    }
}

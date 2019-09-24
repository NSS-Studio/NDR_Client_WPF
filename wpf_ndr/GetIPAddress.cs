using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

namespace wpf_ndr
{
    class GetIPAddress
    {
        private string adapterName = "";
        public GetIPAddress(string adapterName)
        {
            this.adapterName = adapterName;
        }

        public string GetIPv4AddressByAdapterName()
        {
            NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface adapter in adapters)
            {
                if (adapter.Name == adapterName)
                {
                    UnicastIPAddressInformationCollection unicastIPAddressInformation = adapter.GetIPProperties().UnicastAddresses;
                    foreach (var item in unicastIPAddressInformation)
                    {
                        if (item.Address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
                        {
                            return item.Address.ToString();
                        }
                    }
                }
            }
            return string.Empty;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace wpf_ndr
{
    class ResouceManager
    {
        private static ResouceManager instance = new ResouceManager();

        public static ResouceManager GetInstance()
        {
            return instance;
        }

        public RasDial GetRasDial()
        {
            return _rasDialInstance;
        }

        // Save All Resource
        // rasdial Instance is only one and protect thread-safe;
        private RasDial _rasDialInstance = new RasDial(Utils.adapterName);

        private ResouceManager()
        {

        }
    }
}
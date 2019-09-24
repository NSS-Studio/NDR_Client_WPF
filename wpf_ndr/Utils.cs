using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using log4net;

namespace wpf_ndr
{
    class Utils
    {
        public static ArrayList do_main = new ArrayList();
        public static SqlLiteHelper database = null;
        public static String home_dir = null;
        public static String adapterName = "NClient"; //NClient
        public static readonly ILog log = LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);
        static Utils()
        {
            do_main.Add("@cnc50a");
            do_main.Add("@cnc30a");
            do_main.Add("@cnc");
            do_main.Add("@ct10a");   
            do_main.Add("@ct10");
            do_main.Add("@edu");
            do_main.Add("");
            home_dir = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile) + "/.nssc/";
            database = new SqlLiteHelper();
        }
    }
}

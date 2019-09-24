using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ControlLibrary
{
    /// <summary>
    /// Interaction logic for MainWin.xaml
    /// </summary>

    public partial class MainWin : UserControl
    {
        private Grid[] items = new Grid[3];
        private int successRate = 40;
        private string username;
        private string ip_addr;

        public int SuccessRate
        {
            get
            {
                return successRate;
            }
            set
            {
                if (value != successRate)
                {
                    successRate = value;
                }
            }
        }

        public MainWin()
        {
            InitializeComponent();
            items[0] = this.mainPanel;
            items[1] = this.optionPanel;
            items[2] = this.aboutPanel;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://e.neusoft.edu.cn/www/");
        }

        DoubleAnimation animation(int from, int to)
        {
            DoubleAnimation opacityAnimation = new DoubleAnimation();
            opacityAnimation.Duration = new Duration(TimeSpan.FromSeconds(0.3));
            opacityAnimation.From = from;
            opacityAnimation.To = to;
            return opacityAnimation;
        }

       private void animatioStart(string type)
       {
            foreach (Grid item in items)
            {
                if (item.Opacity != 0 && item.Opacity != 1) return;
            }
            foreach (Grid item in items)
            {
                int[] during = {0, 1};
                if (type != item.Name)
                {
                    item.IsEnabled = false;
                    during[0] = 1; during[1] = 0;
                }
                else
                {
                    item.IsEnabled = true;
                }
                if (item.Opacity == during[0])
                {
                    item.Opacity = during[1];
                    item.BeginAnimation(Grid.OpacityProperty, animation(during[0], during[1]));
                }
            }
       }

        private void ListBoxItem_Selected_Main(object sender, RoutedEventArgs e)
        {
            animatioStart("mainPanel");
        }

        private void ListBoxItem_Selected_Option(object sender, RoutedEventArgs e)
        {
            animatioStart("optionPanel");
        }

        private void ListBoxItem_Selected_About(object sender, RoutedEventArgs e)
        {
            animatioStart("aboutPanel");
        }

        public void setUserName(string username)
        {
            Char[] un = username.ToCharArray();
            string name = "";
            for(int i = 0;i < un.Length;i++)
            {
                Char c;
                c = un[i];
                if (i == 0) c = Char.ToUpper(un[i]);
                name += c;
            }
            this.label_username.Content = "Hi, " + name;
        }
        public void setNetType(string nettype)
        {
            this.label_nettype.Content = nettype;
        }
        public void setNetIP(string ip)
        {
            this.label_netIP.Content = "登录IP:" + ip;
        }
        public void setNetTime(long sec)
        {
            string res = "";
            int hour = (int)sec / 3600;
            int min = (int)sec % 3600 / 60;
            int second = (int)sec % 60;
            res += (hour < 10 ? "0" + hour.ToString() : hour.ToString());
            res += ":";
            res += min < 10 ? "0" + min.ToString() : min.ToString();
            res += ":";
            res += second < 10 ? "0" + second.ToString() : second.ToString();
            this.label_nettime.Content = "上网时长: " + res;
        }
        public void setNetFlow(long flow)
        {
            if (flow < 1000)
            {
                this.label_netflow.Content = "上网流量: " + flow.ToString() + "KB";
            }else if(flow < 1000000)
            {
                this.label_netflow.Content = "上网流量: " + (flow / 1000.0).ToString() + "MB";
            }
            else
            {
                this.label_netflow.Content = "上网流量: " + (flow / 1000000.0).ToString() + "GB";
            }
        }

        private void Logout_MouseEnter(object sender, MouseEventArgs e)
        {
            //this.img_logout.Source = "Resources/logout_active.png";
        }

        private void Logout_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {

        }

        private void Home_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://ndr.neusoft.edu.cn");
        }

        private void NSS_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("http://go.neusoft.edu.cn");
        }

        private void ListBoxItem_Selected(object sender, RoutedEventArgs e)
        {
            MessageBox.Show(e.ToString());
        }
    }
}

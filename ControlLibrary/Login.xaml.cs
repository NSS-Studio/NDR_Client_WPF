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
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Windows.Media.Animation;

namespace ControlLibrary
{
    /// <summary>
    /// Interaction logic for Login.xaml
    /// </summary>
    /// 

    public partial class Login : UserControl
    {
        public string username = "";
        public string password = "";
        public bool isConnected
        {
            get
            {
                return isConnected;
            }
            set
            {
                isConnected = value;
            }
        }
        Rectangle[] recs = new Rectangle[6];
        public static readonly RoutedEvent Login_Event =
        EventManager.RegisterRoutedEvent("Login", RoutingStrategy.Bubble, typeof(RoutedEventHandler), typeof(Login));
        public event RoutedEventHandler Login_Click
        {
            add
            {
                AddHandler(Login_Event, value);
            }
            remove
            {
                RemoveHandler(Login_Event, value);
            }
        }

        public Login()
        {
            InitializeComponent();
        }


        private void Btn_login_Click(object sender, RoutedEventArgs e)
        {
            this.username = user_name.Text;
            this.password = pass.Password;
            RoutedEventArgs args = new RoutedEventArgs(Login_Event, this);
            RaiseEvent(args);
        }

        void startLoading()
        {
            this.main_panel.Opacity = 0;
            this.loading.Opacity = 1;

            DoubleAnimation opacityAnimation = animation(0, 1);
            this.loading.BeginAnimation(Grid.OpacityProperty, opacityAnimation);
            opacityAnimation = animation(1, 0);
            this.main_panel.BeginAnimation(Grid.OpacityProperty, opacityAnimation);
        }

        DoubleAnimation animation(int from, int to)
        {
            DoubleAnimation opacityAnimation = new DoubleAnimation();
            opacityAnimation.Duration = new Duration(TimeSpan.FromSeconds(0.3));
            opacityAnimation.From = from;
            opacityAnimation.To = to;
            return opacityAnimation;
        }

        public void connectedCallback()
        {
            this.main_panel.Opacity = 1;
            this.loading.Opacity = 0;
        }
    }
}

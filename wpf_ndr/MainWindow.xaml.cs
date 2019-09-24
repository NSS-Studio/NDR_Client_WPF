using System.Drawing;
using System.Windows;
using Hardcodet.Wpf.TaskbarNotification;
using System.Threading;
using System;
using ControlLibrary;
using System.Net.NetworkInformation;
using System.Windows.Controls;
using System.Net;
using System.Net.Sockets;
using log4net;

namespace wpf_ndr
{
    public partial class MainWindow : Window
    {
        public TaskbarIcon taskBarIcon;
        public ErrorWindow err_window = null;
        public MainWin main_win = null;
        private DialThread dial = null;
        public int stat = 0;
        public MainWindow()
        {
            Utils.log.Info("===================Start log===================");
            InitializeComponent();
            WindowBlur.SetIsEnabled(this, true);
            login_panel.Login_Click += new RoutedEventHandler(login_click);
            taskBarIcon = new TaskbarIcon
            {
                Icon = new Icon("icons/app.ico"),
                Visibility = Visibility.Visible,
                ToolTipText = "N-Client"
            };
        }
        public void login_click(object sender, RoutedEventArgs e)
        {
            dial = new DialThread(login_panel.username, login_panel.password, this);
            dial.dial_field += ShowErrorMsg;
            dial.dial_success += DialSuccess;
            Thread dial_thread = new Thread(dial.Dial);
            dial_thread.SetApartmentState(ApartmentState.STA);
            dial_thread.Start();
        }

        private void ShowErrorMsg(string text)
        {
            err_window = new ErrorWindow();
            err_window.SetContext(text);
            err_window.ShowDialog();
        }

        private void DialSuccess()
        {
            this.stat = 1;
            if(main_win == null)
            {
                main_win = new MainWin();
            }
            Width = 900;
            Height = 700;
            main_win.Height = 700;
            main_win.Width = 900;
            string ip = new GetIPAddress(Utils.adapterName).GetIPv4AddressByAdapterName();
            main_win.setNetIP(ip == "" ? "0.0.0.0" : ip);
            main_win.setUserName(login_panel.username);
            double screenWidth = System.Windows.SystemParameters.PrimaryScreenWidth;
            double screenHeight = System.Windows.SystemParameters.PrimaryScreenHeight;
            double windowWidth = this.Width;
            double windowHeight = this.Height;
            this.Left = (screenWidth / 2) - (windowWidth / 2);
            this.Top = (screenHeight / 2) - (windowHeight / 2);
            canvas.Children.Add(main_win);
            canvas.Children.Remove(login_panel);
            canvas.RegisterName("main_win", main_win);

            Thread dial_thread = new Thread(dial.getStatus);
            dial_thread.Start();
        }

        

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // TODO close event: when dial success minimum window
            //                   when nodial close window*-+
            if (stat != 0)
            {
                this.Visibility = Visibility.Hidden;
                e.Cancel = true;
            }
        }
    }
}

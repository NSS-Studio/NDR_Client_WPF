using System.Drawing;
using System.Windows;
using System;
using System.Threading;
using ControlLibrary;

namespace wpf_ndr
{
    class DialThread
    {
        private String username;
        private String password;
        private MainWindow main_window;
        public delegate void dial_faild_delegate(string text);
        public event dial_faild_delegate dial_field;
        public delegate void dial_success_delegate();
        public event dial_success_delegate dial_success;
        public DialThread(String username, String password, MainWindow main_window)
        {
            this.username = username;
            this.password = password;
            this.main_window = main_window;
        }
        public void Dial()
        {
            bool dial = ResouceManager.GetInstance().GetRasDial().RDial(username, password);
            //dial = true; // 模拟拨号成功
            if (!dial)
            {
                main_window.Dispatcher.BeginInvoke(new Action(() => main_window.Visibility = Visibility.Hidden));
                dial_field(ResouceManager.GetInstance().GetRasDial().GetErrorMsg());
                main_window.Dispatcher.BeginInvoke(new Action(() => main_window.Visibility = Visibility.Visible));
            }
            else
            {
                main_window.Dispatcher.BeginInvoke(new Action(dial_success));
            }
        }
        public void getStatus()
        {
            long time = 0;
            while (true)
            {
                bool is_disconnected = ResouceManager.GetInstance().GetRasDial().IsDisconnected();

                if (is_disconnected)
                {
                    main_window.stat = 0;
                }

                main_window.main_win.Dispatcher.BeginInvoke(new Action(() => this.main_window.main_win.setNetFlow(FlowInfo.GetFlowInfo())));
                main_window.main_win.Dispatcher.BeginInvoke(new Action(() => this.main_window.main_win.setNetTime(time)));
                Thread.Sleep(1000);
                time++;
            }
        }
    }
}

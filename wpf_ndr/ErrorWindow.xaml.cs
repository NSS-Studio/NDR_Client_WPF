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
using System.Windows.Shapes;

namespace wpf_ndr
{
    /// <summary>
    /// Interaction logic for ErrorWindow.xaml
    /// </summary>
    public partial class ErrorWindow : Window
    {
        public ErrorWindow()
        {
            InitializeComponent();
            WindowBlur.SetIsEnabled(this, true);
        }

        public void SetContext(string context)
        {
            this.context.Text = context;
        }

        private void Ok_Button_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}

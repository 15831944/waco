using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using FirstFloor.ModernUI.Presentation;

namespace TickCad
{
    /// <summary>
    /// Interaction logic for UserControl1.xaml
    /// </summary>
    public partial class WSUrl : Window
    {
        public WSUrl()
        {
            InitializeComponent();
        }

        

        private void Closebutton_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            txtWSUserName.Text = string.Empty;
            txtWSPassword.Text = string.Empty;
            txtWSMainUrl.Text = string.Empty;
            txtWSSubUrl.Text = string.Empty;

            this.Close();
        }

        private void Closebutton_MouseEnter(object sender, MouseEventArgs e)
        {
            Closebutton.Cursor = Cursors.Hand;
        }

        private void Closebutton_MouseLeave(object sender, MouseEventArgs e)
        {
            Closebutton.Cursor = Cursors.Arrow;
        }


        private void btnWSSave_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }
       
        private void btnWSClear_Click(object sender, RoutedEventArgs e)
        {
            txtWSUserName.Text = string.Empty;
            txtWSPassword.Text = string.Empty;
            txtWSMainUrl.Text = string.Empty;
            txtWSSubUrl.Text = string.Empty;
        }

        

        



    }
}

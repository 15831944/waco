using KwikscafApplication;
using System;
using System.Collections.Generic;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
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
using System.Xml;

namespace TickCad.Pages
{
    /// <summary>
    /// Interaction logic for TickCadLicense.xaml
    /// </summary>
    public partial class TickCadLicense : Page
    {
        public TickCadLicense()
        {
            InitializeComponent();
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            if (txtMachineName.Text == "")
            {
                MessageBox.Show("Please input Machine Name");
            }
            else if (txtMACAddress.Text == "")
            {
                MessageBox.Show("Please input User Domain Name");
            }
            else if (txtInventorVersion.Text == "")
            {
                MessageBox.Show("Please input Inventor Version Number");
            }
            else if (txtNoofDays.Text == "")
            {
                MessageBox.Show("Please input From Date");
            }
            else if (txtFilePath.Text == "")
            {
                MessageBox.Show("Please input FilePath");
            }
            else
            {  // @"F:\Temp.xml"
                try
                {
                    string Path = string.Empty;
                    if (txtFilePath.Text.Trim().EndsWith(@"\"))
                    {
                         Path = txtFilePath.Text + "License.XML";
                    }
                    else
                    {
                         Path = txtFilePath.Text + "\\License.XML";
                    }
                 
                    //Create a datatable to store XML data
                    DataTable DT = new DataTable();
                    DT.Columns.Add("MachineName");
                    DT.Columns.Add("MACAddress");
                    DT.Columns.Add("InventorVersionNumber");
                    DT.Columns.Add("NoOfDays");
                    DT.Rows.Add(new object[] { txtMachineName.Text.ToUpper(CultureInfo.CurrentCulture), txtMACAddress.Text.ToUpper(CultureInfo.CurrentCulture).Replace("-",""), txtInventorVersion.Text.ToUpper(CultureInfo.CurrentCulture), txtNoofDays.Text.ToUpper(CultureInfo.CurrentCulture) });
                    //Create a dataset
                    DataSet DS = new DataSet();
                    //Add datatable to this dataset
                    DS.Tables.Add(DT);
                    //XmlWriter writer = XmlWriter.Create(@"F:\Temp.xml");
                    //Write dataset to XML file
                    DS.WriteXml(Path);
                    XmlDocument xmlDoc = new XmlDocument();
                    xmlDoc.PreserveWhitespace = false;
                    xmlDoc.Load(Path);
                    Encryption.Encrypt(xmlDoc);
                    xmlDoc.Save(Path);
                    MessageBox.Show("XML data written successfully to " + Path + "");
                    clear();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Exception:- Path cannot Found " + ex.Message);
                }
            }
        }

        private void clear()
        {
            txtInventorVersion.Clear();
            txtMachineName.Clear();
            txtNoofDays.Clear();
            txtMACAddress.Clear();
            txtFilePath.Clear();
        }
        private void btnReset_Click(object sender, RoutedEventArgs e)
        {
            clear();
        }

        //private void txtNoofDays_KeyDown(object sender, KeyEventArgs e)
        //{
        //    if (!Char.IsDigit((char)KeyInterop.VirtualKeyFromKey(e.Key)) & e.Key != Key.Back | e.Key == Key.Space)
        //    {
        //        e.Handled = true;
        //        MessageBox.Show("It only accept numbers");
        //    }
        //}

        //private void txtInventorVersion_KeyDown(object sender, KeyEventArgs e)
        //{
        //    if (!Char.IsDigit((char)KeyInterop.VirtualKeyFromKey(e.Key)) & e.Key != Key.Back | e.Key == Key.Space)
        //    {
        //        e.Handled = true;
        //        MessageBox.Show("It only accept numbers");
        //    }
        //}

        private void NumberValidationTextBox(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("[^0-9]+");
            e.Handled = regex.IsMatch(e.Text);
        }
    }
}

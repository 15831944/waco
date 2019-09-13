using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Encryption;
using System.Xml;
using System.Data;
using System.Globalization;
using System.Text.RegularExpressions;

namespace License
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void btnSave_Click(object sender, RoutedEventArgs e)
        {
            if (txtMachineName.Text == "")
            {
                MessageBox.Show("Please Enter Machine Name");
            }
            else if (txtMACAddress.Text == "")
            {
                MessageBox.Show("Please Enter User Mac Address");
            }
            else if (txtInventorVersion.Text == "")
            {
                MessageBox.Show("Please Enter AutoCAD Version Number");
            }
            else if (txtNoofDays.Text == "")
            {
                MessageBox.Show("Please Enter From Date");
            }
            else if (txtFilePath.Text == "")
            {
                MessageBox.Show("Please Enter FilePath");
            }
            else
            {  
                try
                {               

                    string Path = string.Empty;
                    if (txtFilePath.Text.Trim().EndsWith(@"\"))
                    {
                        Path = txtFilePath.Text + @"License.XML";
                    }
                    else
                    {
                        Path = txtFilePath.Text + @"\License.XML";
                    }


                    ////if (txtFilePath.Text.Trim().EndsWith("/"))
                    ////{
                    ////    Path = txtFilePath.Text + "License.XML";
                    ////}
                    ////else
                    ////{
                    ////    Path = txtFilePath.Text + "/License.XML";
                    ////}  
                    
                    //if (txtFilePath.Text.Trim().EndsWith("/"))
                    //{
                    //    Path = txtFilePath.Text + @"License.XML";
                    //}
                    //else
                    //{
                    //    Path = txtFilePath.Text + "\\License.XML";
                    //} 

                    //Create a datatable to store XML data
                    
                    DataTable DT = new DataTable();
                    DT.Columns.Add("MachineName");
                    DT.Columns.Add("MACAddress");
                    DT.Columns.Add("InventorVersionNumber");
                    DT.Columns.Add("NoOfDays");
                    DT.Columns.Add("GUID");
                    String guid = Guid.NewGuid().ToString();
                    DT.Rows.Add(new object[] { txtMachineName.Text.ToUpper(CultureInfo.CurrentCulture), txtMACAddress.Text.ToUpper(CultureInfo.CurrentCulture).Replace("-", ""), txtInventorVersion.Text.ToUpper(CultureInfo.CurrentCulture), txtNoofDays.Text.ToUpper(CultureInfo.CurrentCulture), guid });
                    
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
                    ClsExtention.Encrypt(xmlDoc);
                    xmlDoc.Save(Path);
                    MessageBox.Show("XML data written successfully to " + Path + "");
                   
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Exception:- Path cannot Found " + ex.Message);
                }
            }
        }

        private void clear()
        {
            //txtInventorVersion.Text = string.Empty;
            txtMachineName.Text = string.Empty;
            txtNoofDays.Text = string.Empty;
            txtMACAddress.Text = string.Empty;
            //txtFilePath.Text = string.Empty;
        }
        private void btnReset_Click(object sender, RoutedEventArgs e)
        {
            clear();
        }

        private void NumberValidationTextBox(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("[^0-9]+");
            e.Handled = regex.IsMatch(e.Text);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Data;
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
using IniAccessor;


namespace TickCad.Pages
{
    /// <summary>
    /// Interaction logic for UserSetting.xaml
    /// </summary>
    public partial class UserSetting : Page
    {


        #region Data Members Declaration



        #endregion


        #region List, Dataset, Datatable Declaration


        List<ConstantValues> listCV = new List<ConstantValues>();

        List<ConstantValues> AvailableInformationList = new List<ConstantValues>();

        List<ConstantValues> DeletedInformationList = new List<ConstantValues>();

        List<ConstantValues> serviceList = new List<ConstantValues>();

        List<string> WSList = new List<string>();

        List<string> ECList = new List<string>();

        #endregion


        #region Constructor

        public UserSetting()
        {

            InitializeComponent();

            dgAvailableInfo.ItemsSource = null;
            dgDeletedInfo.ItemsSource = null;
            AvailableInformationList.Clear();
            DeletedInformationList.Clear();

            WSList.Clear();
            ECList.Clear();

            #region Get User Setting Values

            listCV.Clear();
            listCV = GetConstantValues();

            DataTable dt = getDeletedIniFileValues();

            if (dt.Rows.Count > 0)
            {

                GetIniInformation();

            }
            else
            {
                GetIniInformation(listCV);
            }

            #endregion
        }


        #endregion


        #region Loaded


        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            //dgAvailableInfo.ItemsSource = null;
            //dgDeletedInfo.ItemsSource = null;
            //AvailableInformationList.Clear();
            //DeletedInformationList.Clear();

            //dtConstantValue = null;
            //dtConstantValue = GetConstantValues();

            //DataTable dt = getDeletedIniFileValues();

            //if (dt.Rows.Count > 0)
            //{

            //    GetIniInformation();

            //}
            //else
            //{
            //    GetIniInformation(dtConstantValue);
            //}

        }

        #endregion


        #region Get Constant Values


        private List<ConstantValues> GetConstantValues()
        {
            List<ConstantValues> cvList = new List<ConstantValues>();
            ConstantValues cv = new ConstantValues();
            return cvList = cv.ConstantInformation();
        }


        #endregion


        #region Get Ini Information for Loading User Setting Datagrid


        private DataTable getDeletedIniFileValues()
        {
            DataTable dtDelValues = new DataTable();
            dtDelValues.Columns.Add("Headers", typeof(string));

            string sPath = System.Reflection.Assembly.GetExecutingAssembly().Location.Trim();

            Int32 nIndex = sPath.LastIndexOf(@"\");
            string iniFilePath = "";
            sPath = sPath.Substring(0, nIndex);
            if (System.IO.File.Exists(sPath + @"\UserSetting.ini"))
            {
                iniFilePath = sPath + @"\UserSetting.ini";
                IniFile _ini = new IniFile(iniFilePath);

                // need to add file name


                //  string CurrentDrawingName = extension.ToUpper() + "INFO";


                string formatCollection = _ini.GetIniFileString(iniFilePath, "SETPROPERTIES", "IDWINFO");

                if (!string.IsNullOrEmpty(formatCollection))
                {

                    String[] _getValue = formatCollection.Replace("\0", "").Split(',');

                    foreach (string item in _getValue)
                    {
                        ConstantValues cv = new ConstantValues();

                        if (item != string.Empty)
                        {
                            if (item.Contains("-"))
                            {
                                string[] items = item.Split('-');

                                cv.IsChecked = Convert.ToBoolean(items[0]);
                                cv.Name = items[1].ToString();
                                AvailableInformationList.Add(cv);
                            }
                            else
                            {
                                cv.IsChecked = false;
                                cv.Name = item.ToString();
                                AvailableInformationList.Add(cv);

                            }
                        }
                    }

                }

                formatCollection = _ini.GetIniFileString(iniFilePath, "SETDELPROPERTIES", "IDWINFO");

                if (!string.IsNullOrEmpty(formatCollection))
                {


                    String[] _getValue = formatCollection.Replace("\0", "").Split(',');

                    foreach (string item in _getValue)
                    {

                        ConstantValues cv = new ConstantValues();

                        if (item != string.Empty)
                        {
                            if (item.Contains("-"))
                            {
                                string[] items = item.Split('-');

                                cv.IsChecked = Convert.ToBoolean(items[0]);
                                cv.Name = items[1].ToString();
                                DeletedInformationList.Add(cv);
                                dtDelValues.Rows.Add(items[1].ToString());
                            }
                            else
                            {
                                cv.IsChecked = false;
                                cv.Name = item.ToString();
                                DeletedInformationList.Add(cv);
                                dtDelValues.Rows.Add(item);

                            }
                        }


                    }



                }


            }
            else
            {
                iniFilePath = "";
            }

            return dtDelValues;

        }


        #endregion


        #region After Getting Ini Information Check Values With and Without Constant Values


        public void GetIniInformation(List<ConstantValues> dtGet)
        {
            AvailableInformationList = dtGet;

            dgAvailableInfo.ItemsSource = AvailableInformationList;
        }


        public void GetIniInformation()
        {
            dgAvailableInfo.ItemsSource = null;

            dgDeletedInfo.ItemsSource = null;

            AvailableInformationList.Clear();


            AvailableInformationList = (from w1 in listCV
                                        where !DeletedInformationList.Any(w2 => w2.Name == w1.Name)
                                        select w1).ToList();


            dgAvailableInfo.ItemsSource = AvailableInformationList;

            dgDeletedInfo.ItemsSource = DeletedInformationList;

        }


        #endregion


        #region Remove

        private void btnUserSettingRemove_Click(object sender, RoutedEventArgs e)
        {

            List<ConstantValues> dvInform = new List<ConstantValues>();




            if (dgAvailableInfo.SelectedItems.Count > 0)
            {
                try
                {

                    List<ConstantValues> selectedList = dgAvailableInfo.SelectedItems.Cast<ConstantValues>().ToList();

                    foreach (ConstantValues item in selectedList)
                    {
                        ConstantValues cv = new ConstantValues();
                        cv.IsChecked = item.IsChecked;
                        cv.Name = item.Name;
                        dvInform.Add(cv);
                    }

                    dgAvailableInfo.ItemsSource = null;
                    dgDeletedInfo.ItemsSource = null;


                    List<ConstantValues> dtAv = new List<ConstantValues>();

                    foreach (ConstantValues item in dvInform)
                    {
                        ConstantValues cv = new ConstantValues();
                        cv.IsChecked = item.IsChecked;
                        cv.Name = item.Name;
                        dtAv.Add(cv);
                        DeletedInformationList.Add(cv);
                    }

                    AvailableInformationList = (from w1 in AvailableInformationList
                                                where !dtAv.Any(w2 => w2.Name == w1.Name)
                                                select w1).ToList();



                }
                catch
                {
                    AvailableInformationList.Clear();
                }
                finally
                {
                    dgAvailableInfo.ItemsSource = AvailableInformationList;

                    dgDeletedInfo.ItemsSource = DeletedInformationList;
                }
            }
        }

        #endregion


        #region Add

        private void btnUserSettingAdd_Click(object sender, RoutedEventArgs e)
        {
            List<ConstantValues> avInform = new List<ConstantValues>();



            if (dgDeletedInfo.SelectedItems.Count > 0)
            {
                try
                {
                    List<ConstantValues> selectedList = dgDeletedInfo.SelectedItems.Cast<ConstantValues>().ToList();

                    foreach (ConstantValues item in selectedList)
                    {
                        ConstantValues cv = new ConstantValues();
                        cv.IsChecked = item.IsChecked;
                        cv.Name = item.Name;
                        avInform.Add(cv);
                    }

                    dgAvailableInfo.ItemsSource = null;
                    dgDeletedInfo.ItemsSource = null;


                    List<ConstantValues> dtAv = new List<ConstantValues>();


                    foreach (ConstantValues item in avInform)
                    {
                        ConstantValues cv = new ConstantValues();
                        cv.IsChecked = item.IsChecked;
                        cv.Name = item.Name;
                        dtAv.Add(cv);
                        AvailableInformationList.Add(cv);
                    }




                    DeletedInformationList = (from w1 in DeletedInformationList
                                              where !dtAv.Any(w2 => w2.Name == w1.Name)
                                              select w1).ToList();

                }
                catch
                {
                    DeletedInformationList.Clear();
                }
                finally
                {
                    dgAvailableInfo.ItemsSource = AvailableInformationList;

                    dgDeletedInfo.ItemsSource = DeletedInformationList;
                }
            }
        }

        #endregion


        #region Save


        private void btnUserSettingSave_Click(object sender, RoutedEventArgs e)
        {

            if (txtMachineName.Text != string.Empty && txtMacAddress.Text != string.Empty && txtPath.Text != string.Empty)
            {
                List<ConstantValues> avaList = (List<ConstantValues>)dgAvailableInfo.ItemsSource;
                List<ConstantValues> delList = (List<ConstantValues>)dgDeletedInfo.ItemsSource;

                SetIniFileValues(avaList, delList);
            }
            else
            {
                MessageBox.Show("Machine Name, Mac Address, Path are mandatory", "Tick CAD", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }


        #endregion


        #region Set Ini Information

        private void SetIniFileValues(List<ConstantValues> avaList, List<ConstantValues> delList)
        {




            grbDrawingInformation.IsEnabled = false;


            string sPath = System.Reflection.Assembly.GetExecutingAssembly().Location.Trim();
            Int32 nIndex = sPath.LastIndexOf(@"\");
            string iniFilePath = "";
            string iniFilePathLocal = "";
            string sPathLocal = txtPath.Text;
            Int32 nIndexLocal = sPathLocal.LastIndexOf(@"\");
            sPath = sPath.Substring(0, nIndex);
            sPathLocal = sPathLocal.Substring(0, nIndexLocal);

            if (System.IO.File.Exists(sPath + @"\UserSetting.ini"))
            {
                iniFilePath = sPath + @"\UserSetting.ini";
                iniFilePathLocal = sPathLocal + @"\UserSetting.ini";

                IniFile _ini = new IniFile(iniFilePath);
                IniFile _iniLocal = new IniFile(iniFilePathLocal);

                string properties = string.Empty;
                string delProperties = string.Empty;

                int i = AvailableInformationList.Count;
                int j = 0;

                foreach (ConstantValues item in avaList)
                {

                    if (i - 1 == j)
                    {
                        if (item.Name != string.Empty)
                        {
                            properties += item.IsChecked + "-" + item.Name;
                            j++;
                        }

                    }
                    else
                    {
                        if (item.Name != string.Empty)
                        {
                            properties += item.IsChecked + "-" + item.Name + ",";
                            j++;
                        }
                    }
                }

                int x = DeletedInformationList.Count;
                int y = 0;

                foreach (ConstantValues item in DeletedInformationList)
                {

                    if (x - 1 == y)
                    {
                        if (item.Name != string.Empty)
                        {
                            delProperties += item.IsChecked + "-" + item.Name;
                            y++;
                        }

                    }
                    else
                    {
                        if (item.Name != string.Empty)
                        {
                            delProperties += item.IsChecked + "-" + item.Name + ",";
                            y++;
                        }
                    }
                }



                _ini.SetIniFileString(iniFilePath, "SETPROPERTIES", "IDWINFO", properties);
                _ini.SetIniFileString(iniFilePath, "SETDELPROPERTIES", "IDWINFO", delProperties);
                _ini.SetIniFileString(iniFilePath, "SYSTEMINFORMATION", "MachineName", txtMachineName.Text);
                _ini.SetIniFileString(iniFilePath, "SYSTEMINFORMATION", "MacAddress", txtMacAddress.Text);


                _iniLocal.SetIniFileString(iniFilePathLocal, "SETPROPERTIES", "IDWINFO", properties);
                _iniLocal.SetIniFileString(iniFilePathLocal, "SETDELPROPERTIES", "IDWINFO", delProperties);
                _iniLocal.SetIniFileString(iniFilePathLocal, "SYSTEMINFORMATION", "MachineName", txtMachineName.Text);
                _iniLocal.SetIniFileString(iniFilePathLocal, "SYSTEMINFORMATION", "MacAddress", txtMacAddress.Text);


                if (WSList.Count > 0)
                {

                    foreach (string item in WSList)
                    {
                        string[] setp = item.Split(',');
                        string prop = setp[0].ToString();
                        string userName = setp[1].ToString();
                        string password = setp[2].ToString();
                        string mainUrl = setp[3].ToString();
                        string subUrl = setp[4].ToString();

                        _iniLocal.SetIniFileString(iniFilePathLocal, "Service-" + prop, prop, userName + "," + password + "," + mainUrl + "," + subUrl);

                    }
                }

                if (ECList.Count > 0)
                {

                    foreach (string item in ECList)
                    {
                        string[] setp = item.Split(',');
                        string prop = setp[0].ToString();
                        string input = setp[1].ToString();
                        string output = setp[2].ToString();


                        _iniLocal.SetIniFileString(iniFilePathLocal, "Excel-" + prop, prop, input + "," + output);

                    }
                }


                MessageBox.Show("User Setting Information written successfully to " + txtPath.Text + "UserSetting.ini", "Information", MessageBoxButton.OK, MessageBoxImage.Information);


                Reload();

            }
            else
            {
                iniFilePath = "";
            }
            grbDrawingInformation.IsEnabled = true;


        }



        #endregion


        public void Reload()
        {

            dgAvailableInfo.ItemsSource = null;
            dgDeletedInfo.ItemsSource = null;
            AvailableInformationList.Clear();
            DeletedInformationList.Clear();

            WSList.Clear();
            ECList.Clear();

            #region Get User Setting Values

            listCV.Clear();
            listCV = GetConstantValues();

            DataTable dt = getDeletedIniFileValues();

            if (dt.Rows.Count > 0)
            {

                GetIniInformation();

            }
            else
            {
                GetIniInformation(listCV);
            }

            #endregion
        }


        private void rdbService_Click(object sender, RoutedEventArgs e)
        {

            RadioButton rdbSer = sender as RadioButton;
            bool chk = rdbSer.IsChecked.Value;
            string removeCheck = string.Empty;

            ConstantValues cv = rdbSer.DataContext as ConstantValues;

            if (chk == true)
            {
                int i = 0;
                if (ECList.Count > 0)
                {

                    foreach (string item in ECList)
                    {
                        if (item.Contains(cv.Name))
                        {
                            removeCheck = item;
                            i = 1;
                            break;
                        }
                    }
                }

                if (i == 0)
                {
                    GetWSCheck(cv);
                }
                else
                {
                    MessageBoxResult ms = MessageBox.Show("Property already assigned in Excel, Do you want to change from Excel to Service", "TickCAD", MessageBoxButton.OKCancel, MessageBoxImage.Information);
                    if (ms == MessageBoxResult.OK)
                    {
                        GetWSCheck(cv);
                        ECList.Remove(removeCheck);
                    }

                    
                }
            }



           
        }

        public void GetWSCheck(ConstantValues cv)
        {
            if (cv.Name != null || cv.Name != string.Empty)
            {
              
                    ConstantValues cvs = new ConstantValues();
                    cvs.Name = cv.Name;
                    cvs.ServiceIsChecked = true;
                    cvs.ExcelIsChecked = false;
                    serviceList.Add(cvs);
                    WSUrl wsurl = new WSUrl();
                    wsurl.txtWSProperties.Text = cvs.Name;
                    wsurl.ShowDialog();

                    wsurl.Closing += wsurl_Closing;

                    if (wsurl.txtWSUserName.Text != string.Empty && wsurl.txtWSPassword.Text != string.Empty && wsurl.txtWSMainUrl.Text != string.Empty && wsurl.txtWSSubUrl.Text != string.Empty)
                    {
                        WSList.Add(wsurl.txtWSProperties.Text + "," + wsurl.txtWSUserName.Text + "," + wsurl.txtWSPassword.Text + "," + wsurl.txtWSMainUrl.Text + "," + wsurl.txtWSSubUrl.Text);

                    }

           
            }
        }




        void wsurl_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {


        }

        //void wsurl_Closed(object sender, EventArgs e)
        //{
        //    WSUrl wsurl = new WSUrl();
        //    string ss = wsurl.txtWSMainUrl.Text;
        //}


        private void rdbExcel_Click(object sender, RoutedEventArgs e)
        {
            RadioButton rdbSer = sender as RadioButton;
            bool chk = rdbSer.IsChecked.Value;
            string removeCheck = string.Empty;

            ConstantValues cv = rdbSer.DataContext as ConstantValues;

            if (chk == true)
            {
                int i = 0;
                if (WSList.Count > 0)
                {

                    foreach (string item in WSList)
                    {
                        if (item.Contains(cv.Name))
                        {
                            removeCheck = item;
                            i = 1;
                            break;
                        }
                    }
                }

                if (i == 0)
                {
                    GetEXCheck(cv);
                }
                else
                {
                    MessageBoxResult ms = MessageBox.Show("Property already assigned in Service, Do you want to change from Service to Excel", "TickCAD", MessageBoxButton.OKCancel, MessageBoxImage.Information);
                    if (ms == MessageBoxResult.OK)
                    {
                        GetEXCheck(cv);
                        WSList.Remove(removeCheck);
                    }


                }
            }



           


        }

        void ecl_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {

        }

        public void GetEXCheck(ConstantValues cv)
        {
            if (cv.Name != null || cv.Name != string.Empty)
            {
                
                    ConstantValues cvs = new ConstantValues();
                    cvs.Name = cv.Name;
                    cvs.ServiceIsChecked = false;
                    cvs.ExcelIsChecked = true;
                    serviceList.Add(cvs);
                    ECL ecl = new ECL();
                    ecl.txtECProperties.Text = cvs.Name;
                    ecl.ShowDialog();



                    ecl.Closing += ecl_Closing;

                    if (ecl.txtECInputColumn.Text != string.Empty && ecl.txtECOutputColumn.Text != string.Empty)
                    {
                        ECList.Add(ecl.txtECProperties.Text + "," + ecl.txtECInputColumn.Text + "," + ecl.txtECOutputColumn.Text);

                    }

              
            }
        }





    }


}

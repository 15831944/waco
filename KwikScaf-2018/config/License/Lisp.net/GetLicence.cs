using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using System.Xml;
using Encryption;
using Microsoft.Win32;
using System.Globalization;
using System.Net.NetworkInformation;
using System.Data;

using System.Security.Cryptography;
using System.Windows;

namespace LisptoNet
{
    public class GetLicence
    {

        string globalPath = @"Lisp\Protection";
        string reg_Install = "Install";
        string reg_Use = "Use";
        string reg_TICKCAD = "Lisp";
        string guid = string.Empty;


        private DateTime GetCreationDateOfFolder(string Path)
        {
            string directoryString = Path;
            Directory.CreateDirectory(directoryString);
            DateTime dateTime = Directory.GetCreationTime(directoryString);
            return dateTime;
        }

        private string SetKey()
        {
            if (globalPath != string.Empty)
            {

                return globalPath;
            }
            else
            {
                return "SampleKey";
            }

        }

        private String dayDifPutPresent(int NoOFDAYS)
        {
            // get present date from system
            DateTime dt = DateTime.Now;
            string today = dt.ToShortDateString();
            DateTime presentDate = Convert.ToDateTime(today);

            // get instalation date
            RegistryKey regkey = Registry.CurrentUser;

            string encrp_global = EncriptionDecription.Encrypt(globalPath, SetKey());
            string encrp_install = EncriptionDecription.Encrypt(reg_Install, SetKey());
            string encrp_use = EncriptionDecription.Encrypt(reg_Use, SetKey());


            regkey = regkey.CreateSubKey(@"Software\" + encrp_global); //path
            string Br = (string)regkey.GetValue(encrp_install);

            // string encrp_br = EncriptionDecription.Decrypt(Br, SetKey());


            DateTime installationDate = Convert.ToDateTime(Br);

            TimeSpan diff = presentDate.Subtract(installationDate); //first.Subtract(second);
            int totaldays = (int)diff.TotalDays;

            // special check if user chenge date in system
            string usd = (string)regkey.GetValue(encrp_use);

            //string encrp_usd = EncriptionDecription.Decrypt(usd, SetKey());


            DateTime lastUse = Convert.ToDateTime(usd);
            TimeSpan diff1 = presentDate.Subtract(lastUse); //first.Subtract(second);
            int useBetween = (int)diff1.TotalDays;

            // put next use day in registry
            regkey.SetValue(encrp_use, today); //Value Name,Value Data

            if (useBetween >= 0)
            {

                if (totaldays < 0)
                    return "Error"; // if user change date in system like date set before installation
                else if (totaldays >= 0 && totaldays <= NoOFDAYS)
                    return Convert.ToString(NoOFDAYS - totaldays); //how many days remaining
                else
                    return "Expired"; //Expired
            }
            else
                return "Error"; // if user change date in system
        }

        private void blackList()
        {
            RegistryKey regkey = Registry.CurrentUser;


            string encrp_global = EncriptionDecription.Encrypt(globalPath, SetKey());
            string black = EncriptionDecription.Encrypt("Black", SetKey());

            regkey = regkey.CreateSubKey(@"Software\" + encrp_global); //path
            regkey.SetValue(black, "True");
        }

        private bool blackListCheck()
        {
            RegistryKey regkey = Registry.CurrentUser;

            string encrp_global = EncriptionDecription.Encrypt(globalPath, SetKey());
            string black = EncriptionDecription.Encrypt("Black", SetKey());

            regkey = regkey.CreateSubKey(@"Software\" + encrp_global); //path

            if (!string.IsNullOrEmpty((string)regkey.GetValue(black)))
                return false; //No
            else
                return true;//Yes
        }

        private String checkfirstDate()
        {
            RegistryKey regkey = Registry.CurrentUser;

            string encrp_global = EncriptionDecription.Encrypt(globalPath, SetKey());
            string encrp_install = EncriptionDecription.Encrypt(reg_Install, SetKey());
            string encrp_use = EncriptionDecription.Encrypt(reg_Use, SetKey());
            string encrp_tickcad = EncriptionDecription.Encrypt(reg_TICKCAD, SetKey());
            //string encrp_version = EncriptionDecription.Encrypt("V.1.10", SetKey());

            regkey = regkey.CreateSubKey(@"Software\" + encrp_global); //path
            string Br = (string)regkey.GetValue(encrp_install);
            string Brs = (string)regkey.GetValue(encrp_tickcad);


            if (Brs == guid)
            {
                Br = "Same";
            }
            else
            {
                try
                {
                    string[] regs = regkey.GetSubKeyNames();

                    if (regs.Count() > 0)
                    {

                        regkey.DeleteValue(encrp_install);
                        regkey.DeleteValue(encrp_use);
                        regkey.DeleteValue(encrp_tickcad);
                        if (regkey.GetValue(encrp_install) == null)
                            return "First";
                        else
                            Br = "First";

                    }
                    else
                    {
                        Br = "First";

                    }
                }
                catch
                {
                }


            }
            return Br;
        }

        private void firstTime()
        {
            RegistryKey regkey = Registry.CurrentUser;

            string encrp_global = EncriptionDecription.Encrypt(globalPath, SetKey());
            string encrp_install = EncriptionDecription.Encrypt(reg_Install, SetKey());
            string encrp_use = EncriptionDecription.Encrypt(reg_Use, SetKey());
            string encrp_tickcad = EncriptionDecription.Encrypt(reg_TICKCAD, SetKey());



            //  string encrp_version = EncriptionDecription.Encrypt(guid, SetKey());

            regkey = regkey.CreateSubKey(@"Software\" + encrp_global); //path

            DateTime dt = DateTime.Now;
            string onlyDate = dt.ToShortDateString(); // get only date not time

            //  string encrp_onlyDate = EncriptionDecription.Encrypt(dt.ToShortDateString(), SetKey());

            regkey.SetValue(encrp_install, onlyDate); //Value Name,Value Data
            regkey.SetValue(encrp_use, onlyDate); //Value Name,Value Data
            regkey.SetValue(encrp_tickcad, guid);
        }

        public string GetMACAddress()
        {
            NetworkInterface[] nics = NetworkInterface.GetAllNetworkInterfaces();
            String sMacAddress = string.Empty;
            foreach (NetworkInterface adapter in nics)
            {
                if (sMacAddress == String.Empty)// only return MAC Address from first card  
                {
                    IPInterfaceProperties properties = adapter.GetIPProperties();
                    sMacAddress = adapter.GetPhysicalAddress().ToString();
                }
            } return sMacAddress;
        }

        public bool ApplicationStatus()
        {
            string path = @"C:\Program Files\New Post Tensioning\License.xml";
            //string path = "C:/License.xml";
          //  DataTable dt = getIniFileValues();


            if (File.Exists(path))
            {
                System.Data.DataSet _ds = new System.Data.DataSet();
                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc = ClsExtention.getXmlData(path);
                _ds.ReadXml(new XmlTextReader(new System.IO.StringReader(xmlDoc.InnerXml)));

                guid = _ds.Tables[0].Rows[0]["GUID"].ToString();


                if (_ds.Tables[0].Rows.Count > 0)
                {
                    string macAddress = GetMACAddress();
                    //string mas = _ds.Tables[0].Rows[0]["MacAddress"].ToString().ToUpper(CultureInfo.CurrentCulture);
                    //string mmm = _ds.Tables[0].Rows[0]["MachineName"].ToString().ToUpper(CultureInfo.CurrentCulture);

                    //string ff = Environment.MachineName.ToUpper(CultureInfo.CurrentCulture);
                    //if (macAddress.ToUpper().Trim() == mas.ToUpper().Trim())
                    //{

                    //}
                    //if (mmm == ff)
                    //{

                    //}

                    //if (_ds.Tables[0].Rows[0]["MacAddress"].ToString().ToUpper(CultureInfo.CurrentCulture) == macAddress.ToUpper()  && _ds.Tables[0].Rows[0]["MachineName"].ToString().ToUpper(CultureInfo.CurrentCulture) == Environment.MachineName.ToUpper(CultureInfo.CurrentCulture))
                    if (_ds.Tables[0].Rows[0]["MacAddress"].ToString().Trim().Equals(macAddress.Trim(), StringComparison.InvariantCultureIgnoreCase) && _ds.Tables[0].Rows[0]["MachineName"].ToString().Equals(Environment.MachineName.Trim(), StringComparison.InvariantCultureIgnoreCase))
                    {
                        // check registry version match with application version no
                        //if availble same
                        // else not matching or no tag delete entire path and proceed with same code.
                        //  regkey.SetValue("Version", "V1.6");

                        bool block = blackListCheck();
                        string chinstall = checkfirstDate();
                        if (chinstall == "First")
                        {

                            firstTime();
                            return true;

                        }
                        else
                        {
                            string status = dayDifPutPresent(Convert.ToInt16(_ds.Tables[0].Rows[0]["NoOfDays"].ToString()));
                            if (status == "Error")
                            {
                                blackList();
                                MessageBoxResult ds = MessageBox.Show("Application Can't be loaded, Unauthorized Date Interrupt Occurred! Without activation it can't run! Would you like to activate? Please Contact Administrator!", "Terminate Error-02", MessageBoxButton.OK, MessageBoxImage.Error);
                                if (ds == MessageBoxResult.Yes)
                                {

                                    if (ds == MessageBoxResult.OK)
                                        return false;

                                }
                                else
                                    return false;
                            }
                            else if (status == "Expired")
                            {
                                MessageBoxResult ds = MessageBox.Show("The trial version is now expired! Would you Like to Activate? Please Contact Administrator!", "Product key", MessageBoxButton.OK, MessageBoxImage.Information);
                                if (ds == MessageBoxResult.Yes)
                                {


                                    if (ds == MessageBoxResult.OK)
                                        return false;

                                }
                                else
                                    return false;
                            }

                        }

                    }




                    else
                    {
                        MessageBoxResult ds = MessageBox.Show("Machine Name or Mac Address is not valid. Please Contact Administartor!", "Freyssinet", MessageBoxButton.OK, MessageBoxImage.Error);
                        if (ds == MessageBoxResult.Yes)
                        {

                            if (ds == MessageBoxResult.OK)
                                return false;

                        }
                        else
                            return false;

                    }


                }

            }

            else
            {
                return false;

            }
            return true;
        }


    }

    public class EncriptionDecription
    {
        private static string _key;

        public EncriptionDecription()
        {
        }


        public static string Key
        {
            set
            {
                _key = value;
            }
        }

        /// <summary>
        /// Encrypt the given string using the default key.
        /// </summary>
        /// <param name="strToEncrypt">The string to be encrypted.</param>
        /// <returns>The encrypted string.</returns>
        public static string Encrypt(string strToEncrypt)
        {
            try
            {
                return Encrypt(strToEncrypt, _key);
            }
            catch (Exception ex)
            {
                return "Wrong Input. " + ex.Message;
            }

        }

        /// <summary>
        /// Decrypt the given string using the default key.
        /// </summary>
        /// <param name="strEncrypted">The string to be decrypted.</param>
        /// <returns>The decrypted string.</returns>
        public static string Decrypt(string strEncrypted)
        {
            try
            {
                return Decrypt(strEncrypted, _key);
            }
            catch (Exception ex)
            {
                return "Wrong Input. " + ex.Message;
            }
        }

        /// <summary>
        /// Encrypt the given string using the specified key.
        /// </summary>
        /// <param name="strToEncrypt">The string to be encrypted.</param>
        /// <param name="strKey">The encryption key.</param>
        /// <returns>The encrypted string.</returns>
        public static string Encrypt(string strToEncrypt, string strKey)
        {
            try
            {
                TripleDESCryptoServiceProvider objDESCrypto = new TripleDESCryptoServiceProvider();
                MD5CryptoServiceProvider objHashMD5 = new MD5CryptoServiceProvider();

                byte[] byteHash, byteBuff;
                string strTempKey = strKey;

                byteHash = objHashMD5.ComputeHash(ASCIIEncoding.ASCII.GetBytes(strTempKey));
                objHashMD5 = null;
                objDESCrypto.Key = byteHash;
                objDESCrypto.Mode = CipherMode.ECB; //CBC, CFB

                byteBuff = ASCIIEncoding.ASCII.GetBytes(strToEncrypt);
                return Convert.ToBase64String(objDESCrypto.CreateEncryptor().TransformFinalBlock(byteBuff, 0, byteBuff.Length));
            }
            catch (Exception ex)
            {
                return "Wrong Input. " + ex.Message;
            }
        }


        /// <summary>
        /// Decrypt the given string using the specified key.
        /// </summary>
        /// <param name="strEncrypted">The string to be decrypted.</param>
        /// <param name="strKey">The decryption key.</param>
        /// <returns>The decrypted string.</returns>
        public static string Decrypt(string strEncrypted, string strKey)
        {
            try
            {
                TripleDESCryptoServiceProvider objDESCrypto = new TripleDESCryptoServiceProvider();
                MD5CryptoServiceProvider objHashMD5 = new MD5CryptoServiceProvider();

                byte[] byteHash, byteBuff;
                string strTempKey = strKey;

                byteHash = objHashMD5.ComputeHash(ASCIIEncoding.ASCII.GetBytes(strTempKey));
                objHashMD5 = null;
                objDESCrypto.Key = byteHash;
                objDESCrypto.Mode = CipherMode.ECB; //CBC, CFB

                byteBuff = Convert.FromBase64String(strEncrypted);
                string strDecrypted = ASCIIEncoding.ASCII.GetString(objDESCrypto.CreateDecryptor().TransformFinalBlock(byteBuff, 0, byteBuff.Length));
                objDESCrypto = null;

                return strDecrypted;
            }
            catch (Exception ex)
            {
                return "Wrong Input. " + ex.Message;
            }
        }
    }

}

using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TickCad
{
    public class ConstantValues
    {
        public string Name { get; set; }
        public Boolean IsChecked { get; set; }
        public Boolean ServiceIsChecked { get; set; }
        public Boolean ExcelIsChecked { get; set; }

        public List<ConstantValues> ConstantInformation()
        {
            List<ConstantValues> list = new List<ConstantValues>();

            string drawingInformation = "Title,Subject,Author,Keywords,Comments,Last Saved By,Revision Number,Manager,Company,Category,Document SubType Name,Part Number,Stock Number,Description,Project,Designer,Engineer,Authority,Cost Center,Cost,Creation Time,Vendor,Catalog Web Link,User Status,Design Status,Checked By,Date Checked,Engr Approved By,Engr Date Approved,Mfg Approved By,Mfg Date Approved,Thumbnail";

            String[] _getValue = drawingInformation.Replace("\0", "").Split(',');

            foreach (string item in _getValue)
            {
                if (item != string.Empty)
                {
                    ConstantValues cv = new ConstantValues();
                    cv.IsChecked = false;
                    cv.ExcelIsChecked = false;
                    cv.ServiceIsChecked = false;
                    cv.Name = item;
                    list.Add(cv);
                }
            }

            return list;

        }


    }
}

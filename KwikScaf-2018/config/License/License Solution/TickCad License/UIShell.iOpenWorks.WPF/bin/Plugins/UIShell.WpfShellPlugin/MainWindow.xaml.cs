using FirstFloor.ModernUI.Windows.Controls;
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
using UIShell.WpfShellPlugin.ExtensionModel;
using FirstFloor.ModernUI.Presentation;

namespace UIShell.WpfShellPlugin
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public static ShellExtensionPointHandler ShellExtensionPointHandler { get; set; }
        private List<Tuple<LinkGroupData, LinkGroup>> LinkGroupTuples { get; set; }
        
        public MainWindow()
        {
            InitializeComponent();
            LinkGroupTuples = new List<Tuple<LinkGroupData, LinkGroup>>();
            ShellExtensionPointHandler = new ShellExtensionPointHandler(BundleActivator.Bundle);
            //ShellExtensionPointHandler.LinkGroups.CollectionChanged += LinkGroups_CollectionChanged;
            //InitializeLinkGroupsForExtensions();
        }

      
    }
}

using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;
using FirstFloor.ModernUI.Presentation;

namespace TickCad
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            AppearanceManager.Current.AccentColor = TickCad.Properties.Settings.Default.Color;
            AppearanceManager.Current.FontSize = TickCad.Properties.Settings.Default.FontSize;
            AppearanceManager.Current.ThemeSource = new Uri(TickCad.Properties.Settings.Default.ThemeSource, UriKind.Relative);
        }
    }
}

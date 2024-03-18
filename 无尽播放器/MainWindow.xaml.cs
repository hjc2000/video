using Microsoft.Extensions.DependencyInjection;
using System.Windows;

namespace 无尽播放器;
/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
	public MainWindow()
	{
		InitializeComponent();
		ServiceCollection serviceCollection = new();
		serviceCollection.AddWpfBlazorWebView();
		Resources.Add("services", serviceCollection.BuildServiceProvider());
	}
}

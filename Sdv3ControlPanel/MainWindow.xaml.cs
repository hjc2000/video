using JCNET;
using Microsoft.Extensions.DependencyInjection;
using System.Windows;

namespace Sdv3ControlPanel;
/// <summary>
///		Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
	public MainWindow()
	{
		InitializeComponent();
		ServiceCollection serviceCollection = new();
		serviceCollection.AddWpfBlazorWebView();
		Resources.Add("services", serviceCollection.BuildServiceProvider());

		LogOutputPort = new LogOutputPort("log.txt");
		serviceCollection.AddSingleton((p) =>
		{
			return LogOutputPort;
		});
	}

	private LogOutputPort LogOutputPort { get; set; }
}

using JCNET;
using Microsoft.Extensions.DependencyInjection;
using RazorUI.导航;
using System.Collections.Generic;
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
		serviceCollection.InjectRedirector();
		serviceCollection.AddWpfBlazorWebView();
		Resources.Add("services", serviceCollection.BuildServiceProvider());
		LogOutputPort = new LogOutputPort("log.txt");
	}

	private LogOutputPort LogOutputPort { get; set; }
}

internal static class Injector
{
	public static void InjectRedirector(this IServiceCollection services)
	{
		services.AddSingleton<IRedirectUriProvider>((p) =>
		{
			return new DictionaryRedirectUriProvider()
			{
				new KeyValuePair<string, string>("", "control/config"),
				new KeyValuePair<string, string>("control", "control/config"),
			};
		});

		services.AddScoped<Redirector>();
	}
}

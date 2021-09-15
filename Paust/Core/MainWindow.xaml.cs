using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using ICSharpCode.AvalonEdit.CodeCompletion;
using ICSharpCode.AvalonEdit.Document;
using ICSharpCode.AvalonEdit.Editing;
using Newtonsoft.Json;

namespace Paust.Core
{
    public partial class MainWindow : Window
    {
        private readonly SortedSet<CompletionData> completionDatas = new SortedSet<CompletionData>();
        
        public MainWindow()
        {
            this.InitializeComponent();

            this.CtlConent.IsEnabled = false;

            this.CtlJavascriptBody.Encoding = Encoding.UTF8;

            this.CtlJavascriptBody.Options.HighlightCurrentLine = true;

            this.CtlJavascriptBody.TextArea.TextEntering += this.CtlJavascriptBody_TextArea_TextEntering;
            this.CtlJavascriptBody.TextArea.TextEntered += this.CtlJavascriptBody_TextArea_TextEntered;

            using (var r = new StringReader(Properties.Resources.party_property))
            {
                string line;

                var re = new Regex(@"^([^\t]*)\t+([^\t]+)\t+([^\t]+)$");
                while ((line = r.ReadLine()) != null)
                {
                    var m = re.Match(line);
                    if (!m.Success) continue;

                    _ = this.completionDatas.Add(
                        new CompletionData(m.Groups[1].Value, m.Groups[2].Value, m.Groups[3].Value)
                    );
                }
            }

            this.CtlJavascrriptProfileList.ItemsSource = Settings.Instance.JavaScript.Keys.ToList();
        }

        private class LatestRealease
        {
            [JsonProperty("tag_name")]
            public string TagName { get; set; }
        }
        private async void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.Show();

            try
            {
                using (var client = new HttpClient())
                {
                    using (var req = new HttpRequestMessage(HttpMethod.Get, "https://api.github.com/repos/RyuaNerin/Paust/releases/latest"))
                    {
                        req.Headers.UserAgent.Add(new ProductInfoHeaderValue("ScraperBot"));

                        using (var res = await client.SendAsync(req, HttpCompletionOption.ResponseHeadersRead))
                        using (var content = await res.Content.ReadAsStreamAsync())
                        using (var sReader = new StreamReader(content))
                        {
                            var last = await Task.Factory.StartNew(() => JsonConvert.DeserializeObject<LatestRealease>(sReader.ReadToEnd()));

                            if (new Version(last.TagName) > Assembly.GetExecutingAssembly().GetName().Version)
                            {
                                Process.Start(new ProcessStartInfo { UseShellExecute = true, FileName = "\"https://github.com/RyuaNerin/Paust\"" }).Dispose();
                            }
                        }
                    }
                }
            }
            catch
            {
            }

            this.CtlConent.IsEnabled = true;
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            this.CtlConent.IsEnabled = false;

            var data = new Injector.IpcInput
            {
                ShortNameEnabled = false,
                FilterEnabled = false,
            };
            Injector.ApplyFilterWithoutInjection(data);
        }

        private CompletionWindow completionWindow;
        private void CtlJavascriptBody_TextArea_TextEntered(object sender, TextCompositionEventArgs e)
        {
            // text...
            var str = this.CtlJavascriptBody.Text.Substring(0, this.CtlJavascriptBody.CaretOffset);

            if (e.Text == ".")
            {
                var c = this.completionDatas.Where(le => le.IsMatched(str)).ToArray();
                if (c.Length == 0)
                {
                    return;
                }

                this.completionWindow = new CompletionWindow(this.CtlJavascriptBody.TextArea);
                this.completionWindow.Closed += (_1, _2) => this.completionWindow = null;

                var data = this.completionWindow.CompletionList.CompletionData;
                data.Clear();
                foreach (var ac in c)
                {
                    data.Add(ac);
                }

                this.completionWindow.Show();
            }
        }

        private void CtlJavascriptBody_TextArea_TextEntering(object sender, TextCompositionEventArgs e)
        {
            if (e.Text.Length > 0)
            {
                if (!char.IsLetterOrDigit(e.Text[0]))
                {
                    this.completionWindow?.CompletionList.RequestInsertion(e);
                }
            }
        }

        public class CompletionData : ICompletionData, IComparable<CompletionData>
        {
            public CompletionData(string prefix, string text, string description)
            {
                this.regex = new Regex(prefix, RegexOptions.Compiled);
                this.Text = text;
                this.Description = description;
            }

            private readonly Regex regex;
            public bool IsMatched(string body) => this.regex.IsMatch(body);

            public ImageSource Image => null;

            public string Text { get; }

            public object Content => this.Text;

            public object Description { get; }

            public double Priority => 0;

            public void Complete(TextArea textArea, ISegment completionSegment, EventArgs insertionRequestEventArgs)
            {
                textArea.Document.Replace(completionSegment, this.Text);
            }

            public int CompareTo(CompletionData other)
            {
                return this.Text.CompareTo(other.Text);
            }
        }

        private void CtlJavascriptProfileSave_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var key = this.CtlJavascriptProfileName.Text.Trim();
            if (string.IsNullOrWhiteSpace(key)) return;

            Settings.Instance.JavaScript[key] = this.CtlJavascriptBody.Text;
            Settings.Instance.Save();

            this.CtlJavascrriptProfileList.ItemsSource = Settings.Instance.JavaScript.Keys.ToList();
            this.CtlJavascrriptProfileList.SelectedItem = key;
        }

        private void CtlJavascriptProfileLoad_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var key = this.CtlJavascrriptProfileList.SelectedItem as string;
            if (string.IsNullOrWhiteSpace(key)) return;

            this.CtlJavascriptProfileName.Text = key;
            this.CtlJavascriptBody.Text = Settings.Instance.JavaScript[key];
            _ = this.CtlJavascriptBody.Focus();

            var cur = this.CtlJavascrriptProfileList.SelectedItem;
            this.CtlJavascrriptProfileList.ItemsSource = Settings.Instance.JavaScript.Keys.ToList();
            this.CtlJavascrriptProfileList.SelectedItem = cur;
        }

        private void CtlJavascriptProfileDelete_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var key = this.CtlJavascrriptProfileList.SelectedItem as string;
            if (string.IsNullOrWhiteSpace(key)) return;

            _ = Settings.Instance.JavaScript.Remove(key);
            Settings.Instance.Save();

            this.CtlJavascrriptProfileList.ItemsSource = Settings.Instance.JavaScript.Keys.ToList();
        }

        private async void CtlAttach_Click(object sender, RoutedEventArgs e)
        {
            this.CtlAttach.IsEnabled = false;

            var rpcData = new Injector.IpcInput
            {
                ShortNameEnabled = this.CtlShortName.IsChecked ?? false,
                FilterEnabled    = this.CtlEnable.IsChecked    ?? false,
            };
            if (rpcData.FilterEnabled)
            {
                rpcData.FilterScript = this.CtlJavascriptBody.Text;
            }

            try
            {
                var r = await Injector.ApplyFilter(rpcData);

                if (r.Succeed)
                {
                    _ = MessageBox.Show(this, "적용되었습니다..", this.Title, MessageBoxButton.OK, MessageBoxImage.Information);
                }
                else
                {
                    _ = MessageBox.Show(this, $"오류가 발생하였습니다.\n\n{r.Error}", this.Title, MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
            catch (Injector.InjectionException)
            {
                _ = MessageBox.Show(this, "파이널 판타지 14가 실행중이지 않거나 권한이 없습니다.", this.Title, MessageBoxButton.OK, MessageBoxImage.Error);
            }
            catch (Exception ex)
            {
                _ = MessageBox.Show(this, $"알 수 없는 오류가 발생하였습니다.\n\n{ex}", this.Title, MessageBoxButton.OK, MessageBoxImage.Error);
            }
            finally
            {
                this.CtlAttach.IsEnabled = true;
            }
        }

        private void CtlManual_MouseDown(object sender, MouseButtonEventArgs e)
        {
            try
            {
                Process.Start(new ProcessStartInfo { UseShellExecute = true, FileName = "\"https://github.com/RyuaNerin/Paust\"" }).Dispose();
            }
            catch
            {
            }
        }
    }
}

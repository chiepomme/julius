using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace TestApp
{
    class Program
    {
        [DllImport("recognition.dll")]
        private static extern void initialize(string filename);
        [DllImport("recognition.dll")]
        private static extern void set_debug_log_func(DebugLogDelegate debugLog);
        [DllImport("recognition.dll")]
        private static extern void set_audio_callback(AudioReadCallback callback);
        [DllImport("recognition.dll")]
        private static extern void set_result_func(OutputResultDelegate outputResult);
        [DllImport("recognition.dll")]
        private static extern void stop();

        public delegate void DebugLogDelegate(string message);
        public delegate void OutputResultDelegate(string result);
        public delegate int AudioReadCallback(IntPtr buffer, int requestedBytes);

        static DebugLogDelegate d;
        static OutputResultDelegate r;

        static void Main(string[] args)
        {
            var thread = new Thread(new ThreadStart(() =>
            {
                d = DebugLog;
                r = Result;
                set_debug_log_func(d);
                set_result_func(r);
                initialize(@"C:\Users\chiepomme\Downloads\grammar-kit-4.3.1\grammar-kit-4.3.1\SampleGrammars\fruit\testmic.jconf");
            }));
            thread.Start();

            Thread.Sleep(20000);
            stop();
        }

        static void DebugLog(string msg)
        {
            // Console.WriteLine(msg);
        }

        static void Result(string msg)
        {
            Console.WriteLine("==========================");
            Console.WriteLine(msg);
            Console.WriteLine("==========================");
        }
    }
}

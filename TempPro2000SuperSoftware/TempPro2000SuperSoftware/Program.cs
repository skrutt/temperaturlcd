using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.IO.Ports;

namespace TempPro2000SuperSoftware
{
    class Program
    {
        static StreamWriter _streamWriter;
        static SerialPort _serialPort;
        static string[] ComList = new string[10];
        static bool readData = false;
        static int perc;
        static int comSelect;

        static int bufferSize = 105;
        static string[] fileBuffer = new string[bufferSize];
        static int bufferPointer = 0;

        static void Main(string[] args)
        {
            _serialPort = new SerialPort();

            init();
            
            Console.WriteLine("Försöker öppna COM-port");
            //Försök öppna com-port:
            try
            {
                _serialPort.PortName = ComList[comSelect]; //Ange vilken port som skall användas
                _serialPort.Encoding = Encoding.ASCII;
                _serialPort.BaudRate = 9600;
                _serialPort.DataBits = 8;
                _serialPort.StopBits = StopBits.One;
                _serialPort.Open();
                Console.WriteLine();
                Console.WriteLine("COM-port öppnad");
                Console.WriteLine();
            }
            //Om det inte går skriv ut felet:
            catch (Exception e)
            {
                Console.WriteLine();
                Console.WriteLine("Kunde inte öppna COM-port: " + e + ".");
                Console.WriteLine("\nTryck på valfri knapp för att börja om.");
                Console.ReadKey();
                init(); //Starta om programmet
            }
            _serialPort.DataReceived += new SerialDataReceivedEventHandler(responseHandler); //Knyt en event handler till när det kommer input. 
            Console.WriteLine("Väntar på data...");
            Console.ReadKey();
        }

        static void init()
        {
            Console.Clear();
            int comNr = 1;
            Console.BackgroundColor = ConsoleColor.Blue;
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("----------------------------");
            Console.WriteLine("Temp Pro 2000 Super Software");
            Console.WriteLine("----------------------------\n");
            Console.BackgroundColor = ConsoleColor.Black;
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine("Tillgängliga COM-portar:");

            //Skriv ut lista med tillgängliga serieportar:
            foreach (string s in SerialPort.GetPortNames())
            {
                Console.Write(comNr + ".");
                Console.WriteLine(s);
                ComList[comNr] = s; //Spara porten i en array
                comNr++;
            }
            Console.WriteLine("\nVälj COM-port");
            try
            {
                comSelect = Int32.Parse(Console.ReadLine()); //Input sträng till int
                Console.WriteLine("Du har valt " + ComList[comSelect]);
            }
            catch (Exception e)
            {
                Console.WriteLine("Den valda porten finns ej. Tryck på valfri knapp för att börja om. ");
                Console.ReadKey();
                init(); //Starta om programmet
            }
        }

        //Behandla inkommande data:
        private static void responseHandler(object sender, SerialDataReceivedEventArgs args)
        {
            string y = _serialPort.ReadLine();
            if (y.Length > 0)
            {
                y = y.Remove(0, 1);
            }
           

            //Om stoppsträngen kommer:
            if (y == "stop")
            {
                readData = false; //Läs ej in mer data
                string fileDir = Environment.GetFolderPath(Environment.SpecialFolder.Desktop) + "\\tempData_" + DateTime.Now.ToString("yyyy-MM-dd-HH-mm-ss") + ".csv"; //Plats för sparning av csv-filen
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine();
                Console.WriteLine("Sparar data till fil...");
                File.WriteAllText(fileDir, string.Empty); //Om filen redan skulle existera, radera allt innehåll.
                int i = 1;
                //Loopa igenom alla temperaturvärden och skriv dessa till filen:
                foreach (string s in fileBuffer)
                {
                    using (_streamWriter = new StreamWriter(fileDir, true)) //Öppna filen 
                    {
                        _streamWriter.WriteLine(i + ";" +s); //Skriv till filen
                        i++; //Öka pekare till nästa temperaturvärde

                    }
                    
                }
                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("Data sparad i " + fileDir);
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine("\nVäntar På Data...");
            }

            //Läs temperaturdata:
            if (readData)
            {
                if (bufferPointer == bufferSize) //Om buffern är full
                {
                    bufferPointer = 0;
                }
                int colPos = y.IndexOf(';') + 1; //Position för skiljetecknet ; som står före temperaturvärdet
                y = y.Substring(colPos); //Plocka ut temperaturvärdet
                fileBuffer[bufferPointer] = y; //Spara värdet i temperaturarrayen
                bufferPointer++;
                perc = (bufferPointer * 100) / bufferSize; //Räkna ut hur många procent som har tagits emot
                Console.Write("Tar emot data: " + perc + "%" + "\r");
                
            }

            if (y == "start")
            {
                readData = true;
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine();
            }
        }

    }
}

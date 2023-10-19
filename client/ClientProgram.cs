using System.Text;
using System.Text.RegularExpressions;
using TcpIp;

namespace client
{
    internal class ClientProgram
    {
        static byte[] Parse((int, int) value)
        {
            return Encoding.UTF8.GetBytes($"{value.Item1} {value.Item2}");
        }

        static void Main(string[] args)
        {
            string text = File.ReadAllText("addr_server.txt");
            Client<(int, int)> client = new Client<(int, int)>(text, 8080);
            client.ToBytes = Parse;
            client.Connect();
            string ans;
            Regex reg = new Regex("[0-9]+ [0-9]+");
            Console.WriteLine("Enter \"*source* *dest*\" or ex to exit");
            while (true)
            {
                ans = Console.ReadLine() ?? "";
                if (ans == "ex")
                {
                    break;
                }
                
                if (reg.IsMatch(ans)) 
                {
                    string[] values = ans.Split(" ");
                    int source = int.Parse(values[0]);
                    int dest = int.Parse(values[1]);
                    client.Send((source, dest));
                }
            }
            Console.WriteLine("Press enter to continue...");
            Console.ReadLine();
        }
    }
}
using System.Text;
using TcpIp;

namespace ProgramNamespace
{
    public class ServerProgram
    {
        public class Edge
        {
            public int Source;
            public int Destination;
            public int Weight;

            public Edge(int source, int destination, int weight)
            {
                Source = source;
                Destination = destination;
                Weight = weight;
            }
        }

        public static List<int> BellmanFord(List<Edge> edges, int numVertices, int source)
        {
            List<int> distance = new List<int>(new int[numVertices]);
            for (int i = 0; i < numVertices; i++)
                distance[i] = int.MaxValue;

            source--;
            distance[source] = 0;

            // Релаксация рёбер
            for (int i = 1; i < numVertices; i++)
            {
                foreach (Edge edge in edges)
                {
                    if (distance[edge.Source] != int.MaxValue && distance[edge.Source] + edge.Weight < distance[edge.Destination])
                    {
                        distance[edge.Destination] = distance[edge.Source] + edge.Weight;
                    }
                }
            }

            // Проверка наличия циклов с отрицательным весом
            foreach (Edge edge in edges)
            {
                if (distance[edge.Source] != int.MaxValue && distance[edge.Source] + edge.Weight < distance[edge.Destination])
                {
                    return new List<int>();
                }
            }

            //// Вывод кратчайших путей
            //for (int i = 0; i < numVertices; i++)
            //{
            //    Console.WriteLine($"{i} \t\t {distance[i]}");
            //}

            return distance;
        }

        public static int GetMinDistance((int, int) tuple)
        {
            int source = tuple.Item1;
            int destination = tuple.Item2;

            int result = 0;
            if (source < 0 || source > numVertices || destination < 0 || destination > numVertices)
            {
                result = 1;
            }

            List<int> dists = BellmanFord(edges, numVertices, source);
            result = dists[destination - 1];
            Console.WriteLine($"For task {tuple} result is {result}");
            return result;
        }

        static int numVertices = 10;
        static List<Edge> edges = new List<Edge>
        {
            new Edge(source: 0, destination: 1, weight: 5),
            new Edge(source: 0, destination: 2, weight: 3),
            new Edge(source: 1, destination: 2, weight: 2),
            new Edge(source: 1, destination: 3, weight: 7),
            new Edge(source: 2, destination: 3, weight: 4),
            new Edge(source: 3, destination: 4, weight: 1),
            new Edge(source: 4, destination: 0, weight: 2),
            new Edge(source: 0, destination: 5, weight: 3),
            new Edge(source: 5, destination: 2, weight: 6),
            new Edge(source: 4, destination: 5, weight: 8),
            new Edge(source: 6, destination: 7, weight: 2),
            new Edge(source: 7, destination: 8, weight: 4),
            new Edge(source: 8, destination: 6, weight: 6),
            new Edge(source: 6, destination: 9, weight: 5),
            new Edge(source: 7, destination: 9, weight: 1),
            new Edge(source: 9, destination: 8, weight: 3),
            new Edge(source: 9, destination: 5, weight: 9),
            new Edge(source: 2, destination: 7, weight: 5),
            new Edge(source: 1, destination: 4, weight: 3),
            new Edge(source: 3, destination: 6, weight: 4),
            new Edge(source: 8, destination: 5, weight: 2),
            new Edge(source: 2, destination: 4, weight: 1),
            new Edge(source: 7, destination: 5, weight: 3),
            new Edge(source: 9, destination: 0, weight: 7)
        };

        static (int, int) FromBytes(byte[] bytes)
        {
            string line = Encoding.UTF8.GetString(bytes);
            string[] values = line.Split(' ');
            (int, int) tuple = (int.Parse(values[0]), int.Parse(values[1]));
            return tuple;
        }

        public static void Main(string[] args)
        {
            string text = File.ReadAllText("addr_info.txt");
            Listener<(int, int), int> server = new Listener<(int, int), int>(100, text, 8080);
            server.Operation = GetMinDistance;
            server.FromBytes = FromBytes;
            server.StartServer();
            string ans;
            while(true)
            {
                Console.WriteLine("Enter ex to exit");
                ans = Console.ReadLine();
                if (ans == "ex")
                {
                    server.EndServer();
                    break;
                }
            }
            Console.WriteLine("Press enter to continue...");
            Console.ReadLine();
        }
    }
}

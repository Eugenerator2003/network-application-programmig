using System.Net;
using System.Net.Sockets;
using System.Text;

namespace TcpIp
{
    public class Client<TParameter> : TcpObject
    {
        public Func<TParameter, byte[]> ToBytes { get; set; }

        private IPEndPoint serverEndPoint;

        private bool isWorking;
        private Queue<TParameter> messages;
        private Queue<TParameter> sendedMessages;
        private Mutex mutex;

        public Client(string serverAddres, int port) : base(serverAddres, port)
        {
            mutex = new Mutex(false);
            messages = new();
            sendedMessages = new();
            ThreadPool.SetMinThreads(3, 1);
        }

        public void Connect()
        {
            socket.Connect(serverEndPoint);
            Console.WriteLine($"Connected to {Address}:{Port}");
            isWorking = true;
            ThreadPool.QueueUserWorkItem(Sending);
            ThreadPool.QueueUserWorkItem(Receiving);
        }

        public void End()
        {
            isWorking = false;
        }

        public void Send(TParameter message)
        {
            mutex.WaitOne();
            messages.Enqueue(message);
            mutex.ReleaseMutex();
        }

        public override void Dispose()
        {
            socket.Close();
            GC.SuppressFinalize(this);
        }

        protected override Socket CreateSocket()
        {
            serverEndPoint = new IPEndPoint(IPAddress.Parse(Address), Port);
            Socket socket = new Socket(serverEndPoint.AddressFamily, SocketType.Stream, ProtocolType.IP);
            return socket;
        }

        private void Sending(object? obj)
        {
            while(isWorking)
            {
                if (messages.Count > 0)
                {
                    mutex.WaitOne();
                    var param = messages.Dequeue();
                    sendedMessages.Enqueue(param);
                    mutex.ReleaseMutex();
                    socket.Send(ToBytes(param));
                }
            }
        }

        private void Receiving(object? obj)
        {
            byte[] buffer = new byte[100];
            while(isWorking)
            {
                socket.Receive(buffer);
                mutex.WaitOne();
                Console.WriteLine($"Answer to {sendedMessages.Dequeue()} is {Encoding.UTF8.GetString(buffer)}");
                mutex.ReleaseMutex();
            }
        }
    }
}
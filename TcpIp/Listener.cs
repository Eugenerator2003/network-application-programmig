using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Diagnostics;
using System.Net;

namespace TcpIp
{
    public class Listener<TParameter, TResult> : TcpObject
    {
        public readonly bool MutableOperation;

        public Func<byte[], TParameter>? FromBytes { get; set; }
        public Func<TParameter, TResult>? Operation { get; set; }

        private readonly int bufferSize;
        private bool isListen;

        private readonly Queue<Socket> clientsQueue;
        private readonly List<Socket> clientsList;
        private readonly Mutex mutex;

        public Listener(int bufferSize, string address, int port, bool mutableOperation = true) : base(address, port)
        {
            ThreadPool.SetMinThreads(3, 2);

            this.bufferSize = bufferSize;
            MutableOperation = mutableOperation;

            mutex = new Mutex(false);
            clientsQueue = new Queue<Socket>();
            clientsList = new List<Socket>();
        }

        public void Listen()
        {
            socket.Listen();
            isListen = true;
        }

        public void StartServer()
        {
            if (Operation is null || FromBytes is null)
            {
                throw new NullReferenceException("Operation doesn't setted");
            }

            if (!isListen)
            {
                Listen();
            }

            ThreadPool.QueueUserWorkItem(Accepting);

            Console.WriteLine($"Server started at {Address}:{Port}");

            //acceptingTask = Task.Run(Accepting);
        }

        public void EndServer()
        {
            isListen = false;
        }

        public override void Dispose()
        {
            isListen = false;
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();
           
            stopwatch.Stop();
            clientsList.ForEach(c => c.Close());
            socket.Close();
            GC.SuppressFinalize(this);
        }

        protected void HandleClient(object? obj)
        {
            mutex.WaitOne();
            Socket socket = clientsQueue.Dequeue();
            mutex.ReleaseMutex();

            byte[] buffer = new byte[bufferSize];
            while (isListen)
            {
                try
                {
                    socket.Receive(buffer);
                    if (MutableOperation)
                    {
                        mutex.WaitOne();
                    }

                    TResult result = Operation.Invoke(FromBytes.Invoke(buffer));
                    byte[] bytes = Encoding.UTF8.GetBytes(result?.ToString() ?? "None");
                    Array.Clear(buffer);
                    Array.Copy(bytes, buffer, bytes.Length);
                    socket.Send(buffer);
                   
                    if (MutableOperation)
                    {
                        mutex.ReleaseMutex();
                    }
                }
                catch (Exception)
                {
                    IPEndPoint endPoint = (IPEndPoint)socket.LocalEndPoint;
                    Console.WriteLine($"Client {endPoint.Address}:{endPoint.Port} has disconnected");
                    break;
                }
            }
        }

        protected override Socket CreateSocket()
        {
            IPEndPoint ipPoint = new IPEndPoint(IPAddress.Parse(Address), Port);
            Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.Bind(ipPoint);
            return socket;
        }

        private void Accepting(object? obj)
        {
            while (isListen)
            {
                Socket client = socket.Accept();
                IPEndPoint endPoint = (IPEndPoint)client.LocalEndPoint;
                Console.WriteLine($"Client {endPoint.Address}:{endPoint.Port} has connected");
                mutex.WaitOne();
                clientsQueue.Enqueue(client);
                clientsList.Add(client);
                ThreadPool.QueueUserWorkItem(HandleClient);
                mutex.ReleaseMutex();
            }
        }
    }
}

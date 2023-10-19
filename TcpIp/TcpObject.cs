using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TcpIp
{
    public abstract class TcpObject : IDisposable
    {
        protected static readonly int WaitingTime = 60;

        public int Port { get; }

        public string Address { get; }

        protected readonly Socket socket;

        public TcpObject(string address, int port)
        {
            this.Port = port;
            this.Address = address;

            socket = CreateSocket();
        }

        public abstract void Dispose();

        protected abstract Socket CreateSocket();
    }
}

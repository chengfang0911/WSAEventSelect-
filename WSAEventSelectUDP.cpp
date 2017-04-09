// 事件句柄和套节字句柄表
WSAEVENT	eventArray[WSA_MAXIMUM_WAIT_EVENTS];
SOCKET		sockArray[WSA_MAXIMUM_WAIT_EVENTS];
int nEventTotal = 0;

USHORT nPort = 6000;	// 此服务器监听的端口号

// 创建监听套节字
SOCKET s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);	
sockaddr_in sin;
sin.sin_family = AF_INET;
sin.sin_port = htons(nPort);
sin.sin_addr.S_un.S_addr = INADDR_ANY;
if(::bind(s, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
{
	printf(" Failed bind() /n");
	return -1;
}

// 创建事件对象，并关联到新的套节字
WSAEVENT event = ::WSACreateEvent();
::WSAEventSelect(s, event, FD_READ|FD_CLOSE);

// 添加到表中
eventArray[nEventTotal] = event;
sockArray[nEventTotal] = s;	
nEventTotal++;

// 处理网络事件
while(TRUE)
{
	// 在所有事件对象上等待
	int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
	// 对每个事件调用WSAWaitForMultipleEvents函数，以便确定它的状态
	nIndex = nIndex - WSA_WAIT_EVENT_0;
	for(int i=nIndex; i<nEventTotal; i++)
	{
		int ret;
		ret = ::WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 1000, FALSE);
		if(ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT)
		{
			continue;
		}
		else
		{
			// 获取到来的通知消息，WSAEnumNetworkEvents函数会自动重置受信事件
			WSANETWORKEVENTS event;
			::WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event);
			if(event.lNetworkEvents & FD_READ)			// 处理FD_READ通知消息
			{
				if(event.iErrorCode[FD_READ_BIT] == 0)
				{
					char szText[256];
					int nRecv = ::recv(sockArray[i], szText, strlen(szText), 0);
					if(nRecv > 0)				
					{
						szText[nRecv] = '/0';
						printf("接收到数据：%s /n", szText);
					}
				}
			}


		}

	}

}
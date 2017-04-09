// �¼�������׽��־����
WSAEVENT	eventArray[WSA_MAXIMUM_WAIT_EVENTS];
SOCKET		sockArray[WSA_MAXIMUM_WAIT_EVENTS];
int nEventTotal = 0;

USHORT nPort = 6000;	// �˷����������Ķ˿ں�

// ���������׽���
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

// �����¼����󣬲��������µ��׽���
WSAEVENT event = ::WSACreateEvent();
::WSAEventSelect(s, event, FD_READ|FD_CLOSE);

// ��ӵ�����
eventArray[nEventTotal] = event;
sockArray[nEventTotal] = s;	
nEventTotal++;

// ���������¼�
while(TRUE)
{
	// �������¼������ϵȴ�
	int nIndex = ::WSAWaitForMultipleEvents(nEventTotal, eventArray, FALSE, WSA_INFINITE, FALSE);
	// ��ÿ���¼�����WSAWaitForMultipleEvents�������Ա�ȷ������״̬
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
			// ��ȡ������֪ͨ��Ϣ��WSAEnumNetworkEvents�������Զ����������¼�
			WSANETWORKEVENTS event;
			::WSAEnumNetworkEvents(sockArray[i], eventArray[i], &event);
			if(event.lNetworkEvents & FD_READ)			// ����FD_READ֪ͨ��Ϣ
			{
				if(event.iErrorCode[FD_READ_BIT] == 0)
				{
					char szText[256];
					int nRecv = ::recv(sockArray[i], szText, strlen(szText), 0);
					if(nRecv > 0)				
					{
						szText[nRecv] = '/0';
						printf("���յ����ݣ�%s /n", szText);
					}
				}
			}


		}

	}

}
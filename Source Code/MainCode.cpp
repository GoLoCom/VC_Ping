#pragma once
#include <Windows.h>
#include "icmp.h"

HWND hWnd_WinClass;
HWND hWnd_Edit;
HWND hWnd_Static;
HWND hWnd_Button;
LRESULT CALLBACK WinProc(HWND,UINT,WPARAM,LPARAM);
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	MSG Msg;
	WNDCLASS WndClass;
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WinProc;
	WndClass.lpszClassName=L"YWinClass";
	WndClass.lpszMenuName=NULL;
	WndClass.style=NULL;
	RegisterClass(&WndClass);
	hWnd_WinClass=CreateWindow(L"YWinClass",L"",WS_BORDER|WS_SYSMENU,CW_USEDEFAULT,CW_USEDEFAULT,306,173,NULL,NULL,hInstance,0);
	hWnd_Edit=CreateWindow(L"edit",L"",ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE,5,10,235,20,hWnd_WinClass,NULL,hInstance,0);
	hWnd_Button=CreateWindow(L"button",L"Ping",BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,245,5,50,30,hWnd_WinClass,NULL,hInstance,0);
	hWnd_Static=CreateWindow(L"static",L"",SS_LEFT|WS_CHILD|WS_VISIBLE,5,40,290,100,hWnd_WinClass,NULL,hInstance,0);
	ShowWindow(hWnd_WinClass,SW_SHOWNORMAL);
	Ping::InitWSA();
	UpdateWindow(hWnd_WinClass);
	while(GetMessage(&Msg,NULL,0,0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
LRESULT CALLBACK WinProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_CTLCOLOREDIT://EDIT控件
		if((HWND)lParam==hWnd_Edit)
		{
			SetBkColor((HDC)wParam,RGB(200,200,200));
			return (INT_PTR)CreateSolidBrush(RGB(200,200,200));
		}
		break;
	case WM_COMMAND:
		if(((HWND)lParam==hWnd_Button)&&(HIWORD(wParam)==BN_CLICKED))
		{
			char WebSiteAddr[256]={0};
			int Result=0;
			GetWindowTextA(hWnd_Edit,WebSiteAddr,256);
			Ping tPing;
			Result=tPing.ping(WebSiteAddr);
			switch(Result)
			{
			case -1:
				SetWindowTextA(hWnd_Static,"");
				MessageBox(NULL,L"创建套接字失败",L"",0);
				break;
			case -2:
				SetWindowTextA(hWnd_Static,"");
				MessageBox(NULL,L"查询DNS服务器失败",L"",0);
				break;
			case -3:
				SetWindowTextA(hWnd_Static,"");
				MessageBox(NULL,L"发送ICMP包失败",L"",0);
				break;
			case -4:
				SetWindowTextA(hWnd_Static,"");
				MessageBox(NULL,L"接收ICMP包失败",L"",0);
				break;
			case -5:
				SetWindowTextA(hWnd_Static,"");
				MessageBox(NULL,L"超时",L"",0);
				break;
			default:
				char tOutNum[16];
				char* Ip=tPing.GetIP();
				char tOut[64]={""};
				strcpy(tOut,Ip);//这个系列函数不安全!
				itoa(Result,tOutNum,10);
				strcat(tOut,":");
				strcat(tOut,tOutNum);
				strcat(tOut,"ms\r\n");
				SetWindowTextA(hWnd_Static,tOut);
				break;
			}
		}
		break;
	case WM_CLOSE:
		Ping::CloseWSA();
		exit(0);
		break;
	}
	return DefWindowProc(hWnd,Msg,wParam,lParam);
}
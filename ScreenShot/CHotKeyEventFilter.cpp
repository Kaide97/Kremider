#include "CHotKeyEventFilter.hpp"
#include <string>
#include <QDebug>

CHotKeyEventFilter::CHotKeyEventFilter(WId id)
    : mWid(id)
{
    RegisterHotKeys();
}

CHotKeyEventFilter::~CHotKeyEventFilter()
{
    UnregisterHotKeys();
}

bool CHotKeyEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
  //  qDebug() << "nativeEventFilter type is :" << eventType;
    if(eventType == "windows_generic_MSG")
    {
        MSG* msg = static_cast<MSG*>(message);
        switch(msg->message)
        {
            case WM_HOTKEY:
                qDebug() << "Here is on HotKey!";
                qDebug() << "msg->wParam is :" << msg->wParam;
                qDebug() << "msg->lParam is :" << msg->lParam;
             //   qDebug() << "( MOD_ALT | MOD_CONTROL , 'A')) :" <<  MOD_ALT | MOD_CONTROL + 'A';
            if(msg->wParam == mOwnId)//热键触发
               emit HotKeyTrigger();
//            if(msg->wParam == mOwnId2)
//                emit HotKeyTrigger_2();
//}
                return true;
            default:
                break;
        }
    }
    if(eventType == "windows_dispatcher_MSG")
    {}
    return false;
}

int CHotKeyEventFilter::RegisterHotKeys(void)
{
    qDebug() << "mWid is :" << mWid;
   // mHotKeyId = GlobalAddAtom("myHotKeyId") - 0xc000;
     RegisterHotKey(reinterpret_cast<HWND>(mWid), mOwnId, MOD_ALT | MOD_CONTROL , 'A');
     //RegisterHotKey(reinterpret_cast<HWND>(mWid), mOwnId2, MOD_ALT | MOD_CONTROL , 'Q');//注册CTRL+ALT+Q
    return 0;
}




void CHotKeyEventFilter::UnregisterHotKeys(void)
{
    if(false != UnregisterHotKey(reinterpret_cast<HWND>(mWid), mOwnId))//reinterpret_cast
        qDebug() << "unregisterHotKey success!";
    else
        qDebug() << "UnregisterHotKey failed!";
    return ;
}

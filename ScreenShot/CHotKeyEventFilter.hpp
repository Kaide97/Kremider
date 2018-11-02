#ifndef CHOTKEYEVENTFILTER_HPP
#define CHOTKEYEVENTFILTER_HPP

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <Windows.h>
#include <qwindowdefs.h>

class CHotKeyEventFilter : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit CHotKeyEventFilter(WId id);
    ~CHotKeyEventFilter();

    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result);

signals:
    void HotKeyTrigger(void);
    void HotKeyTrigger_2(void);
public slots:

private:
    int RegisterHotKeys(void);//for distinguish with the win32 API RegisterHotKey

    void UnregisterHotKeys(void);

private:
    ATOM mHotKeyId;
    WId mWid;
    int mOwnId;
    int mOwnId2;

};

#endif // CHOTKEYEVENTFILTER_HPP

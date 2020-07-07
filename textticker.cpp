
#include "textticker.h"
#include <QPainter>
#include <QTimer>
#include <ctime>

TextTicker::TextTicker(QString str,int offset,QWidget *parent)
    : QLabel(parent) {
    setMinimumWidth(200);
    setMinimumHeight(40);


    qsrand(time(NULL));
    m_showText = str;//显示的文字
    m_charWidth = fontMetrics().width("a");//每个字符的宽度
    m_curIndex = qrand()%(width()/m_charWidth)+2*offset;//当前角码


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TextTicker::updateIndex);
    timer->start(100);
}


TextTicker::~TextTicker() {


}


void TextTicker::paintEvent(QPaintEvent *event) {
    //__super::paintEvent(event);
    QPainter painter(this);
    painter.drawText(0, 30, m_showText.mid(m_curIndex));
    painter.drawText(width() - m_charWidth*m_curIndex, 30, m_showText.left(m_curIndex));
}


void TextTicker::updateIndex() {
    update();
    m_curIndex++;
    if (m_curIndex*m_charWidth > width())
        m_curIndex = 0;
}

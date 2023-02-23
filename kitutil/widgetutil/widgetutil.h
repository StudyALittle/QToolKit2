#ifndef WIDGETUTIL_H
#define WIDGETUTIL_H

#include <QWidget>
#include <QLayout>
#include <QStyle>

namespace wkit {

class WidgetUtil
{
public:
    WidgetUtil();

    /**
     * @brief upCssStyle: 刷新窗口的css样式
     * @param widget
     */
    void upCssStyle(QWidget *widget);

    /**
     * @brief clearLayout: 清除layout下的所有布局和窗口
     * @param layout
     */
    void clearLayout(QLayout *layout);

//    void paintEvent(QPaintEvent *event)
//    {
//        QStyleOption opt;
//        opt.init(this);
//        QPainter p(this);
//        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//        return QWidget::paintEvent(event);
//    }
};

}
#endif // WIDGETUTIL_H

#include "widgetutil.h"

namespace wkit {

WidgetUtil::WidgetUtil()
{

}

void WidgetUtil::upCssStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void WidgetUtil::clearLayout(QLayout *layout)
{
    if (nullptr == layout)
    {
        return;
    }
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        //setParent为NULL，防止删除之后界面不消失
        if(child->widget()) {
            child->widget()->setParent(nullptr);
            delete child->widget();
        } else if (child->layout()) {
            clearLayout(child->layout());
            child->layout()->deleteLater();
        }
        delete child;
    }
}

}
